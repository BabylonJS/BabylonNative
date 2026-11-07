const fs = require("node:fs");
const path = require("node:path");
const os = require("node:os");
const { createHash, randomUUID } = require("node:crypto");
const { Local } = require("browserstack-local");
const { waitForCloudTunnel } = require("./tunnel-readiness.cjs");

function credentials(env = process.env) {
    const user = env.BROWSERSTACK_USERNAME;
    const key = env.BROWSERSTACK_ACCESS_KEY;
    if (!user || !key) {
        throw new Error("Set BROWSERSTACK_USERNAME and BROWSERSTACK_ACCESS_KEY from the approved credential source");
    }
    const basic = Buffer.from(`${user}:${key}`).toString("base64");
    const secrets = [key, user, basic, encodeURIComponent(key), encodeURIComponent(user)];
    const redact = value => secrets.reduce((text, secret) => text.split(secret).join("[REDACTED]"), String(value));
    return { user, key, authorization: `Basic ${basic}`, redact };
}

async function requestJSON(url, options, auth, signal) {
    let response;
    let text;
    try {
        response = await fetch(url, {
            ...options, redirect: "error",
            headers: { Authorization: auth.authorization, ...options.headers },
            signal: signal ? AbortSignal.any([signal, AbortSignal.timeout(180_000)]) : AbortSignal.timeout(180_000)
        });
        text = await response.text();
    } catch (error) {
        throw new Error(auth.redact(`BrowserStack ${options.method || "GET"} ${new URL(url).pathname} request failed: ${error.message}`));
    }
    if (!response.ok) {
        throw new Error(auth.redact(`BrowserStack HTTP ${response.status}: ${text.slice(0, 4000)}`));
    }
    let json;
    try {
        json = JSON.parse(text);
    } catch {
        throw new Error("BrowserStack returned a non-JSON response");
    }
    if (json.value?.error) {
        throw new Error(auth.redact(`Appium ${json.value.error}: ${json.value.message}`));
    }
    return json;
}

async function uploadApp({ app, auth = credentials(), signal }) {
    const bytes = fs.readFileSync(app);
    const sha256 = createHash("sha256").update(bytes).digest("hex");
    const form = new FormData();
    form.append("file", new Blob([bytes]), `Playground${path.extname(app)}`);
    const started = performance.now();
    const upload = await requestJSON("https://api-cloud.browserstack.com/app-automate/upload",
        { method: "POST", body: form }, auth, signal);
    if (typeof upload.app_url !== "string" || !/^bs:\/\/[a-zA-Z0-9]+$/.test(upload.app_url)) {
        throw new Error("BrowserStack upload returned an invalid app identifier");
    }
    return { appUrl: upload.app_url, bytes: bytes.length, sha256, uploadMs: performance.now() - started };
}

