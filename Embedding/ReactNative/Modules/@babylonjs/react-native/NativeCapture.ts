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
  public constructor(frameBuffer: unknown);
  public addCallback(onCaptureCallback: CaptureCallback): void;
  public dispose(): void;
};

export class CaptureSession {
    private readonly nativeCapture: NativeCapture;

    public constructor(camera: Camera | undefined, onCaptureCallback: CaptureCallback) {
        console.warn(`CaptureSession is experimental and likely to change significantly.`);
        // HACK: There is no exposed way to access the frame buffer from render target texture
        this.nativeCapture = new NativeCapture((camera?.outputRenderTarget?.renderTarget as any)?._framebuffer);
        this.nativeCapture.addCallback(onCaptureCallback);
    }

    public dispose(): void {
        this.nativeCapture.dispose();
    }
}