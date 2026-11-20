import { expect } from "chai";

describe("IndexedDB", function () {
    this.timeout(10000);

    let databaseCounter = 0;
    const databaseName = (test: string) =>
        `jsruntimehost-${test}-${++databaseCounter}`;

    const requestResult = <T>(request: IDBRequest<T>): Promise<T> =>
        new Promise((resolve, reject) => {
            request.onsuccess = () => resolve(request.result);
            request.onerror = () => reject(request.error);
        });

    const transactionDone = (transaction: IDBTransaction): Promise<void> =>
        new Promise((resolve, reject) => {
            transaction.oncomplete = () => resolve();
            transaction.onabort = () =>
                reject(transaction.error || new Error("IndexedDB transaction aborted"));
            transaction.onerror = () => {
                // The cancelable request error determines whether the
                // transaction aborts; onabort carries the final result.
            };
        });

    const openDatabase = (
        name: string,
        version: number,
        upgrade: (database: IDBDatabase, transaction: IDBTransaction) => void
    ): Promise<IDBDatabase> =>
        new Promise((resolve, reject) => {
            const request = indexedDB.open(name, version);
            request.onupgradeneeded = () =>
                upgrade(request.result, request.transaction!);
            request.onsuccess = () => resolve(request.result);
            request.onerror = () => reject(request.error);
        });

    it("supports key paths, generated keys, indexes, and storage clones", async function () {
        // Adapted from WPT IndexedDB/keygenerator.any.js,
        // clone-before-keypath-eval.any.js, and structured-clone.any.js.
        const name = databaseName("records");
        const database = await openDatabase(name, 1, db => {
            const store = db.createObjectStore("records", {
                keyPath: "id",
                autoIncrement: true,
            });
            store.createIndex("by-email", "email", { unique: true });
        });

        const bytes = new Uint8Array([1, 2, 3, 4]);
        const record: any = {
            email: "first@example.test",
            bytes,
            alias: bytes,
        };
        record.self = record;

        const write = database.transaction("records", "readwrite");
        const writeDone = transactionDone(write);
        const key = await requestResult(
            write.objectStore("records").add(record)
        );
        expect(key).to.equal(1);
        expect(record.id).to.equal(undefined);
        bytes[0] = 99;
        await writeDone;

        const read = database.transaction("records");
        const stored: any = await requestResult(
            read.objectStore("records").index("by-email").get("first@example.test")
        );
        expect(stored.id).to.equal(1);
        expect(stored.self).to.equal(stored);
        expect(stored.bytes).to.equal(stored.alias);
        expect(Array.from(stored.bytes)).to.eql([1, 2, 3, 4]);

        stored.bytes[1] = 88;
        const readAgain = database.transaction("records");
        const storedAgain: any = await requestResult(
            readAgain.objectStore("records").get(1)
        );
        expect(Array.from(storedAgain.bytes)).to.eql([1, 2, 3, 4]);
        database.close();
    });

    it("stores Blob values without stalling an unrelated transaction", async function () {
        // Regression coverage for Chromium crbug.com/475947902:
        // https://chromium.googlesource.com/chromium/src/+/4e5f6ab9fcf10d98dadb6861184b686d244635fb
        // Chromium regressed by coupling an in-progress Blob write to an
        // unrelated object-store read. Keep disjoint scopes independent and
        // preserve the Blob while cloning it for storage.
        const name = databaseName("blob-concurrency");
        const database = await openDatabase(name, 1, db => {
            db.createObjectStore("blobs");
            db.createObjectStore("other");
        });

        const write = database.transaction("blobs", "readwrite");
        const writeDone = transactionDone(write);
        write.objectStore("blobs").put(
            { blob: new Blob(["abc"], { type: "text/plain" }) },
            "key"
        );

        const read = database.transaction("other", "readonly");
        const readDone = transactionDone(read);
        expect(await requestResult(read.objectStore("other").get("missing")))
            .to.equal(undefined);
        read.commit();
        await readDone;
        await writeDone;

        const stored: any = await requestResult(
            database.transaction("blobs").objectStore("blobs").get("key")
        );
        expect(stored.blob).to.be.an.instanceof(Blob);
        expect(stored.blob.type).to.equal("text/plain");
        expect(await stored.blob.text()).to.equal("abc");
        database.close();
    });

    it("rolls back writes and generated keys when a transaction aborts", async function () {
        // Adapted from WPT IndexedDB/idbtransaction_abort.any.js and
        // transaction-abort-generator-revert.any.js. Applying writes directly
        // to a backing Map is a tempting in-memory implementation shortcut;
        // it violates IndexedDB atomicity when the transaction aborts.
        const name = databaseName("rollback");
        const database = await openDatabase(name, 1, db => {
            db.createObjectStore("records", { autoIncrement: true });
        });

        const transaction = database.transaction("records", "readwrite");
        const aborted = new Promise<void>((resolve, reject) => {
            transaction.onabort = () => resolve();
            transaction.oncomplete = () =>
                reject(new Error("aborted transaction completed"));
        });
        transaction.objectStore("records").add("discarded");
        transaction.abort();
        await aborted;

        const replacement = database.transaction("records", "readwrite");
        const replacementDone = transactionDone(replacement);
        const key = await requestResult(
            replacement.objectStore("records").add("committed")
        );
        await replacementDone;
        expect(key).to.equal(1);
        expect(
            await requestResult(
                database.transaction("records").objectStore("records").get(1)
            )
        ).to.equal("committed");
        database.close();
    });

    it("continues a transaction when a request error is canceled", async function () {
        // Adapted from WPT IndexedDB/idbtransaction.any.js and
        // idbobjectstore-put-unique-index-constraint-is-atomic.any.js.
        // Chromium's abort/error ordering regression tests landed in:
        // https://chromium.googlesource.com/chromium/src/+/7af4cd7effc0e86f7f4a13b740f9315b243bf469
        // Do not abort the whole transaction before the cancelable request
        // error can suppress its default action.
        const name = databaseName("cancel-error");
        const database = await openDatabase(name, 1, db => {
            const store = db.createObjectStore("records", { keyPath: "id" });
            store.createIndex("unique-value", "value", { unique: true });
        });

        const seed = database.transaction("records", "readwrite");
        const seedDone = transactionDone(seed);
        seed.objectStore("records").add({ id: 1, value: "duplicate" });
        await seedDone;

        const transaction = database.transaction("records", "readwrite");
        const done = transactionDone(transaction);
        const duplicate = transaction
            .objectStore("records")
            .add({ id: 2, value: "duplicate" });
        duplicate.onerror = event => event.preventDefault();
        transaction
            .objectStore("records")
            .add({ id: 3, value: "survives" });
        await done;

        expect(
            await requestResult(
                database.transaction("records").objectStore("records").get(2)
            )
        ).to.equal(undefined);
        expect(
            (
                await requestResult<any>(
                    database.transaction("records").objectStore("records").get(3)
                )
            ).value
        ).to.equal("survives");
        database.close();
    });

    it("iterates bounded key ranges in key order", async function () {
        // Adapted from WPT IndexedDB/idbcursor-continue.any.js and
        // idbkeyrange-includes.any.js.
        const name = databaseName("cursor");
        const database = await openDatabase(name, 1, db => {
            db.createObjectStore("records");
        });

        const write = database.transaction("records", "readwrite");
        const writeDone = transactionDone(write);
        for (let key = 5; key >= 1; --key) {
            write.objectStore("records").put(`value-${key}`, key);
        }
        await writeDone;

        const keys: number[] = [];
        const cursorRequest = database
            .transaction("records")
            .objectStore("records")
            .openCursor(IDBKeyRange.bound(2, 4));
        await new Promise<void>((resolve, reject) => {
            cursorRequest.onerror = () => reject(cursorRequest.error);
            cursorRequest.onsuccess = () => {
                const cursor = cursorRequest.result;
                if (!cursor) {
                    resolve();
                    return;
                }
                keys.push(cursor.key as number);
                cursor.continue();
            };
        });
        expect(keys).to.eql([2, 3, 4]);
        database.close();
    });

    it("dispatches versionchange before a blocked upgrade can finish", async function () {
        // Adapted from WPT IndexedDB/idbfactory-open-request-error.any.js and
        // open-request-queue.any.js.
        const name = databaseName("versionchange");
        const first = await openDatabase(name, 1, db => {
            db.createObjectStore("records");
        });

        let oldVersion = -1;
        let newVersion: number | null = -1;
        first.onversionchange = event => {
            oldVersion = event.oldVersion;
            newVersion = event.newVersion;
            first.close();
        };

        const second = await openDatabase(name, 2, () => {});
        expect(oldVersion).to.equal(1);
        expect(newVersion).to.equal(2);
        expect(second.version).to.equal(2);
        second.close();
    });

    it("throws DataCloneError synchronously for unsupported values", async function () {
        // Adapted from WPT IndexedDB/structured-clone.any.js.
        const name = databaseName("clone-error");
        const database = await openDatabase(name, 1, db => {
            db.createObjectStore("records");
        });
        const transaction = database.transaction("records", "readwrite");
        let error: any;
        try {
            transaction.objectStore("records").put(() => {}, 1);
        } catch (caught) {
            error = caught;
        }
        expect(error).to.be.an.instanceof(DOMException);
        expect(error.name).to.equal("DataCloneError");
        transaction.abort();
        database.close();
    });
});