async function startSession({ controller, platform, deviceName, osVersion, app, output, uploadedApp, auth = credentials(), signal }) {
    if (!["android", "ios"].includes(platform) || !deviceName || !osVersion || !fs.existsSync(app)) {
        throw new Error("Mobile session needs platform, exact device/OS, and an existing APK or IPA");
    }
    const identifier = `native-${randomUUID()}`;
    const measurements = { platform, deviceName, osVersion, identifier, startedAtMs: Date.now() };
    const privateLogs = fs.mkdtempSync(path.join(os.tmpdir(), "babylon-native-mobile-"));
    const log = path.join(privateLogs, "local.log");
    let local;
    let sessionId;
    let heartbeat;
    let stopped = false;
    let starting = true;
    const originalError = console.error;
    console.error = (...values) => originalError(...values.map(value => auth.redact(value)));
    async function api(url, options = {}) {
        return requestJSON(url, options, auth, starting ? signal : undefined);
    }
    async function stop() {
        if (stopped) {
            return;
        }
        stopped = true;
        starting = false;
        clearInterval(heartbeat);
        const errors = [];
        if (sessionId) {
            try {
                await api(`https://hub.browserstack.com/wd/hub/session/${sessionId}`, { method: "DELETE" });
                measurements.sessionDeleted = true;
            } catch (error) {
                errors.push(error);
            }
        }
        if (local?.pid) {
            try {
                await new Promise((resolve, reject) => local.stop(error => error ? reject(error) : resolve()));
                measurements.localStopped = !local.isRunning();
                if (!measurements.localStopped) {
                    throw new Error("Owned BrowserStack Local process is still running");
                }
            } catch (error) {
                errors.push(new Error(auth.redact(error.message)));
            }
        }
        // Local's diagnostic log is never a publishable artifact.
        try {
            fs.rmSync(log, { force: true });
            fs.rmdirSync(privateLogs);
        } catch (error) {
            errors.push(new Error(`Cannot remove private Local log: ${error.code}`));
        }
        measurements.finishedAtMs = Date.now();
        console.error = originalError;
        if (errors.length) {
            measurements.cleanupErrors = errors.map(error => auth.redact(error.message));
            throw new AggregateError(errors, "Mobile cloud cleanup failed");
        }
    }
    async function diagnostics({ includeNativeLogs = true } = {}) {
        if (!sessionId) {
            throw new Error("No allocated mobile session has diagnostics");
        }
        const data = await api(`https://api-cloud.browserstack.com/app-automate/sessions/${sessionId}.json`);
        const details = data.automation_session;
        if (!details || !Number.isFinite(details.duration)) {
            throw new Error("BrowserStack session diagnostics lack a duration");
        }
        measurements.deviceSeconds = details.duration;
        measurements.cloudStatus = details.status;
        if (includeNativeLogs && details.device_logs_url) {
            const url = new URL(details.device_logs_url);
            if (url.protocol !== "https:" || !url.hostname.endsWith(".browserstack.com")) {
                throw new Error("Unexpected BrowserStack device log origin");
            }
            let response;
            for (let attempt = 0; attempt < 5; attempt++) {
                response = await fetch(url, {
                    headers: { Authorization: auth.authorization }, redirect: "error",
                    signal: AbortSignal.timeout(30_000)
                });
                if (response.status !== 404 || attempt === 4) {
                    break;
                }
                await response.body?.cancel();
                await new Promise(resolve => setTimeout(resolve, 1000 * 2 ** attempt));
            }
            if (!response.ok) {
                throw new Error(`BrowserStack native diagnostics HTTP ${response.status}`);
            }
            const nativeLines = auth.redact(await response.text()).split("\n")
                .filter(line => /BabylonNative|Playground(?:\(|\[)|Uncaught Error|Fatal signal|FATAL EXCEPTION|DEBUG.*(?:pid:|backtrace|#\d+ pc)/.test(line));
            fs.writeFileSync(path.join(output, `${sessionId}-native.log`), nativeLines.join("\n"));
        }
    }
    try {
        fs.mkdirSync(output, { recursive: true });
        const plan = await api("https://api-cloud.browserstack.com/app-automate/plan.json");
        if (!Number.isInteger(plan.parallel_sessions_max_allowed) || !Number.isInteger(plan.parallel_sessions_running)) {
            throw new Error("BrowserStack plan response lacks capacity information");
        }
        measurements.maxParallels = plan.parallel_sessions_max_allowed;
        measurements.runningAtStart = plan.parallel_sessions_running;
        if (measurements.runningAtStart >= measurements.maxParallels) {
            throw new Error("No App Automate parallel slot is currently available");
        }
        const devices = await api("https://api-cloud.browserstack.com/app-automate/devices.json");
        if (!Array.isArray(devices) || !devices.some(device => device.device === deviceName &&
                device.os === platform && String(device.os_version) === osVersion)) {
            throw new Error("Requested device/OS pair is absent from current App Automate inventory");
        }
        const bytes = fs.readFileSync(app);
        measurements.app = { bytes: bytes.length, sha256: createHash("sha256").update(bytes).digest("hex") };
        if (uploadedApp && (uploadedApp.sha256 !== measurements.app.sha256 || uploadedApp.bytes !== bytes.length ||
                typeof uploadedApp.appUrl !== "string" || !/^bs:\/\/[a-zA-Z0-9]+$/.test(uploadedApp.appUrl))) {
            throw new Error("Shared upload does not match the exact mobile package");
        }
        const upload = uploadedApp || await uploadApp({ app, auth, signal });
        measurements.uploadMs = uploadedApp ? 0 : upload.uploadMs;
        measurements.uploadShared = Boolean(uploadedApp);
        local = new Local();
        local.sanitizePath = value => value;
        local.retriesLeft = 0;
        const binary = process.env.BROWSERSTACK_LOCAL_BINARY ||
            path.join(os.homedir(), ".browserstack", os.platform() === "win32" ? "BrowserStackLocal.exe" : "BrowserStackLocal");
        if (!fs.existsSync(binary)) {
            throw new Error("Set BROWSERSTACK_LOCAL_BINARY to an installed BrowserStack Local executable");
        }
        const localStart = performance.now();
        await new Promise((resolve, reject) => local.start({
            key: auth.key, binarypath: binary, localIdentifier: identifier,
            onlyAutomate: true, logFile: log, "enable-logging-for-api": true
        }, error => error ? reject(new Error(auth.redact(error.message))) : resolve()));
        if (!local.isRunning()) {
            throw new Error("BrowserStack Local did not start");
        }
        measurements.localPid = local.pid;
        measurements.localStartupMs = performance.now() - localStart;
        measurements.cloudTunnelRegistration = await waitForCloudTunnel({ key: auth.key, identifier, signal });
        const endpoint = `http://bs-local.com:${controller.port}${controller.devicePath}`;
        const allocationStart = performance.now();
        const session = await api("https://hub.browserstack.com/wd/hub/session", {
            method: "POST", headers: { "Content-Type": "application/json" },
            body: JSON.stringify({ capabilities: { alwaysMatch: {
                platformName: platform,
                "appium:automationName": platform === "ios" ? "XCUITest" : "UiAutomator2",
                "appium:app": upload.appUrl,
                "appium:deviceName": deviceName,
                "appium:platformVersion": osVersion,
                "appium:newCommandTimeout": 180,
                ...(platform === "ios" ? {
                    "appium:autoAcceptAlerts": true,
                    "appium:processArguments": { args: ["--mobile-test-url", endpoint] }
                } : {
                    "appium:autoGrantPermissions": true,
                    "appium:optionalIntentArguments": `--es mobileTestUrl ${endpoint}`
                }),
                "bstack:options": {
                    projectName: "Babylon Native mobile validation",
                    buildName: process.env.GITHUB_RUN_ID || "local-mobile-validation",
                    sessionName: `${platform} native framebuffer validation`,
                    local: true, localIdentifier: identifier,
                    networkLogs: false, debug: false, video: false
                }
            }, firstMatch: [{}] } })
        });
        sessionId = session.value?.sessionId || session.sessionId;
        if (typeof sessionId !== "string" || !/^[a-zA-Z0-9-]+$/.test(sessionId)) {
            throw new Error("Appium did not return a valid session identifier");
        }
        measurements.sessionId = sessionId;
        const capabilities = session.value?.capabilities;
        measurements.actualPlatformVersion = capabilities?.platformVersion || capabilities?.["appium:platformVersion"];
        measurements.allocationMs = performance.now() - allocationStart;
        heartbeat = setInterval(() => {
            api(`https://hub.browserstack.com/wd/hub/session/${sessionId}/window/rect`)
                .catch(error => controller.fail(error));
        }, 60_000);
        const ready = await controller.ready();
        starting = false;
        measurements.ready = ready;
        return { measurements, stop, diagnostics, redact: auth.redact };
    } catch (error) {
        let cleanupError;
        try {
            await stop();
        } catch (caught) {
            cleanupError = caught;
        }
        if (sessionId) {
            try {
                await diagnostics();
            } catch (diagnosticsError) {
                measurements.diagnosticsError = auth.redact(diagnosticsError.message);
            }
        }
        const failure = cleanupError
            ? new AggregateError([new Error(auth.redact(error.message)), cleanupError], "Startup and cleanup failed")
            : new Error(auth.redact(error.message));
        failure.mobileMeasurements = measurements;
        throw failure;
    }
}

module.exports = { credentials, startSession, uploadApp };
