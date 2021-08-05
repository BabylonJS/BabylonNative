// TODO: use premade reporter (once Console Polyfill is fixed)
function BabylonReporter(runner) {
    const stats = runner.stats;

    runner.on('pass', function (test) {
        console.log(`Passed: ${test.fullTitle()}`);
    });

    runner.on('fail', function (test, err) {
        console.log(`Failed: ${test.fullTitle()} with error: ${err.message}`);
    });

    runner.on('end', function () {
        console.log(`Total passed: ${stats.passes}/${stats.tests} tests`);
    });
}

mocha.setup({ ui: 'bdd', reporter: BabylonReporter });

function makeXHR(method, url) {
    return new Promise( (resolve, reject) => {
        var xhr = new XMLHttpRequest();
        xhr.open(method, url);
        xhr.addEventListener("loadend", () => resolve(xhr));
        xhr.send();
    });
}

const expect = chai.expect;

describe('XMLHTTPRequest', function () {
    this.timeout(0);
    it('should have readyState=4 when load ends', async function () {
        const xhr = await makeXHR("GET", "https://babylonjs.com");
        expect(xhr.readyState).to.equal(4);
    })
    it('should have status=200 for a valid page', async function () {
        const xhr = await makeXHR("GET", "https://babylonjs.com");
        expect(xhr.status).to.equal(200);
    })
    it('should have status=404 for a page that does not exist', async function () {
        const xhr = await makeXHR('GET', 'https://babylonjs.com/invalid');
        expect(xhr.status).to.equal(404);
    })

});


mocha.run(function (failures) {
    // TODO: set exit code if any tests failed
});