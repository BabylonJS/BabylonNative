function caseKey(record) {
    if (record.location?.file && Array.isArray(record.title)) {
        return JSON.stringify([
            record.location.file,
            record.location.line,
            record.location.column,
            record.title,
        ]);
    }
    return record.id;
}

export function latestCases(records) {
    return Array.from(new Map(records.map(record => [
        caseKey(record),
        record,
    ])).values());
}

export function summarizeRun(cases, scheduled, invocationStatus) {
    const completedKeys = new Set(cases.map(caseKey));
    const pending = scheduled?.filter(record => !completedKeys.has(caseKey(record))).length ?? 0;
    const status = !invocationStatus || pending > 0 ? "IN PROGRESS / incomplete"
        : cases.some(record => record.status !== "passed") ||
            (!cases.length && invocationStatus !== "passed") ? "failed" : "passed";
    return { status, pending };
}

export function metricsForCases(metrics, cases) {
    const byId = new Map(cases.map(record => [record.id, record]));
    return metrics.filter(metric => {
        if (!metric.testId) {
            return true;
        }
        const record = byId.get(metric.testId);
        if (!record) {
            return false;
        }
        if (!record.startedAt || !metric.recordedAt) {
            return true;
        }
        const timestamp = Date.parse(metric.recordedAt);
        return timestamp >= Date.parse(record.startedAt) &&
            (!record.finishedAt || timestamp <= Date.parse(record.finishedAt));
    });
}
