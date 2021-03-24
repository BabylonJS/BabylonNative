import { Camera } from '@babylonjs/core';

export type CapturedFrame = {
  width: number;
  height: number;
  format: "RGBA8" | "BGRA8" | undefined;
  yFlip: boolean;
  data: ArrayBuffer;
};

export type CaptureCallback = (capture: CapturedFrame) => void;

declare class NativeCapture {
  public constructor(frameBuffer?: unknown | undefined);
  public addCallback(onCaptureCallback: CaptureCallback): void;
  public dispose(): void;
};

export class CaptureSession {
    private readonly nativeCapture: NativeCapture;

    public constructor(camera: Camera | undefined, onCaptureCallback: CaptureCallback) {
        console.warn(`CaptureSession is experimental and likely to change significantly.`);
        this.nativeCapture = new NativeCapture(camera?.outputRenderTarget?.getInternalTexture()?._framebuffer);
        this.nativeCapture.addCallback(onCaptureCallback);
    }

    public dispose(): void {
        this.nativeCapture.dispose();
    }
}