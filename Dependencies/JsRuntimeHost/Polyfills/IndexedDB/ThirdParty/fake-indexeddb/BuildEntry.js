import fakeIndexedDB from "fake-indexeddb/lib/fakeIndexedDB";
import FDBCursor from "fake-indexeddb/lib/FDBCursor";
import FDBCursorWithValue from "fake-indexeddb/lib/FDBCursorWithValue";
import FDBDatabase from "fake-indexeddb/lib/FDBDatabase";
import FDBFactory from "fake-indexeddb/lib/FDBFactory";
import FDBIndex from "fake-indexeddb/lib/FDBIndex";
import FDBKeyRange from "fake-indexeddb/lib/FDBKeyRange";
import FDBObjectStore from "fake-indexeddb/lib/FDBObjectStore";
import FDBOpenDBRequest from "fake-indexeddb/lib/FDBOpenDBRequest";
import FDBRequest from "fake-indexeddb/lib/FDBRequest";
import FDBTransaction from "fake-indexeddb/lib/FDBTransaction";
import FDBVersionChangeEvent from "fake-indexeddb/lib/FDBVersionChangeEvent";
import { DOMException as StorageDOMException } from "./StorageClone.js";

if (Array.prototype.findLast === undefined) {
    Object.defineProperty(Array.prototype, "findLast", {
        configurable: true,
        writable: true,
        value(predicate, thisArg) {
            for (let index = this.length - 1; index >= 0; --index) {
                if (predicate.call(thisArg, this[index], index, this)) {
                    return this[index];
                }
            }
            return undefined;
        },
    });
}

if (Object.hasOwn === undefined) {
    Object.defineProperty(Object, "hasOwn", {
        configurable: true,
        writable: true,
        value(object, property) {
            return Object.prototype.hasOwnProperty.call(object, property);
        },
    });
}

const descriptor = value => ({
    value,
    enumerable: false,
    configurable: true,
    writable: true,
});

Object.defineProperties(globalThis, {
    ...(
        typeof globalThis.DOMException === "function"
            ? {}
            : { DOMException: descriptor(StorageDOMException) }
    ),
    indexedDB: descriptor(fakeIndexedDB),
    IDBCursor: descriptor(FDBCursor),
    IDBCursorWithValue: descriptor(FDBCursorWithValue),
    IDBDatabase: descriptor(FDBDatabase),
    IDBFactory: descriptor(FDBFactory),
    IDBIndex: descriptor(FDBIndex),
    IDBKeyRange: descriptor(FDBKeyRange),
    IDBObjectStore: descriptor(FDBObjectStore),
    IDBOpenDBRequest: descriptor(FDBOpenDBRequest),
    IDBRequest: descriptor(FDBRequest),
    IDBTransaction: descriptor(FDBTransaction),
    IDBVersionChangeEvent: descriptor(FDBVersionChangeEvent),
});
