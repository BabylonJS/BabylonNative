import { createNativeHost } from "./lite-parity-page.mjs";
import { appendFileSync, writeFileSync } from "node:fs";
import { dirname, join } from "node:path";

export function createParityFixture(base, browserExpect, options) {
    const histories = new Map();
    const test = base.extend({
        browser: [async ({ browser }, use) => {
            writeFileSync(join(dirname(options.historyFile), "test-runtime.json"), JSON.stringify({
                node: process.version, browser: browser.version(),
            }, null, 2));
            const wrapped = new Proxy(browser, {
                get(target, key) {
                    if (key === "newContext") {
                        return settings => target.newContext({
                            baseURL: options.assetBase,
                            viewport: { width: 1280, height: 720 },
                            deviceScaleFactor: 1,
                            ...settings,
                        });
                    }
                    const value = target[key];
                    return typeof value === "function" ? value.bind(target) : value;
                },
            });
            await use(wrapped);
        }, { scope: "worker" }],
        nativeHost: [async ({}, use) => {
            const host = await createNativeHost(options);
            try {
                await use(host);
            } finally {
                await host.close();
            }
        }, { scope: "worker" }],
        page: async ({ browser, nativeHost }, use, testInfo) => {
            let active;
            let browserContext;
            let sceneFailure;
            let viewport = { width: 1280, height: 720 };
            const listeners = [];
            const history = [];
            async function closeActive() {
                if (active) {
                    if (active.history) {
                        history.push(...active.history);
                    }
                    await active.close();
                    active = undefined;
                }
                if (browserContext) {
                    await browserContext.close();
                    browserContext = undefined;
                }
            }
            const page = new Proxy({
                async goto(url, settings) {
                    await closeActive();
                    const absolute = new URL(url, options.assetBase).href;
                    const isOracle = /\/(?:babylon-ref-scene\d+|demo-ocean-reference)\.html/.test(absolute);
                    sceneFailure = undefined;
                    if (isOracle || process.env.NATIVE_PARITY_BROWSER === "1") {
                        browserContext = await browser.newContext({ viewport });
                        active = await browserContext.newPage();
                        history.push({ backend: isOracle ? "browser-oracle" : "browser-lite", url: absolute });
                    } else {
                        active = await nativeHost.createPage({ browser, testInfo, viewport });
                        active.on("console", message => {
                            if (message.type() === "error") {
                                sceneFailure = new Error("NATIVE_SCENE_ERROR: " + message.text());
                            }
                        });
                        active.on("pageerror", error => {
                            sceneFailure = error;
                        });
                    }
                    for (const [event, listener] of listeners) {
                        active.on(event, listener);
                    }
                    return active.goto(absolute, settings);
                },
                context() {
                    return { browser: () => browser };
                },
                async evaluate(...args) {
                    if (sceneFailure) {
                        throw sceneFailure;
                    }
                    return active.evaluate(...args);
                },
                async waitForFunction(...args) {
                    if (browserContext) {
                        return active.waitForFunction(...args);
                    }
                    let [predicate, arg, settings = {}] = args;
                    if (typeof predicate === "function" && predicate.length === 0 && arg
                        && typeof arg === "object" && Object.keys(arg).every(key => ["timeout", "polling"].includes(key))) {
                        settings = arg;
                        arg = undefined;
                    }
                    const deadline = Date.now() + Math.min(settings.timeout ?? options.testTimeout, options.testTimeout);
                    while (Date.now() < deadline) {
                        if (sceneFailure) {
                            throw sceneFailure;
                        }
                        const value = await active.evaluate(predicate, arg);
                        if (value) {
                            return { jsonValue: async () => value, dispose: async () => {} };
                        }
                        await new Promise(resolve => setTimeout(resolve, 50));
                    }
                    throw new Error("Native parity waitForFunction timed out waiting for " + String(predicate));
                },
                locator(selector) {
                    return new Proxy({ __liteParityLocator: true }, {
                        get(target, key) {
                            if (key === "then") {
                                return undefined;
                            }
                            if (key === "__liteParityLocator") {
                                return true;
                            }
                            if (!active) {
                                throw new Error("Parity locator used before navigation");
                            }
                            const locator = active.locator(selector);
                            const value = locator[key];
                            return typeof value === "function" ? value.bind(locator) : value;
                        },
                    });
                },
                async setViewportSize(size) {
                    viewport = size;
                    if (active) {
                        await active.setViewportSize(size);
                    }
                },
                on(event, listener) {
                    listeners.push([event, listener]);
                    active?.on(event, listener);
                    return page;
                },
                off(event, listener) {
                    const index = listeners.findIndex(item => item[0] === event && item[1] === listener);
                    if (index >= 0) {
                        listeners.splice(index, 1);
                    }
                    active?.off(event, listener);
                    return page;
                },
                async close() {
                    await closeActive();
                },
            }, {
                get(target, key) {
                    if (key in target) {
                        return target[key];
                    }
                    if (!active) {
                        throw new Error(`Parity Page.${String(key)} used before navigation`);
                    }
                    const value = active[key];
                    if (value === undefined) {
                        throw new Error(`Unsupported parity Page.${String(key)}`);
                    }
                    return typeof value === "function" ? value.bind(active) : value;
                },
            });
            try {
                await use(page);
            } finally {
                await closeActive();
                histories.set(testInfo.testId, history);
                appendFileSync(options.historyFile, JSON.stringify({
                    testId: testInfo.testId, title: testInfo.title, history,
                }) + "\n");
            }
        },
    });
    const allCases = new Proxy(test, {
        get(target, key) {
            if (key === "skip") {
                return (...args) => {
                    if (args[0]) {
                        console.log("LITE_PARITY_ORIGINAL_SKIP:", String(args.find(arg => typeof arg === "string") ?? "catalog quarantine"));
                    }
                    if (typeof args.at(-1) === "function") {
                        return target(...args);
                    }
                };
            }
            if (key === "setTimeout") {
                return timeout => target.setTimeout(Math.min(timeout, options.testTimeout));
            }
            return target[key];
        },
    });

    function expect(target, message) {
        if (!target?.__liteParityLocator) {
            return browserExpect(target, message);
        }
        function match(negated) {
            return {
                get not() { return match(!negated); },
                async toHaveAttribute(name, expected, settings = {}) {
                    const poll = browserExpect.poll(async () => {
                        const value = await target.getAttribute(name);
                        const matches = expected instanceof RegExp
                            ? value !== null && expected.test(value) : value === expected;
                        return negated ? !matches : matches;
                    }, { timeout: settings.timeout ?? 5000, message });
                    await poll.toBe(true);
                },
                async toBeVisible(settings = {}) {
                    await browserExpect.poll(async () => {
                        const box = await target.boundingBox();
                        const visible = !!box && box.width > 0 && box.height > 0;
                        return negated ? !visible : visible;
                    }, { timeout: settings.timeout ?? 5000, message }).toBe(true);
                },
            };
        }
        return match(false);
    }
    for (const key of ["poll", "soft", "configure", "extend", "getState", "setState"]) {
        if (browserExpect[key]) {
            expect[key] = browserExpect[key].bind(browserExpect);
        }
    }
    async function acquireContext(browser, viewport = { width: 1280, height: 720 }) {
        const context = await browser.newContext({ viewport });
        return { context, release: () => context.close() };
    }
    async function acquireReferencePage(browser, viewport) {
        const { context, release } = await acquireContext(browser, viewport);
        return { page: await context.newPage(), release };
    }
    return {
        test: allCases, expect, REUSE_BROWSER: false,
        acquireContext, acquireReferencePage, reusedContext: () => undefined,
    };
}
