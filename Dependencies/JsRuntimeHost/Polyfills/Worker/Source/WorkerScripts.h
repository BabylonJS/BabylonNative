#pragma once

namespace Babylon::Polyfills::Internal::WorkerScripts
{
    inline constexpr char Common[] = 
        R"JSRH(
(() => {
  'use strict';
  const g = globalThis;
  if (g.__jsrhWorkerCommonInstalled) return;

  const listeners = new WeakMap();
  const handlers = new WeakMap();

  const DOMException = typeof g.DOMException === 'function'
    ? g.DOMException
    : class DOMException extends Error {
      constructor(message = '', name = 'Error') {
        super(String(message));
        this.name = String(name);
        const codes = { IndexSizeError: 1, HierarchyRequestError: 3,
          WrongDocumentError: 4, InvalidCharacterError: 5,
          NoModificationAllowedError: 7, NotFoundError: 8,
          NotSupportedError: 9, InUseAttributeError: 10,
          InvalidStateError: 11, SyntaxError: 12, InvalidModificationError: 13,
          NamespaceError: 14, InvalidAccessError: 15, TypeMismatchError: 17,
          SecurityError: 18, NetworkError: 19, AbortError: 20,
          URLMismatchError: 21, QuotaExceededError: 22, TimeoutError: 23,
          InvalidNodeTypeError: 24, DataCloneError: 25 };
        Object.defineProperty(this, 'code', {
          value: codes[this.name] || 0, enumerable: true
        });
      }
    };

  class Event {
    constructor(type, init = {}) {
      if (arguments.length === 0) throw new TypeError('Event type is required');
      this._type = String(type);
      this._bubbles = Boolean(init.bubbles);
      this._cancelable = Boolean(init.cancelable);
      this._composed = Boolean(init.composed);
      this._target = null;
      this._currentTarget = null;
      this._defaultPrevented = false;
      this._stopped = false;
      this._immediateStopped = false;
      this._timeStamp = Date.now();
    }
    get type() { return this._type; }
    get target() { return this._target; }
    get srcElement() { return this._target; }
    get currentTarget() { return this._currentTarget; }
    get eventPhase() { return this._currentTarget === null ? 0 : 2; }
    get bubbles() { return this._bubbles; }
    get cancelable() { return this._cancelable; }
    get composed() { return this._composed; }
    get defaultPrevented() { return this._defaultPrevented; }
    get timeStamp() { return this._timeStamp; }
    get isTrusted() { return false; }
    preventDefault() { if (this._cancelable) this._defaultPrevented = true; }
    stopPropagation() { this._stopped = true; }
    stopImmediatePropagation() {
      this._stopped = true;
      this._immediateStopped = true;
    }
    composedPath() { return this._target === null ? [] : [this._target]; }
  }
  Event.NONE = 0;
  Event.CAPTURING_PHASE = 1;
  Event.AT_TARGET = 2;
  Event.BUBBLING_PHASE = 3;

  class MessageEvent extends Event {
    constructor(type, init = {}) {
      super(type, init);
      this.data = init.data === undefined ? null : init.data;
      this.origin = init.origin === undefined ? '' : String(init.origin);
      this.lastEventId = init.lastEventId === undefined ? '' : String(init.lastEventId);
      this.source = init.source === undefined ? null : init.source;
      this.ports = init.ports === undefined ? [] : Array.from(init.ports);
    }
  }

  class ErrorEvent extends Event {
    constructor(type, init = {}) {
      super(type, Object.assign({ cancelable: true }, init));
      this.message = init.message === undefined ? '' : String(init.message);
      this.filename = init.filename === undefined ? '' : String(init.filename);
      this.lineno = Number(init.lineno || 0);
      this.colno = Number(init.colno || 0);
      this.error = init.error === undefined ? null : init.error;
    }
  }

  function optionCapture(options) {
    return typeof options === 'boolean' ? options : Boolean(options && options.capture);
  }

  class EventTarget {
    addEventListener(type, callback, options = false) {
      if (callback === null || callback === undefined) return;
      if (typeof callback !== 'function' &&
          (typeof callback !== 'object' || typeof callback.handleEvent !== 'function')) return;
      type = String(type);
      let byType = listeners.get(this);
      if (!byType) listeners.set(this, byType = new Map());
      let list = byType.get(type);
      if (!list) byType.set(type, list = []);
      const capture = optionCapture(options);
      if (list.some(x => x.callback === callback && x.capture === capture)) return;
      const entry = { callback, capture, once: Boolean(options && options.once) };
      list.push(entry);
      if (options && options.signal) {
        if (options.signal.aborted) {
          list.splice(list.indexOf(entry), 1);
        } else if (typeof options.signal.addEventListener === 'function') {
          options.signal.addEventListener('abort', () => {
            this.removeEventListener(type, callback, capture);
          }, { once: true });
        }
      }
    }

    removeEventListener(type, callback, options = false) {
      const byType = listeners.get(this);
      const list = byType && byType.get(String(type));
      if (!list) return;
      const capture = optionCapture(options);
      const index = list.findIndex(x => x.callback === callback && x.capture === capture);
      if (index !== -1) list.splice(index, 1);
    }

    dispatchEvent(event) {
      if (!(event instanceof Event)) throw new TypeError('Argument 1 is not an Event');
      if (event._currentTarget !== null) throw new DOMException('Event is already being dispatched', 'InvalidStateError');
      event._target = this;
      event._currentTarget = this;
      event._immediateStopped = false;
      const byType = listeners.get(this);
      const snapshot = byType && byType.get(event.type) ? byType.get(event.type).slice() : [];
      for (const entry of snapshot) {
        if (event._immediateStopped) break;
        const current = byType && byType.get(event.type);
        if (!current || current.indexOf(entry) === -1) continue;
        if (entry.once) this.removeEventListener(event.type, entry.callback, entry.capture);
        if (typeof entry.callback === 'function') entry.callback.call(this, event);
        else entry.callback.handleEvent.call(entry.callback, event);
      }
      if (!event._immediateStopped) {
        const byTypeHandler = handlers.get(this);
        const handler = byTypeHandler && byTypeHandler.get(event.type);
        // EventHandler attributes retain arbitrary objects, but only callable
        // values are invoked (matching Web IDL's EventHandler processing).
        if (typeof handler === 'function') handler.call(this, event);
      }
      event._currentTarget = null;
      return !event.defaultPrevented;
    }
  }

  function installHandler(target, type) {
    const receiver = value =>
      ((typeof value === 'object' && value !== null) || typeof value === 'function')
        ? value
        : target;
    Object.defineProperty(target, 'on' + type, {
      configurable: true,
      enumerable: true,
      get() {
        // JavaScriptCore can call an accessor installed directly on its
        // engine-owned global object without an object receiver for an
        // unqualified assignment such as `onmessage = callback`.
        const byType = handlers.get(receiver(this));
        return byType && byType.has(type) ? byType.get(type) : null;
      },
      set(value) {
        const owner = receiver(this);
        let byType = handlers.get(owner);
        if (!byType) handlers.set(owner, byType = new Map());
        if ((typeof value === 'object' && value !== null) || typeof value === 'function') {
          byType.set(type, value);
        } else {
          byType.delete(type);
        }
      }
    });
  }

  function dataCloneError(message) {
    return new DOMException(message, 'DataCloneError');
  }

  function normalizeTransfers(argument) {
    if (argument === undefined || argument === null) return [];
    const value = Array.isArray(argument) ? argument : argument.transfer;
    if (value === undefined || value === null) return [];
    try { return Array.from(value); }
    catch (_) { throw new TypeError('transfer must be an iterable'); }
  }

  function serialize(root, transferArgument) {
    const transferList = normalizeTransfers(transferArgument);
    const buffers = [];
    const bufferIndexes = new Map();
    const transferSet = new Set();

    function addBuffer(buffer) {
      let index = bufferIndexes.get(buffer);
      if (index === undefined) {
        index = buffers.length;
        bufferIndexes.set(buffer, index);
        buffers.push(buffer);
      }
      return index;
    }

    for (const value of transferList) {
      if (!(value instanceof ArrayBuffer)) throw dataCloneError('Only ArrayBuffer transfer is supported');
      if (transferSet.has(value)) throw dataCloneError('Transfer list contains a duplicate ArrayBuffer');
      if (value.detached === true) throw dataCloneError('A detached ArrayBuffer cannot be transferred');
      transferSet.add(value);
      addBuffer(value);
    }

    const seen = new Map();
    const nodes = [];
    const ref = id => ['r', id];
    const ownProperties = (value, skip) => {
      const result = [];
      for (const key of Object.keys(value)) {
        if (!skip || !skip(key)) result.push([key, encode(value[key])]);
      }
      return result;
    };

    function encode(value) {
      if (value === null || typeof value === 'string' || typeof value === 'boolean') return value;
      if (value === undefined) return ['u'];
      if (typeof value === 'number') {
        if (Number.isNaN(value)) return ['n', 'nan'];
        if (value === Infinity) return ['n', 'inf'];
        if (value === -Infinity) return ['n', '-inf'];
        if (Object.is(value, -0)) return ['n', '-0'];
        return value;
      }
      if (typeof value === 'bigint') return ['bi', value.toString()];
      if (typeof value === 'symbol' || typeof value === 'function') {
        throw dataCloneError('The value could not be cloned');
      }
      if (typeof SharedArrayBuffer !== 'undefined' && value instanceof SharedArrayBuffer) {
        throw dataCloneError('SharedArrayBuffer is not available across native runtimes');
      }
      if (seen.has(value)) return ref(seen.get(value));

      const id = nodes.length;
      seen.set(value, id);
      nodes.push(null);

      if (value instanceof ArrayBuffer) {
        nodes[id] = { t: 'ab', b: addBuffer(value) };
      } else if (ArrayBuffer.isView(value)) {
        if (value instanceof DataView) {
          nodes[id] = { t: 'dv', b: encode(value.buffer), o: value.byteOffset, l: value.byteLength };
        } else {
          nodes[id] = { t: 'ta', c: value.constructor.name, b: encode(value.buffer),
            o: value.byteOffset, l: value.length };
        }
      } else if (Array.isArray(value)) {
        const items = [];
        for (let i = 0; i < value.length; ++i) {
          items.push(Object.prototype.hasOwnProperty.call(value, i) ? encode(value[i]) : ['h']);
        }
        nodes[id] = { t: 'a', i: items,
          p: ownProperties(value, key => key === 'length' || /^(0|[1-9][0-9]*)$/.test(key)) };
      } else if (value instanceof Date) {
        nodes[id] = { t: 'd', v: value.getTime(), p: ownProperties(value) };
      } else if (value instanceof RegExp) {
        nodes[id] = { t: 're', s: value.source, f: value.flags, x: value.lastIndex,
          p: ownProperties(value) };
      } else if (value instanceof Map) {
        nodes[id] = { t: 'm', e: Array.from(value, pair => [encode(pair[0]), encode(pair[1])]),
          p: ownProperties(value) };
      } else if (value instanceof Set) {
        nodes[id] = { t: 's', e: Array.from(value, encode), p: ownProperties(value) };
      } else if (value instanceof Error) {
        nodes[id] = { t: 'e', n: value.name, m: value.message, s: value.stack || '',
          p: ownProperties(value, key => key === 'name' || key === 'message' || key === 'stack') };
      } else if (value instanceof WeakMap || value instanceof WeakSet || value instanceof Promise) {
        throw dataCloneError('The value could not be cloned');
      } else {
        nodes[id] = { t: 'o', z: Object.getPrototypeOf(value) === null, p: ownProperties(value) };
      }
      return ref(id);
    }

    // Keep the buffers used for native byte extraction distinct from the
    // transfer targets. In JavaScriptCore, exposing a source buffer's backing
    // pointer through the public C API before ArrayBuffer.prototype.transfer()
    // can prevent the original JS wrapper from observing the detach. Slicing
    // first also makes the required copy-before-detach ordering explicit for
    // every backend.
    return {
      json: JSON.stringify({ root: encode(root), nodes }),
      buffers: buffers.map(buffer => buffer.slice(0)),
      transferBuffers: transferList
    };
  }

  function deserialize(json, buffers) {
    const graph = JSON.parse(json);
    const nodes = graph.nodes;
    const values = new Array(nodes.length);
    const built = new Array(nodes.length).fill(false);

    function decode(value) {
      if (!Array.isArray(value)) return value;
      switch (value[0]) {
        case 'u': return undefined;
        case 'h': return undefined;
        case 'n': return value[1] === 'nan' ? NaN : value[1] === 'inf' ? Infinity :
          value[1] === '-inf' ? -Infinity : -0;
        case 'bi': return BigInt(value[1]);
        case 'r': return build(value[1]);
        default: throw dataCloneError('Invalid native structured-clone record');
      }
    }

    function properties(target, entries) {
      for (const [key, value] of entries || []) {
        Object.defineProperty(target, key, {
          value: decode(value), writable: true, enumerable: true, configurable: true
        });
      }
    }

    function build(id) {
      if (built[id]) return values[id];
      const node = nodes[id];
      if (!node) throw dataCloneError('Invalid native structured-clone node');
      let value;
      switch (node.t) {
        case 'a': value = []; break;
        case 'o': value = node.z ? Object.create(null) : {}; break;
        case 'd': value = new Date(node.v); break;
        case 're': value = new RegExp(node.s, node.f); value.lastIndex = node.x; break;
        case 'm': value = new Map(); break;
        case 's': value = new Set(); break;
        case 'e': value = new Error(node.m); value.name = node.n; value.stack = node.s; break;
        case 'ab': value = buffers[node.b]; break;
        case 'dv': value = new DataView(decode(node.b), node.o, node.l); break;
        case 'ta': {
          const Constructor = g[node.c];
          if (typeof Constructor !== 'function') throw dataCloneError('Unknown TypedArray ' + node.c);
          value = new Constructor(decode(node.b), node.o, node.l);
          break;
        }
        default: throw dataCloneError('Unknown native structured-clone type');
      }
      values[id] = value;
      built[id] = true;
      if (node.t === 'a') {
        value.length = node.i.length;
        node.i.forEach((item, index) => { if (!(Array.isArray(item) && item[0] === 'h')) value[index] = de)JSRH"
        R"JSRH(code(item); });
      } else if (node.t === 'm') {
        for (const pair of node.e) value.set(decode(pair[0]), decode(pair[1]));
      } else if (node.t === 's') {
        for (const entry of node.e) value.add(decode(entry));
      }
      properties(value, node.p);
      return value;
    }

    return decode(graph.root);
  }

  function dispatchMessage(target, value) {
    target.dispatchEvent(new MessageEvent('message', { data: value }));
  }

  function dispatchError(target, message) {
    const error = new Error(String(message));
    target.dispatchEvent(new ErrorEvent('error', { message: String(message), error }));
  }

  function installWorker(Constructor) {
    const nativePostMessage = Constructor.prototype.__jsrhNativePostMessage;
    Object.defineProperty(Constructor.prototype, 'postMessage', {
      configurable: true,
      writable: true,
      value(message, transfer) {
        return nativePostMessage.call(this, {
          __jsrhMessage: message,
          __jsrhTransfer: transfer
        });
      }
    });
    Object.setPrototypeOf(Constructor.prototype, EventTarget.prototype);
    installHandler(Constructor.prototype, 'message');
    installHandler(Constructor.prototype, 'messageerror');
    installHandler(Constructor.prototype, 'error');
    Object.defineProperty(Constructor.prototype, Symbol.toStringTag,
      { value: 'Worker', configurable: true });
  }

  Object.defineProperties(g, {
    EventTarget: { value: EventTarget, writable: true, configurable: true },
    Event: { value: Event, writable: true, configurable: true },
    MessageEvent: { value: MessageEvent, writable: true, configurable: true },
    ErrorEvent: { value: ErrorEvent, writable: true, configurable: true },
    DOMException: { value: DOMException, writable: true, configurable: true },
    __jsrhInstallHandler: { value: installHandler },
    __jsrhInstallWorker: { value: installWorker },
    __jsrhSerialize: { value: serialize },
    __jsrhDeserialize: { value: deserialize },
    __jsrhDispatchMessage: { value: dispatchMessage },
    __jsrhDispatchError: { value: dispatchError },
    __jsrhWorkerCommonInstalled: { value: true }
  });
})();
)JSRH";

