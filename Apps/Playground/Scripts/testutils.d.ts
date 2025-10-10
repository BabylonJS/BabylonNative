declare const TestUtils: {
    getImageData(image: any): Uint8Array;
    getOutputDirectory(): string;
    writePNG(data: Uint8Array, width: number, height: number, fileName: string): void;
    getGraphicsApiName(): string;
    setTitle(title: string): void;
    decodeImage(data: ArrayBuffer): any;
    exit(code: number): void;
    updateSize(width: number, height: number): void;
    getFrameBufferData(callback: (screenshot: Uint8Array) => void): void;
};