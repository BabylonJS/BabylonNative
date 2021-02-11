export type CapturedFrame = {
  width: number;
  height: number;
  pitch: number;
  format: "BGRA8" | undefined;
  yFlip: boolean;
  data: ArrayBuffer;
};

export type CaptureCallback = (capture: CapturedFrame) => void;

declare class NativeCapture {
  public constructor();
  public addCallback(onCaptureCallback: CaptureCallback): void;
  public dispose(): void;
};

export class CaptureSession {
    private readonly nativeCapture: NativeCapture;

    public constructor(onCaptureCallback: CaptureCallback) {
        console.warn(`CaptureSession is experimental and likely to change significantly.`);
        this.nativeCapture = new NativeCapture();
        this.nativeCapture.addCallback(onCaptureCallback);
    }

    public dispose(): void {
        this.nativeCapture.dispose();
    }
}