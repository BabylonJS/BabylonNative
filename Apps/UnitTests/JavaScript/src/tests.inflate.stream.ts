import * as Mocha from "mocha";
import { expect } from "chai";

declare var describe: typeof Mocha.describe;
declare var it: typeof Mocha.it;
declare const setExitCode: (code: number) => void;

Mocha.setup("bdd");
// @ts-ignore
Mocha.reporter("spec");

describe("InflateStream", function () {
    this.timeout(0);

    function createRequest(method: string, url: string): Promise<XMLHttpRequest> {
        return new Promise((resolve) => {
            const xhr = new XMLHttpRequest();
            xhr.open(method, url);
            xhr.responseType = "arraybuffer";
            xhr.addEventListener("loadend", () => resolve(xhr));
            xhr.send();
        });
    }

    it("Inflate GZip", async function () {
        const xhr = await createRequest("GET", "https://raw.githubusercontent.com/CedricGuillemet/dump/master/hornedlizard/hornedlizard.spz");
        expect(xhr.status).to.equal(200);
        var data = new Uint8Array(xhr.response);

        const readableStream = new ReadableStream({
            start(controller) {
                controller.enqueue(new Uint8Array(data)); // Enqueue the ArrayBuffer as a Uint8Array
                controller.close();
            },
        });

        // Use GZip DecompressionStream
        const decompressionStream = new DecompressionStream("gzip");
        const decompressedStream = readableStream.pipeThrough(decompressionStream);

        await new Response(decompressedStream)
            .arrayBuffer()
            .then((buffer) => {
                expect(buffer.byteLength).to.equal(50318928);
            });
    });
});

function runTests() {
    mocha.run((failures: number) => {
        // Test program will wait for code to be set before exiting
        if (failures > 0) {
            // Failure
            setExitCode(1);
        } else {
            // Success
            setExitCode(0);
        }
    });
}

runTests();