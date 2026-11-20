class StorageDOMException extends Error {
    constructor(message = "", name = "Error") {
        super(String(message));
        this.name = String(name);
        const codes = {
            IndexSizeError: 1,
            HierarchyRequestError: 3,
            WrongDocumentError: 4,
            InvalidCharacterError: 5,
            NoModificationAllowedError: 7,
            NotFoundError: 8,
            NotSupportedError: 9,
            InUseAttributeError: 10,
            InvalidStateError: 11,
            SyntaxError: 12,
            InvalidModificationError: 13,
            NamespaceError: 14,
            InvalidAccessError: 15,
            TypeMismatchError: 17,
            SecurityError: 18,
            NetworkError: 19,
            AbortError: 20,
            URLMismatchError: 21,
            QuotaExceededError: 22,
            TimeoutError: 23,
            InvalidNodeTypeError: 24,
            DataCloneError: 25,
        };
        Object.defineProperty(this, "code", {
            value: codes[this.name] || 0,
            enumerable: true,
        });
    }
}

const DOMExceptionImplementation =
    typeof globalThis.DOMException === "function"
        ? globalThis.DOMException
        : StorageDOMException;

const AggregateErrorImplementation =
    typeof globalThis.AggregateError === "function"
        ? globalThis.AggregateError
        : class AggregateError extends Error {
              constructor(errors, message = "") {
                  super(String(message));
                  this.name = "AggregateError";
                  this.errors = Array.from(errors);
              }
          };

function dataCloneError(message = "The object could not be cloned.") {
    return new DOMExceptionImplementation(message, "DataCloneError");
}

function cloneStorageValue(value, seen = new Map()) {
    if (
        value === null ||
        typeof value === "string" ||
        typeof value === "boolean" ||
        typeof value === "number" ||
        typeof value === "bigint" ||
        value === undefined
    ) {
        return value;
    }
    if (typeof value === "function" || typeof value === "symbol") {
        throw dataCloneError();
    }
    if (seen.has(value)) {
        return seen.get(value);
    }
    if (
        (typeof WeakMap !== "undefined" && value instanceof WeakMap) ||
        (typeof WeakSet !== "undefined" && value instanceof WeakSet) ||
        (typeof Promise !== "undefined" && value instanceof Promise) ||
        (typeof SharedArrayBuffer !== "undefined" &&
            value instanceof SharedArrayBuffer)
    ) {
        throw dataCloneError();
    }

    let clone;
    if (typeof Blob !== "undefined" && value instanceof Blob) {
        if (typeof File !== "undefined" && value instanceof File) {
            clone = new File([value], value.name, {
                type: value.type,
                lastModified: value.lastModified,
            });
        } else {
            clone = new Blob([value], { type: value.type });
        }
        seen.set(value, clone);
        return clone;
    }
    if (value instanceof ArrayBuffer) {
        clone = value.slice(0);
        seen.set(value, clone);
        return clone;
    }
    if (ArrayBuffer.isView(value)) {
        const buffer = cloneStorageValue(value.buffer, seen);
        clone =
            value instanceof DataView
                ? new DataView(buffer, value.byteOffset, value.byteLength)
                : new value.constructor(buffer, value.byteOffset, value.length);
        seen.set(value, clone);
        return clone;
    }
    if (Array.isArray(value)) {
        clone = new Array(value.length);
        seen.set(value, clone);
        for (const key of Object.keys(value)) {
            clone[key] = cloneStorageValue(value[key], seen);
        }
        return clone;
    }
    if (value instanceof Date) {
        clone = new Date(value.getTime());
    } else if (value instanceof RegExp) {
        clone = new RegExp(value.source, value.flags);
        clone.lastIndex = value.lastIndex;
    } else if (value instanceof Map) {
        clone = new Map();
        seen.set(value, clone);
        for (const [key, entry] of value) {
            clone.set(
                cloneStorageValue(key, seen),
                cloneStorageValue(entry, seen)
            );
        }
        return clone;
    } else if (value instanceof Set) {
        clone = new Set();
        seen.set(value, clone);
        for (const entry of value) {
            clone.add(cloneStorageValue(entry, seen));
        }
        return clone;
    } else if (value instanceof Error) {
        clone = new Error(value.message);
        clone.name = value.name;
        if ("stack" in value) {
            clone.stack = value.stack;
        }
    } else {
        clone = {};
    }

    seen.set(value, clone);
    for (const key of Object.keys(value)) {
        clone[key] = cloneStorageValue(value[key], seen);
    }
    return clone;
}

export {
    AggregateErrorImplementation as AggregateError,
    DOMExceptionImplementation as DOMException,
    cloneStorageValue as structuredClone,
};
