// es2019_transpile.js
//
// Lightweight regex-based ES2020+ -> ES2019 syntax repair for engines that
// lack optional chaining (?.), nullish coalescing (??), and numeric
// separators (1_000_000).
//
// This is a SYNTAX REPAIR, not a full transpile. It rewrites parse-time
// failing tokens to the closest legal ES2019 expressions so that the host
// engine accepts the code. Where native ES2020+ semantics differ from the
// rewritten form, the rewritten form runs the "happy path" -- assumes the
// target value is present. If the target is null at runtime the rewritten
// code throws a TypeError where native ?. would have short-circuited to
// undefined. This is an intentional trade-off: it produces valid syntax
// (the prerequisite for the test running at all) while preserving the
// common case behaviour. Tests that rely on the nullish-short-circuit
// semantics may surface runtime errors that were previously hidden by
// the parse failure -- a strict improvement for debuggability.
//
// Transforms applied:
//   * 1_000_000   -> 1000000     (strip numeric separators)
//   * a?.b        -> a.b         (optional chaining -> required chaining)
//   * a?.[x]      -> a[x]
//   * a?.()       -> a()
//   * a ?? b      -> (a != null ? a : b)
//
// Not handled (still cause parse failures on Chakra):
//   * Logical assignment ||= &&= ??=
//   * Class private fields #name
//   * BigInt literals 1n
//
// String/regex/comment literals containing ? . sequences are not skipped,
// but in Babylon snippet code such occurrences are rare.
//
// On engines with native ES2020+ support (V8, JSC) this code is never
// invoked because the caller only retries with this transform after
// initial eval throws a SyntaxError.
//
// Public API: top-level global function __bnTranspileES2019(code) -> string.
// Chakra in Babylon Native does not define `self` or `globalThis`, so we
// install via a top-level `var` declaration which becomes a property of
// the script-global object across all our supported engines.
var __bnTranspileES2019 = (function () {
    "use strict";

    function stripNumericSeparators(code) {
        // Decimal / float: 1_000_000, 1.234_5, 1_000e3
        var out = code.replace(/\b(\d[\d_]*(?:\.[\d_]+)?(?:[eE][+\-]?[\d_]+)?)\b/g, function (m) {
            return m.indexOf("_") === -1 ? m : m.replace(/_/g, "");
        });
        // Hex / octal / binary: 0xFFFF_FFFF, 0o7_5, 0b1010_0101
        out = out.replace(/\b(0[xXoObB][0-9A-Fa-f_]+)\b/g, function (m) {
            return m.indexOf("_") === -1 ? m : m.replace(/_/g, "");
        });
        return out;
    }

    function transformLogicalAssignment(code) {
        // a ||= b  -> (a || (a = b))
        // a &&= b  -> (a && (a = b))
        // a ??= b  -> (a != null ? a : (a = b))
        // Only matches simple LHS (identifier, optional .prop / [idx] chain) to keep
        // the rewrite syntactically safe; complex LHS expressions are left alone.
        var lhs = "(?:[A-Za-z_$][\\w$]*)(?:\\.[A-Za-z_$][\\w$]*|\\[[^\\]\\n]*\\])*";
        var rhs = "[^;\\n]+";
        var prev = null, out = code, guard = 0;
        while (prev !== out && guard++ < 10) {
            prev = out;
            out = out
                .replace(new RegExp("(" + lhs + ")\\s*\\?\\?=\\s*(" + rhs + ")", "g"),
                         "$1 = ($1 != null ? $1 : ($2))")
                .replace(new RegExp("(" + lhs + ")\\s*\\|\\|=\\s*(" + rhs + ")", "g"),
                         "$1 = ($1 || ($2))")
                .replace(new RegExp("(" + lhs + ")\\s*&&=\\s*(" + rhs + ")", "g"),
                         "$1 = ($1 && ($2))");
        }
        return out;
    }

    function transformOptionalChaining(code) {
        var prev = null, out = code, guard = 0;
        while (prev !== out && guard++ < 50) {
            prev = out;
            out = out
                .replace(/\?\.(?=\()/g, "")
                .replace(/\?\.(?=\[)/g, "")
                .replace(/\?\./g, ".");
        }
        return out;
    }

    function transformNullishCoalescing(code) {
        var prev = null, out = code, guard = 0;
        while (prev !== out && guard++ < 50) {
            prev = out;
            out = out.replace(
                /(\b[A-Za-z_$][\w$.]*(?:\[[^\]]*\])?)\s*\?\?\s*(\b[A-Za-z_$][\w$.]*(?:\[[^\]]*\])?|"[^"]*"|'[^']*'|\d+(?:\.\d+)?|true|false|null|undefined)/g,
                "($1 != null ? $1 : $2)"
            );
        }
        return out;
    }

    function transpileES2019(code) {
        if (typeof code !== "string") { return code; }
        var out = code;
        out = stripNumericSeparators(out);
        out = transformLogicalAssignment(out);
        out = transformOptionalChaining(out);
        out = transformNullishCoalescing(out);
        return out;
    }

    return transpileES2019;
})();
