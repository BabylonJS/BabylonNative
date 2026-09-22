const assert = require("node:assert/strict");
const { readFileSync } = require("node:fs");
const { join } = require("node:path");
const { test } = require("node:test");
const { runInNewContext } = require("node:vm");

const source = readFileSync(join(__dirname, "..", "Scripts", "validation_native.js"), "utf8");

function extractFunctionDeclaration(name) {
    const match = new RegExp(`\\bfunction\\s+${name}\\s*\\(`).exec(source);
    assert.ok(match, `Could not find function ${name}`);
    const bodyStart = source.indexOf("{", match.index + match[0].length);
    assert.ok(bodyStart >= 0, `Could not find body for function ${name}`);

    let depth = 0;
    let quote = null;
    let escaped = false;
    let lineComment = false;
    let blockComment = false;
    for (let index = bodyStart; index < source.length; ++index) {
        const character = source[index];
        const next = source[index + 1];
        if (lineComment) {
            lineComment = character !== "\n";
            continue;
        }
        if (blockComment) {
            if (character === "*" && next === "/") {
                blockComment = false;
                ++index;
            }
            continue;
        }
        if (quote) {
            if (escaped) {
                escaped = false;
            } else if (character === "\\") {
                escaped = true;
            } else if (character === quote) {
                quote = null;
            }
            continue;
        }
        if (character === "/" && next === "/") {
            lineComment = true;
            ++index;
        } else if (character === "/" && next === "*") {
            blockComment = true;
            ++index;
        } else if (character === "\"" || character === "'" || character === "`") {
            quote = character;
        } else if (character === "{") {
            ++depth;
        } else if (character === "}" && --depth === 0) {
            return source.slice(match.index, index + 1);
        }
    }
    assert.fail(`Could not find end of function ${name}`);
}

test("restores forward depth between scenes on the reused engine", () => {
    let disposed = 0;
    let released = 0;
    const leakedTexture = {};
    const leakedTextures = [leakedTexture];
    const engine = {
        scenes: [],
        useReverseDepthBuffer: true,
        setHardwareScalingLevel() {},
        setStencilBuffer() {},
        disableScissor() {},
        releaseEffects() { assert.equal(this.useReverseDepthBuffer, false); },
        getLoadedTexturesCache() { return leakedTextures; },
        _releaseTexture(texture) {
            assert.equal(texture, leakedTexture);
            ++released;
            leakedTextures.pop();
        },
        clearInternalTexturesCache() { leakedTextures.length = 0; },
    };
    const context = {
        engine,
        currentScene: { dispose() { disposed++; } },
        console,
        BABYLON: { SceneLoader: { OnPluginActivatedObservable: { clear() {} } } },
    };
    runInNewContext(extractFunctionDeclaration("cleanupAfterTest"), context);
    context.cleanupAfterTest();
    context.cleanupAfterTest();
    assert.equal(disposed, 1);
    assert.equal(released, 1);
    assert.equal(context.currentScene, null);
    assert.equal(engine.useReverseDepthBuffer, false);
});
