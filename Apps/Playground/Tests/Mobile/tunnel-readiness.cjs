async function waitForCloudTunnel({
    key, identifier, timeoutMs = 60_000, request = fetch, signal,
    sleep = ms => new Promise(resolve => setTimeout(resolve, ms))
}) {
    const started = performance.now();
    let polls = 0;
    while (performance.now() - started < timeoutMs) {
        signal?.throwIfAborted();
        let response;
        try {
            response = await request("https://www.browserstack.com/local/v1/list?auth_token=" +
                encodeURIComponent(key) + "&state=running&last=100", {
                redirect: "error", signal: signal
                    ? AbortSignal.any([signal, AbortSignal.timeout(Math.min(15_000, timeoutMs))])
                    : AbortSignal.timeout(Math.min(15_000, timeoutMs))
            });
        } catch {
            // This URL contains the access key; never propagate a fetch error containing it.
            throw new Error("BrowserStack cloud tunnel registration request failed");
        }
        if (!response.ok) {
            throw new Error(`BrowserStack cloud tunnel registration HTTP ${response.status}`);
        }
        let data;
        try {
            data = await response.json();
        } catch {
            throw new Error("BrowserStack cloud tunnel registration returned invalid JSON");
        }
        if (!Array.isArray(data.instances)) {
            throw new Error("BrowserStack cloud tunnel registration lacks an instances collection");
        }
        polls++;
        if (data.instances.some(instance => instance.localIdentifier === identifier && instance.endTime === null)) {
            return { polls, elapsedMs: performance.now() - started };
        }
        await sleep(Math.min(500, Math.max(0, timeoutMs - (performance.now() - started))));
    }
    throw new Error("BrowserStack Local connected locally but did not register an active matching cloud tunnel");
}

module.exports = { waitForCloudTunnel };
