declare const hostPlatform: string;
declare const hostEngine: string;
// Origin ("http://127.0.0.1:<port>") of the HTTP server that the test host runs in-process.
declare const loopbackHttpOrigin: string;
// The same shape, for a port that refuses connections.
declare const loopbackClosedPortOrigin: string;
declare const throwPendingAfterCallback: (callback: () => void) => void;
declare const napiGetPropertyNames: (object: any) => string[];
declare const napiGetPropertyNamesRaw: (object: any) => string[];
