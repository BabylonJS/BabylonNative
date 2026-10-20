import { appendFileSync, mkdirSync, readFileSync, writeFileSync } from "node:fs";
import { join } from "node:path";
import { execFileSync } from "node:child_process";
import { fileURLToPath } from "node:url";

export default class ParityReporter {
    constructor(options) {
        this.output = options.output;
        mkdirSync(this.output, { recursive: true });
    }

    onBegin(config, suite) {
        writeFileSync(join(this.output, "scheduled-tests.json"), JSON.stringify(
            suite.allTests().map(test => ({
                id: test.id, title: test.titlePath(), location: test.location,
            })), null, 2));
    }

    onTestEnd(test, result) {
        let history = [];
        try {
            const lines = readFileSync(join(this.output, "history.jsonl"), "utf8").trim().split("\n");
            history = lines.map(line => JSON.parse(line)).findLast(item => item.testId === test.id)?.history ?? [];
        } catch (error) {
            if (error.code !== "ENOENT") {
                throw error;
            }
        }
        appendFileSync(join(this.output, "cases.jsonl"), JSON.stringify({
            id: test.id, title: test.titlePath(), location: test.location,
            status: result.status, expectedStatus: test.expectedStatus,
            startedAt: result.startTime.toISOString(), finishedAt: new Date().toISOString(),
            durationMs: result.duration,
            errors: result.errors.map(error => ({ message: error.message, stack: error.stack })),
            attachments: result.attachments.filter(item => item.path).map(item => ({
                name: item.name, path: item.path, contentType: item.contentType,
            })),
            stdout: result.stdout.map(chunk => String(chunk)),
            stderr: result.stderr.map(chunk => String(chunk)),
            history,
        }) + "\n");
    }

    onError(error) {
        appendFileSync(join(this.output, "runner-errors.jsonl"), JSON.stringify(error) + "\n");
    }

    onEnd(result) {
        writeFileSync(join(this.output, "run-result.json"), JSON.stringify(result, null, 2));
        if (!process.env.NATIVE_PARITY_BROWSER && process.env.NATIVE_PARITY_REPORT === "1") {
            execFileSync(process.execPath, [
                fileURLToPath(new URL("./report-lite-parity.mjs", import.meta.url)),
                process.cwd(), this.output,
                fileURLToPath(new URL("../../Dawn-Lite-Parity.md", import.meta.url)),
            ], { stdio: "inherit" });
        }
    }
}
