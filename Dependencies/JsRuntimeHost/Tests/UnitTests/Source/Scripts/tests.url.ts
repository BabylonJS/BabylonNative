import { expect } from "chai";

// URL
describe("URL", function () {

    // Currently all of the properties that the polyfill has implemented
    interface URLCheckOptions {
        href: string;
        hostname: string;
        origin: string;
        pathname: string;
        search: string;
    }

    function checkURL(url: URL, { href, hostname, origin, pathname, search }: URLCheckOptions): void {
        expect(url).to.have.property("hostname", hostname);
        expect(url).to.have.property("href", href);
        expect(url).to.have.property("origin", origin);
        expect(url).to.have.property("pathname", pathname);
        expect(url).to.have.property("search", search);
    }

    it("should load URL with no pathname / search", function () {
        // Standard URL (No pathname, no search)
        const url = new URL("https://httpbin.org");
        checkURL(url, {
            href: "https://httpbin.org/",
            hostname: "httpbin.org",
            origin: "https://httpbin.org",
            pathname: "/",
            search: ""
        });
    });

    it("should load URL with pathname (no search)", function () {
        // Augment URL with pathname (no search)
        const url = new URL("https://httpbin.org/en-US/docs");
        checkURL(url, {
            href: "https://httpbin.org/en-US/docs",
            hostname: "httpbin.org",
            origin: "https://httpbin.org",
            pathname: "/en-US/docs",
            search: ""
        });
    });

    it("should load URL with pathname and search", function () {
        // Augment URL with pathname and search
        const url = new URL("https://httpbin.org/en-US/docs?foo=1&bar=2");
        checkURL(url, {
            href: "https://httpbin.org/en-US/docs?foo=1&bar=2",
            hostname: "httpbin.org",
            origin: "https://httpbin.org",
            pathname: "/en-US/docs",
            search: "?foo=1&bar=2"
        });
    });

    it("should load URL with pathname and search with multiple key value pairs", function () {
        const url = new URL("https://httpbin.org/en-US/docs?c=3&b=2&a=1&d=4");
        checkURL(url, {
            href: "https://httpbin.org/en-US/docs?c=3&b=2&a=1&d=4",
            hostname: "httpbin.org",
            origin: "https://httpbin.org",
            pathname: "/en-US/docs",
            search: "?c=3&b=2&a=1&d=4"
        });
    });

    it("should update href after URLSearchParams are changed", function () {
        // Augment URL with pathname and search
        const url = new URL("https://httpbin.org/en-US/docs?foo=1&bar=2");
        url.searchParams.set("foo", 999 as any);
        // href should change to reflect searchParams change
        checkURL(url, {
            href: "https://httpbin.org/en-US/docs?foo=999&bar=2",
            hostname: "httpbin.org",
            origin: "https://httpbin.org",
            pathname: "/en-US/docs",
            search: "?foo=999&bar=2"
        });
    });

    it("should update href after URLSearchParams are changed (Starting with 0 params)", function () {
        // Augment URL with pathname and search
        const url = new URL("https://httpbin.org/en-US/docs");
        url.searchParams.set("foo", "999");
        // href should change to reflect searchParams change
        checkURL(url, {
            href: "https://httpbin.org/en-US/docs?foo=999",
            hostname: "httpbin.org",
            origin: "https://httpbin.org",
            pathname: "/en-US/docs",
            search: "?foo=999"
        });
    });

    // -------------------------------- URL Properties --------------------------------

    it("should parse protocol correctly", function () {
        const url = new URL("https://example.com/path");
        expect(url.protocol).to.equal("https:");
    });

    it("should parse port correctly", function () {
        const url = new URL("https://example.com:8080/path");
        expect(url.port).to.equal("8080");
        expect(url.host).to.equal("example.com:8080");
    });

    it("should return empty port when not specified", function () {
        const url = new URL("https://example.com/path");
        expect(url.port).to.equal("");
        expect(url.host).to.equal("example.com");
    });

    it("should parse hash correctly", function () {
        const url = new URL("https://example.com/path#section1");
        expect(url.hash).to.equal("#section1");
    });

    it("should return empty hash when not present", function () {
        const url = new URL("https://example.com/path");
        expect(url.hash).to.equal("");
    });

    it("should parse username and password correctly", function () {
        const url = new URL("https://user:pass@example.com/path");
        expect(url.username).to.equal("user");
        expect(url.password).to.equal("pass");
    });

    it("should return empty username and password when not present", function () {
        const url = new URL("https://example.com/path");
        expect(url.username).to.equal("");
        expect(url.password).to.equal("");
    });

    it("should parse URL with all components", function () {
        const url = new URL("https://user:pass@example.com:8080/path/to/resource?foo=1&bar=2#section");
        expect(url.protocol).to.equal("https:");
        expect(url.username).to.equal("user");
        expect(url.password).to.equal("pass");
        expect(url.hostname).to.equal("example.com");
        expect(url.port).to.equal("8080");
        expect(url.host).to.equal("example.com:8080");
        expect(url.pathname).to.equal("/path/to/resource");
        expect(url.search).to.equal("?foo=1&bar=2");
        expect(url.hash).to.equal("#section");
        expect(url.origin).to.equal("https://example.com:8080");
    });

    // -------------------------------- URL Property Setters --------------------------------

    it("should update protocol via setter", function () {
        const url = new URL("https://example.com/path");
        url.protocol = "http:";
        expect(url.protocol).to.equal("http:");
        expect(url.href).to.equal("http://example.com/path");
    });

    it("should add colon to protocol if missing", function () {
        const url = new URL("https://example.com/path");
        url.protocol = "http";
        expect(url.protocol).to.equal("http:");
    });

    it("should update hostname via setter", function () {
        const url = new URL("https://example.com/path");
        url.hostname = "newhost.com";
        expect(url.hostname).to.equal("newhost.com");
        expect(url.href).to.equal("https://newhost.com/path");
    });

    it("should update host via setter (hostname only)", function () {
        const url = new URL("https://example.com:8080/path");
        url.host = "newhost.com";
        expect(url.hostname).to.equal("newhost.com");
        expect(url.port).to.equal("");
        expect(url.host).to.equal("newhost.com");
    });

    it("should update host via setter (hostname and port)", function () {
        const url = new URL("https://example.com/path");
        url.host = "newhost.com:9090";
        expect(url.hostname).to.equal("newhost.com");
        expect(url.port).to.equal("9090");
        expect(url.host).to.equal("newhost.com:9090");
    });

    it("should update port via setter", function () {
        const url = new URL("https://example.com/path");
        url.port = "3000";
        expect(url.port).to.equal("3000");
        expect(url.href).to.equal("https://example.com:3000/path");
    });

    it("should update pathname via setter", function () {
        const url = new URL("https://example.com/path");
        url.pathname = "/new/path";
        expect(url.pathname).to.equal("/new/path");
        expect(url.href).to.equal("https://example.com/new/path");
    });

    it("should add leading slash to pathname if missing", function () {
        const url = new URL("https://example.com/path");
        url.pathname = "new/path";
        expect(url.pathname).to.equal("/new/path");
    });

    it("should update search via setter", function () {
        const url = new URL("https://example.com/path");
        url.search = "?newkey=newvalue";
        expect(url.search).to.equal("?newkey=newvalue");
        expect(url.href).to.equal("https://example.com/path?newkey=newvalue");
    });

    it("should update search via setter without leading question mark", function () {
        const url = new URL("https://example.com/path");
        url.search = "key=value";
        expect(url.search).to.equal("?key=value");
    });

    it("should update hash via setter", function () {
        const url = new URL("https://example.com/path");
        url.hash = "#newsection";
        expect(url.hash).to.equal("#newsection");
        expect(url.href).to.equal("https://example.com/path#newsection");
    });

    it("should add leading hash if missing", function () {
        const url = new URL("https://example.com/path");
        url.hash = "newsection";
        expect(url.hash).to.equal("#newsection");
    });

    it("should update username via setter", function () {
        const url = new URL("https://example.com/path");
        url.username = "newuser";
        expect(url.username).to.equal("newuser");
        expect(url.href).to.equal("https://newuser@example.com/path");
    });

    it("should update password via setter", function () {
        const url = new URL("https://user@example.com/path");
        url.password = "newpass";
        expect(url.password).to.equal("newpass");
        expect(url.href).to.equal("https://user:newpass@example.com/path");
    });

    it("should update href via setter", function () {
        const url = new URL("https://example.com/path");
        url.href = "https://newsite.com/newpath?q=1#hash";
        expect(url.hostname).to.equal("newsite.com");
        expect(url.pathname).to.equal("/newpath");
        expect(url.search).to.equal("?q=1");
        expect(url.hash).to.equal("#hash");
    });

    // -------------------------------- URL Instance Methods --------------------------------

    it("should return href from toString()", function () {
        const url = new URL("https://example.com/path?foo=1#section");
        expect(url.toString()).to.equal("https://example.com/path?foo=1#section");
    });

    it("should return href from toJSON()", function () {
        const url = new URL("https://example.com/path?foo=1#section");
        expect(url.toJSON()).to.equal("https://example.com/path?foo=1#section");
    });

    // -------------------------------- URL Static Methods --------------------------------

    it("should return true from canParse() for valid absolute URL", function () {
        expect(URL.canParse("https://example.com")).to.equal(true);
    });

    it("should return false from canParse() for invalid URL", function () {
        expect(URL.canParse("not-a-url")).to.equal(false);
    });

    it("should return true from canParse() for relative URL with valid base", function () {
        expect(URL.canParse("/path", "https://example.com")).to.equal(true);
    });

    it("should return false from canParse() for relative URL with invalid base", function () {
        expect(URL.canParse("/path", "not-a-url")).to.equal(false);
    });

    it("should return false from canParse() with no arguments", function () {
        // @ts-expect-error - testing no arguments
        expect(URL.canParse()).to.equal(false);
    });

    it("should return URL object from parse() for valid URL", function () {
        const url = URL.parse("https://example.com/path");
        expect(url).to.not.equal(null);
        expect(url!.hostname).to.equal("example.com");
        expect(url!.pathname).to.equal("/path");
    });

    // TODO: Uncomment this once the the Node-API implementation for Chakra supports throwing errors from constructors.
    // it("should return null from parse() for invalid URL", function () {
    //     const url = URL.parse("not-a-url");
    //     expect(url).to.equal(null);
    // });

    it("should return URL object from parse() with valid base", function () {
        const url = URL.parse("/path", "https://example.com");
        expect(url).to.not.equal(null);
        expect(url!.hostname).to.equal("example.com");
        expect(url!.pathname).to.equal("/path");
    });

    // TODO: Uncomment this once the the Node-API implementation for Chakra supports throwing errors from constructors.
    // it("should return null from parse() with invalid base", function () {
    //     const url = URL.parse("/path", "not-a-url");
    //     expect(url).to.equal(null);
    // });

    // -------------------------------- URL Base URL Resolution --------------------------------

    it("should resolve relative URL with base URL", function () {
        const url = new URL("/path/to/resource", "https://example.com");
        expect(url.href).to.equal("https://example.com/path/to/resource");
    });

    it("should resolve relative path against base URL", function () {
        const url = new URL("resource.html", "https://example.com/path/to/");
        expect(url.href).to.equal("https://example.com/path/to/resource.html");
    });

    it("should resolve relative path against base URL with file", function () {
        const url = new URL("resource.html", "https://example.com/path/to/index.html");
        expect(url.href).to.equal("https://example.com/path/to/resource.html");
    });

    it("should resolve query-only relative URL", function () {
        const url = new URL("?newquery=1", "https://example.com/path");
        expect(url.href).to.equal("https://example.com/path?newquery=1");
    });

    it("should resolve hash-only relative URL", function () {
        const url = new URL("#newhash", "https://example.com/path?query=1");
        expect(url.href).to.equal("https://example.com/path?query=1#newhash");
    });

    it("should resolve protocol-relative URL", function () {
        const url = new URL("//other.com/path", "https://example.com");
        expect(url.href).to.equal("https://other.com/path");
    });

    it("should use absolute URL and ignore base when URL is absolute", function () {
        const url = new URL("https://other.com/otherpath", "https://example.com/path");
        expect(url.href).to.equal("https://other.com/otherpath");
    });

    it("should resolve dot segments in path (single dot)", function () {
        const url = new URL("https://example.com/a/b/./c");
        expect(url.pathname).to.equal("/a/b/c");
    });

    it("should resolve dot segments in path (double dot)", function () {
        const url = new URL("https://example.com/a/b/../c");
        expect(url.pathname).to.equal("/a/c");
    });

    it("should resolve complex dot segments", function () {
        const url = new URL("https://example.com/a/b/c/../d/./e/../f");
        expect(url.pathname).to.equal("/a/b/d/f");
    });

    it("should resolve relative path with dot segments", function () {
        const url = new URL("../sibling/file.html", "https://example.com/path/to/current/");
        expect(url.pathname).to.equal("/path/to/sibling/file.html");
    });

    // -------------------------------- URL Error Handling --------------------------------

    // TODO: Uncomment this once the the Node-API implementation for Chakra supports throwing errors from constructors.
    // it("should throw for invalid URL without base", function () {
    //     expect(() => new URL("not-a-valid-url")).to.throw();
    // });

    // TODO: Uncomment this once the the Node-API implementation for Chakra supports throwing errors from constructors.
    // it("should throw for relative URL without base", function () {
    //     expect(() => new URL("/path/to/resource")).to.throw();
    // });

    // TODO: Uncomment this once the the Node-API implementation for Chakra supports throwing errors from constructors.
    // it("should throw for empty URL constructor", function () {
    //     // @ts-expect-error - testing no arguments
    //     expect(() => new URL()).to.throw();
    // });

    // -------------------------------- URL Different Schemes --------------------------------

    it("should parse file:// URL", function () {
        const url = new URL("file:///path/to/file.txt");
        expect(url.protocol).to.equal("file:");
        expect(url.pathname).to.equal("/path/to/file.txt");
    });

    it("should parse ftp:// URL", function () {
        const url = new URL("ftp://ftp.example.com/file.zip");
        expect(url.protocol).to.equal("ftp:");
        expect(url.hostname).to.equal("ftp.example.com");
    });

    it("should parse ws:// URL", function () {
        const url = new URL("ws://example.com/socket");
        expect(url.protocol).to.equal("ws:");
        expect(url.hostname).to.equal("example.com");
    });

    it("should parse wss:// URL", function () {
        const url = new URL("wss://example.com/socket");
        expect(url.protocol).to.equal("wss:");
        expect(url.hostname).to.equal("example.com");
    });

    it("should parse custom scheme URL", function () {
        const url = new URL("app:///Assets/tests.js");
        expect(url.protocol).to.equal("app:");
        expect(url.pathname).to.equal("/Assets/tests.js");
    });
});

