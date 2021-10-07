// TODO: use premade reporter (once Console Polyfill is fixed)
function BabylonReporter(runner) {
    const stats = runner.stats;

    runner.on("pass", test => {
        console.log(`Passed: ${test.fullTitle()} (${test.duration}ms)`);
    });

    runner.on("fail", (test, err) => {
        console.log(`Failed: ${test.fullTitle()} with error: ${err.message} (${test.duration}ms)`);
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
    it("should load unescaped URLs", async function () {
        const xhr = await createRequest("GET", "https://github.com/BabylonJS/Assets/raw/master/meshes/στρογγυλεμένος % κύβος.glb");
        expect(xhr.status).to.equal(200);
    })
    it("should load partially unescaped URLs", async function () {
        const xhr = await createRequest("GET", "https://github.com/BabylonJS/Assets/raw/master/meshes/στρογγυλεμένος%20%%20κύβος.glb");
        expect(xhr.status).to.equal(200);
    })
    it("should load escaped URLs", async function () {
        const xhr = await createRequest("GET", "https://github.com/BabylonJS/Assets/raw/master/meshes/%CF%83%CF%84%CF%81%CE%BF%CE%B3%CE%B3%CF%85%CE%BB%CE%B5%CE%BC%CE%AD%CE%BD%CE%BF%CF%82%20%25%20%CE%BA%CF%8D%CE%B2%CE%BF%CF%82.glb");
        expect(xhr.status).to.equal(200);
    })
    it("should load URLs with unescaped %s", async function () {
        const xhr = await createRequest("GET", "https://github.com/BabylonJS/Assets/raw/master/meshes/%CF%83%CF%84%CF%81%CE%BF%CE%B3%CE%B3%CF%85%CE%BB%CE%B5%CE%BC%CE%AD%CE%BD%CE%BF%CF%82%20%%20%CE%BA%CF%8D%CE%B2%CE%BF%CF%82.glb");
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