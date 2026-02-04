import * as Mocha from "mocha";
import { expect } from "chai";

declare var describe: typeof Mocha.describe;
declare var it: typeof Mocha.it;
declare const setExitCode: (code: number) => void;
declare const fflate: any;

Mocha.setup("bdd");
// @ts-ignore
Mocha.reporter("spec");

describe("InflateZip", function () {
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

    it("Inflate SOG", async function () {
        const { unzipSync } = fflate;
        const xhr = await createRequest("GET", "https://raw.githubusercontent.com/CedricGuillemet/dump/master/hornedlizard/hornedlizard.sog");
        expect(xhr.status).to.equal(200);
        var response = new Uint8Array(xhr.response);
        const unzipped = unzipSync(response) as Record<string, Uint8Array>;
        const files = new Map<string, Uint8Array>();
        for (const [name, content] of Object.entries(unzipped)) {
            files.set(name, content);
        }
        expect(files.size).to.equal(8);
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