// URL.createObjectURL / revokeObjectURL (blob: URL registry)
describe("URL.createObjectURL", function () {
    this.timeout(0);

    it("mints a blob: URL for a Blob", function () {
        const url = URL.createObjectURL(new Blob(["hello"], { type: "text/plain" }));
        expect(url).to.be.a("string");
        expect(url.indexOf("blob:")).to.equal(0);
        URL.revokeObjectURL(url);
    });

    it("throws when createObjectURL is given a non-Blob", function () {
        expect(() => URL.createObjectURL({} as any)).to.throw();
        expect(() => URL.createObjectURL("not a blob" as any)).to.throw();
    });

    it("identifies real Blobs, and rejects look-alikes, after a script replaces the global Blob", function () {
        // A page or test harness may install its own Blob class. Identity must come from the
        // polyfill's own constructor: a real Blob still works and a foreign instance is a clean
        // TypeError, not a bare "Invalid argument" from unwrapping an object that wraps nothing.
        // ChakraCore has no globalThis; the sloppy-mode Function trick reaches the global object everywhere.
        const globalObject: any = Function("return this")();
        const NativeBlob = Blob;
        const real = new NativeBlob(["hello"], { type: "text/plain" });
        class LookAlikeBlob {
            size = 5;
            type = "text/plain";
        }
        globalObject.Blob = LookAlikeBlob;
        try {
            const url = URL.createObjectURL(real);
            expect(url.indexOf("blob:")).to.equal(0);
            URL.revokeObjectURL(url);
            // Message only: the JSI adapter still surfaces native throws as plain Errors.
            expect(() => URL.createObjectURL(new LookAlikeBlob() as any)).to.throw(/not a Blob/);
        } finally {
            globalObject.Blob = NativeBlob;
        }
    });

    it("exposes createObjectURL as a writable, configurable static (WebIDL operation)", function () {
        const descriptor = Object.getOwnPropertyDescriptor(URL, "createObjectURL")!;
        expect(descriptor.writable, "writable").to.equal(true);
        expect(descriptor.configurable, "configurable").to.equal(true);
        const original = URL.createObjectURL;
        try {
            (URL as any).createObjectURL = () => "blob:replaced";
            expect(URL.createObjectURL(new Blob([]))).to.equal("blob:replaced");
        } finally {
            (URL as any).createObjectURL = original;
        }
    });

    it("resolves a blob: URL through fetch (text + content-type)", async function () {
        const url = URL.createObjectURL(new Blob(["hello blob"], { type: "text/plain" }));
        const response = await fetch(url);
        expect(response.ok).to.equal(true);
        expect(response.status).to.equal(200);
        expect(response.headers.get("content-type")).to.equal("text/plain");
        expect(await response.text()).to.equal("hello blob");
        URL.revokeObjectURL(url);
    });

    it("resolves binary blob bytes through fetch", async function () {
        const bytes = new Uint8Array([1, 2, 3, 4, 250]);
        const url = URL.createObjectURL(new Blob([bytes]));
        const response = await fetch(url);
        const buffer = new Uint8Array(await response.arrayBuffer());
        expect(Array.from(buffer)).to.deep.equal([1, 2, 3, 4, 250]);
        URL.revokeObjectURL(url);
    });

    it("resolves a blob: URL through XMLHttpRequest", async function () {
        const url = URL.createObjectURL(new Blob(["xhr blob"], { type: "text/plain" }));
        const xhr = await new Promise<XMLHttpRequest>((resolve) => {
            const req = new XMLHttpRequest();
            req.open("GET", url);
            req.addEventListener("loadend", () => resolve(req));
            req.send();
        });
        expect(xhr.status).to.equal(200);
        expect(xhr.statusText).to.equal("OK");
        expect(xhr.responseText).to.equal("xhr blob");
        expect(xhr.getResponseHeader("content-type")).to.equal("text/plain");
        URL.revokeObjectURL(url);
    });

    it("fetch rejects after the blob: URL is revoked", async function () {
        const url = URL.createObjectURL(new Blob(["gone"]));
        URL.revokeObjectURL(url);
        let rejected = false;
        try {
            await fetch(url);
        } catch (e) {
            rejected = true;
        }
        expect(rejected).to.equal(true);
    });

    it("XMLHttpRequest reports status 0 and fires 'error' after revoke", async function () {
        const url = URL.createObjectURL(new Blob(["gone"]));
        URL.revokeObjectURL(url);
        const result = await new Promise<{ status: number; errorFired: boolean }>((resolve) => {
            const req = new XMLHttpRequest();
            let errorFired = false;
            req.addEventListener("error", () => { errorFired = true; });
            req.addEventListener("loadend", () => resolve({ status: req.status, errorFired }));
            req.open("GET", url);
            req.send();
        });
        expect(result.status).to.equal(0);
        expect(result.errorFired).to.equal(true);
    });

    it("XMLHttpRequest honors a revoke between open() and send()", async function () {
        const url = URL.createObjectURL(new Blob(["late revoke"]));
        const result = await new Promise<{ status: number; errorFired: boolean }>((resolve) => {
            const req = new XMLHttpRequest();
            let errorFired = false;
            req.addEventListener("error", () => { errorFired = true; });
            req.addEventListener("loadend", () => resolve({ status: req.status, errorFired }));
            req.open("GET", url);
            // Revoked after open() but before send(): the store is re-checked at send() time, so
            // this must surface as a network error rather than serving stale bytes.
            URL.revokeObjectURL(url);
            req.send();
        });
        expect(result.status).to.equal(0);
        expect(result.errorFired).to.equal(true);
    });
});