    inline constexpr char WorkerGlobal[] = R"JSRH(
(() => {
  'use strict';
  const g = globalThis;

  // JavaScriptCore's Node-API class adapter exposes native constructors as
  // constructable exotic objects, so JavaScript's typeof reports "object".
  // Browser feature detection commonly requires "function". Wrap those
  // constructors in this realm while retaining native instances, prototypes,
  // and inherited static methods.
  function normalizeNativeConstructor(name) {
    const nativeConstructor = g[name];
    if (!nativeConstructor || typeof nativeConstructor === 'function') return;
    function BrowserConstructor(...args) {
      if (!new.target) {
        throw new TypeError(name + ' constructor must be called with new');
      }
      const instance = new nativeConstructor(...args);
      if (new.target !== BrowserConstructor) {
        Object.setPrototypeOf(instance, new.target.prototype);
      }
      return instance;
    }
    BrowserConstructor.prototype = nativeConstructor.prototype;
    Object.setPrototypeOf(BrowserConstructor, nativeConstructor);
    try {
      Object.defineProperty(BrowserConstructor, 'name',
        { value: name, configurable: true });
    } catch (_) {}
    g[name] = BrowserConstructor;
  }

  for (const constructorName of [
    'AbortController', 'AbortSignal', 'Blob', 'File', 'FileReader',
    'TextDecoder', 'TextEncoder', 'URL', 'URLSearchParams', 'WebSocket',
    'XMLHttpRequest'
  ]) {
    normalizeNativeConstructor(constructorName);
  }

  class WorkerGlobalScope extends EventTarget {}
  class DedicatedWorkerGlobalScope extends WorkerGlobalScope {}
  __jsrhInstallHandler(WorkerGlobalScope.prototype, 'message');
  __jsrhInstallHandler(WorkerGlobalScope.prototype, 'messageerror');
  __jsrhInstallHandler(WorkerGlobalScope.prototype, 'error');
  Object.defineProperty(WorkerGlobalScope.prototype, Symbol.toStringTag,
    { value: 'WorkerGlobalScope', configurable: true });
  Object.defineProperty(DedicatedWorkerGlobalScope.prototype, Symbol.toStringTag,
    { value: 'DedicatedWorkerGlobalScope', configurable: true });

  try { delete g.window; } catch (_) { g.window = undefined; }
  try {
    Object.setPrototypeOf(g, DedicatedWorkerGlobalScope.prototype);
  } catch (_) {
    // JavaScriptCore's global object has an immutable prototype. Preserve its
    // engine-owned prototype and provide the observable worker-global shape
    // through own accessors plus @@hasInstance instead.
    Object.defineProperty(WorkerGlobalScope, Symbol.hasInstance, {
      configurable: true,
      value(instance) {
        return instance === g || Function.prototype[Symbol.hasInstance].call(this, instance);
      }
    });
  }
  __jsrhInstallHandler(g, 'message');
  __jsrhInstallHandler(g, 'messageerror');
  __jsrhInstallHandler(g, 'error');

  function createWorkerLocation(href) {
    const location = {
      href,
      origin: 'null',
      protocol: '',
      host: '',
      hostname: '',
      port: '',
      pathname: '',
      search: '',
      hash: '',
      toString() { return this.href; }
    };
    try {
      const parsed = new URL(href, 'app:///');
      for (const key of ['href', 'origin', 'protocol', 'host', 'hostname',
                         'port', 'pathname', 'search', 'hash']) {
        if (parsed[key] !== undefined) location[key] = String(parsed[key]);
      }
    } catch (_) {
      location.pathname = href;
    }
    return Object.freeze(location);
  }

  Object.defineProperties(g, {
    WorkerGlobalScope: { value: WorkerGlobalScope, writable: true, configurable: true },
    DedicatedWorkerGlobalScope: { value: DedicatedWorkerGlobalScope, writable: true, configurable: true },
    self: { value: g, writable: false, enumerable: true, configurable: false },
    location: { value: createWorkerLocation(String(g.__jsrhWorkerLocation || '')),
      writable: false, enumerable: true, configurable: false },
    name: { value: String(g.__jsrhWorkerName || ''), writable: false, configurable: true },
    navigator: { value: Object.freeze({
        hardwareConcurrency: 1,
        language: 'en-US',
        languages: Object.freeze(['en-US']),
        onLine: true,
        platform: '',
        userAgent: 'JsRuntimeHost Worker'
      }),
      writable: false, configurable: true },
    addEventListener: { value: EventTarget.prototype.addEventListener.bind(g),
      writable: true, configurable: true },
    removeEventListener: { value: EventTarget.prototype.removeEventListener.bind(g),
      writable: true, configurable: true },
    dispatchEvent: { value: EventTarget.prototype.dispatchEvent.bind(g),
      writable: true, configurable: true },
    postMessage: { value(message, transfer) { g.__jsrhNativePostMessage({
        __jsrhMessage: message, __jsrhTransfer: transfer
      }); },
      writable: true, configurable: true },
    close: { value() { g.__jsrhNativeClose(); }, writable: true, configurable: true },
    importScripts: { value(...urls) { return g.__jsrhNativeImportScripts(...urls); },
      writable: true, configurable: true }
  });

  // Browser fetch resolves relative inputs against the worker's own location.
  // The shared native fetch polyfill intentionally has no realm/base-URL
  // knowledge, so add that worker-specific behavior here.
  if (typeof g.fetch === 'function') {
    const nativeFetch = g.fetch;
    g.fetch = function(input, init) {
      let candidate = input;
      if (typeof input === 'string' ||
          (typeof URL === 'function' && input instanceof URL)) {
        candidate = String(input);
      } else if (input && typeof input === 'object' &&
                 typeof input.url === 'string') {
        candidate = input.url;
      }
      if (typeof candidate === 'string') {
        try { candidate = new URL(candidate, g.location.href).href; }
        catch (_) {}
      }
      return nativeFetch.call(g, candidate, init);
    };
  }

})();
)JSRH";
}
