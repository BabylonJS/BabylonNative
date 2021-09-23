// TODO: use premade reporter (once Console Polyfill is fixed)
function BabylonReporter(runner) {
    const stats = runner.stats;

    runner.on("pass", test => {
        console.log(`Passed: ${test.fullTitle()}`);
    });

    runner.on("fail", (test, err) => {
        console.log(`Failed: ${test.fullTitle()} with error: ${err.message}`);
    });

    runner.on("end", () => {
        console.log(`Tests passed: ${stats.passes}/${stats.tests}`);
    });
}

mocha.setup({ ui: "bdd", reporter: BabylonReporter });

function createRequest(method, url) {
    return new Promise((resolve, reject) => {
        const xhr = new XMLHttpRequest();
        xhr.open(method, url);
        xhr.addEventListener("loadend", () => resolve(xhr));
        xhr.send();
    });
}

const expect = chai.expect;

describe("XMLHTTPRequest", function () {
    this.timeout(0);
    it("should have readyState=4 when load ends", async function () {
        const xhr = await createRequest("GET", "https://babylonjs.com");
        expect(xhr.readyState).to.equal(4);
    })
    it("should have status=200 for a file that exists", async function () {
        const xhr = await createRequest("GET", "https://babylonjs.com");
        expect(xhr.status).to.equal(200);
    })
    it("should load URLs with escaped spaces", async function () {
        const xhr = await createRequest("GET", "https://github.com/BabylonJS/Assets/raw/master/meshes/aerobatic%20plane.glb");
        expect(xhr.status).to.equal(200);
    })
    it("should load URLs with unescaped spaces", async function () {
        const xhr = await createRequest("GET", "https://github.com/BabylonJS/Assets/raw/master/meshes/aerobatic plane.glb");
        expect(xhr.status).to.equal(200);
    })
    it("should have status=404 for a file that does not exist", async function () {
        const xhr = await createRequest("GET", "https://babylonjs.com/invalid");
        expect(xhr.status).to.equal(404);
    })
    it("should throw something when opening //", async function () {
        function openDoubleSlash() {
            const xhr = new XMLHttpRequest();
            xhr.open("GET", "//");
            xhr.send();
        }
        expect(openDoubleSlash).to.throw();
    })
    it("should throw something when opening a url with no scheme", function () {
        function openNoProtocol() {
            const xhr = new XMLHttpRequest();
            xhr.open("GET", "noscheme.glb");
            xhr.send();
        }
        expect(openNoProtocol).to.throw();
    })
    it("should throw something when sending before opening", function () {
        function sendWithoutOpening() {
            const xhr = new XMLHttpRequest();
            xhr.send();
        }
        expect(sendWithoutOpening).to.throw();
    })
    it("should open a local file", async function () {
        const xhr = await createRequest("GET", "app:///Scripts/tests.js");
        expect(xhr).to.have.property('readyState', 4);
        expect(xhr).to.have.property('status', 200);
        expect(xhr).to.have.property('responseText').with.lengthOf.above(0);
    })
});
describe("RequestFile", function () {
    this.timeout(0);
    it("should throw when requesting a URL with no protocol", function () {
        function RequestFile() {
            BABYLON.FileTools.RequestFile(
                "noprotocol.gltf",
                () => {},
            );
        }
        expect(RequestFile).to.throw();
    })
})

mocha.run(failures => {
    // Test program will wait for code to be set before exiting
    if (failures > 0) {
        // Failure
        SetExitCode(1);
    } else {
        // Success
        SetExitCode(0);
    }
});