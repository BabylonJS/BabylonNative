import { Tools } from '@babylonjs/core';

// Declare _native to get access to NativeCanvas.
declare var _native: any;

/**
 * Partial Polyfill for FontFace Web API to wrap the NativeCanvas object.
 */
export class FontFace {
  private source: string | ArrayBuffer | undefined;
  private _status: "unloaded" | "loading" | "loaded" | "error" = "unloaded";
  public get status(): "unloaded" | "loading" | "loaded" | "error" {
    return this._status;
  }

  public get loaded(): boolean {
    return this._status === "loaded";
  }

  public constructor(public readonly family: string, source: string | ArrayBuffer) {
    this.source = source;
  }

  public async load(): Promise<void> {
    try {
      this._status = "loading";
      if (typeof this.source === 'string') {
        this.source = await Tools.LoadFileAsync(this.source);
      }

      await _native.Canvas.loadTTFAsync(this.family, this.source);
      this.source = undefined;
      this._status = "loaded"
    } catch (ex) {
      console.error("Error encountered when loading font: " + ex);
      this._status = "error";
    }
  }
}