const http = require("node:http");
const { randomUUID } = require("node:crypto");

class MobileController {
    constructor(onEvent = async () => {}) {
        this.onEvent = onEvent;
        this.token = randomUUID();
        this.nextId = 1;
        this.closed = false;
        this.readyPromise = new Promise((resolve, reject) => {
            this.readyResolve = resolve;
            this.readyReject = reject;
        });
        // Startup may fail before a caller begins waiting for the device.
        this.readyPromise.catch(() => {});
    }

    async listen() {
        this.server = http.createServer((request, response) => {
            this.handle(request, response).catch(error => {
                this.fail(error);
                if (!response.headersSent) {
                    response.writeHead(400);
                }
                response.end("Invalid mobile controller message");
            });
        });
        await new Promise((resolve, reject) => {
            this.server.once("error", reject);
            this.server.listen(0, "127.0.0.1", resolve);
        });
        this.port = this.server.address().port;
        this.devicePath = `/device/${this.token}`;
        return this;
    }

    async handle(request, response) {
        if (request.method === "GET" && request.url === `${this.devicePath}/next`) {
            if (this.poll) {
                throw new Error("Multiple device polls on one controller");
            }
            if (this.command) {
                response.setHeader("Content-Type", "application/json");
                response.end(JSON.stringify(this.command));
                this.command = undefined;
            } else {
                this.poll = response;
                const timeout = setTimeout(() => {
                    response.writeHead(204);
                    response.end();
                }, 20_000);
                response.once("close", () => {
                    clearTimeout(timeout);
                    if (this.poll === response) {
                        this.poll = undefined;
                    }
                });
            }
            return;
        }
        if (request.method !== "POST" || request.url !== `${this.devicePath}/message`) {
            response.writeHead(404);
            response.end();
            return;
        }
        const chunks = [];
        let length = 0;
        for await (const chunk of request) {
            length += chunk.length;
            if (length > 8 * 1024 * 1024) {
                throw new Error("Mobile controller payload exceeds 8 MiB");
            }
            chunks.push(chunk);
        }
        const message = JSON.parse(Buffer.concat(chunks).toString("utf8"));
        if (message.type === "ready") {
            if (this.device || message.protocol !== 2 || typeof message.babylonVersion !== "string" ||
                    typeof message.graphicsApi !== "string") {
                throw new Error("Invalid or duplicate mobile ready message");
            }
            this.device = message;
            this.readyResolve(message);
        } else if (message.type === "result") {
            if (!this.active || message.id !== this.active.id) {
                throw new Error("Result does not match the active mobile command");
            }
            const active = this.active;
            if (!Array.isArray(message.console) || message.console.length > 50) {
                throw new Error("Invalid native console batch");
            }
            for (const item of message.console) {
                if (!item || !["log", "warn", "error"].includes(item.level) || typeof item.text !== "string") {
                    throw new Error("Invalid native console message");
                }
                await this.onEvent(item);
            }
            this.clearActive();
            if (message.error) {
                const error = new Error(message.error);
                error.deviceStack = message.stack;
                active.reject(error);
            } else {
                active.resolve(message.value);
            }
        } else {
            throw new Error("Unknown native transport message");
        }
        response.end("OK");
    }

    async ready(timeoutMs = 120_000) {
        let timer;
        try {
            return await Promise.race([
                this.readyPromise,
                new Promise((_, reject) => {
                    timer = setTimeout(() => reject(new Error("Mobile device startup timed out")), timeoutMs);
                })
            ]);
        } finally {
            clearTimeout(timer);
        }
    }

    evaluate(fn, arg, { timeoutMs = 120_000, activityTimeoutMs = timeoutMs } = {}) {
        if (this.closed || this.failure || this.active || !this.device) {
            return Promise.reject(this.failure || new Error("Mobile device is not ready for another command"));
        }
        const argument = arg === undefined ? "undefined" : JSON.stringify(arg);
        if (argument === undefined) {
            return Promise.reject(new Error("Mobile arguments must be JSON-serializable"));
        }
        const source = typeof fn === "string" ? fn : `(${fn.toString()})(${argument})`;
        const id = this.nextId++;
        const promise = new Promise((resolve, reject) => {
            this.active = { id, resolve, reject, activityTimeoutMs };
        });
        this.active.timer = setTimeout(() => this.timeout(), timeoutMs);
        this.armActivityDeadline();
        this.command = { id, source };
        if (this.poll) {
            const poll = this.poll;
            this.poll = undefined;
            poll.setHeader("Content-Type", "application/json");
            poll.end(JSON.stringify(this.command));
            this.command = undefined;
        }
        return promise;
    }

    armActivityDeadline() {
        clearTimeout(this.active.activityTimer);
        this.active.activityTimer = setTimeout(() => this.timeout(), this.active.activityTimeoutMs);
    }

    timeout() {
        const error = new Error("Mobile command timed out; the app session must be replaced");
        error.name = "MobileCommandTimeout";
        this.fail(error);
    }

    clearActive() {
        clearTimeout(this.active?.timer);
        clearTimeout(this.active?.activityTimer);
        this.active = undefined;
    }

    fail(error) {
        this.failure = error;
        this.readyReject(error);
        this.active?.reject(error);
        this.clearActive();
        this.command = undefined;
        if (this.poll) {
            this.poll.writeHead(503);
            this.poll.end();
            this.poll = undefined;
        }
    }

    async close() {
        if (this.closed) {
            return;
        }
        this.closed = true;
        this.fail(new Error("Mobile controller closed"));
        if (this.server) {
            this.server.closeAllConnections();
            await new Promise((resolve, reject) => this.server.close(error => error ? reject(error) : resolve()));
        }
    }
}

module.exports = { MobileController };
