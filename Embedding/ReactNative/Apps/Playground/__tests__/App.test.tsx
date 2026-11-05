import React from "react";
import TestRenderer, { act } from "react-test-renderer";

const mockEnterXRAsync = jest.fn();
const mockExitXRAsync = jest.fn();
const mockCreateDefaultXRExperienceAsync = jest.fn();
const mockEngine = {};
const mockXrSession = {
  exitXRAsync: mockExitXRAsync,
  onXRSessionEnded: { add: jest.fn() },
};

jest.mock("@babylonjs/react-native", () => ({
  EngineView: "EngineView",
  useEngine: () => mockEngine,
}));

jest.mock("@react-native-community/slider", () => "Slider");

jest.mock("@babylonjs/core", () => {
  class Vector3 {
    public static Down = () => new Vector3();
    public static Up = () => new Vector3();

    public origin = this;
    public direction = this;
    public length = 1;

    public add = () => this;
    public scale = () => this;
  }

  class TransformNode {
    public parent: unknown;
    public position = new Vector3();
    public scaling = {
      scaleInPlace: jest.fn(),
    };
    public rotate = jest.fn();
  }

  class Scene {
    public activeCamera = {
      beta: 0,
      getForwardRay: () => new Vector3(),
    };
    public beforeRender: (() => void) | undefined;

    public createDefaultCamera = jest.fn();
    public createDefaultLight = jest.fn();
    public createDefaultXRExperienceAsync = mockCreateDefaultXRExperienceAsync;
    public getAnimationRatio = () => 1;
  }

  return {
    ArcRotateCamera: class {},
    Camera: class {},
    DeviceSourceManager: class {
      public onDeviceConnectedObservable = { add: jest.fn() };
    },
    DeviceType: { Touch: 0, Mouse: 1 },
    PointerInput: { Move: 0, LeftClick: 1 },
    Scene,
    SceneLoader: {
      ImportMeshAsync: jest.fn().mockResolvedValue({ meshes: [{}] }),
    },
    TransformNode,
    Vector3,
    WebXRSessionManager: class {},
    WebXRTrackingState: {},
  };
});

jest.mock("@babylonjs/loaders", () => ({}));

import App from "../App";

describe("Playground controls", () => {
  beforeEach(() => {
    jest.clearAllMocks();
    mockEnterXRAsync.mockResolvedValue(mockXrSession);
    mockExitXRAsync.mockResolvedValue(undefined);
    mockCreateDefaultXRExperienceAsync.mockResolvedValue({
      baseExperience: {
        enterXRAsync: mockEnterXRAsync,
        camera: {
          trackingState: undefined,
          onTrackingStateChanged: { add: jest.fn() },
        },
      },
      renderTarget: {},
    });
  });

  it("renders the controls above the native EngineView", async () => {
    let renderer: TestRenderer.ReactTestRenderer;
    await act(async () => {
      renderer = TestRenderer.create(<App />);
    });

    const controls = renderer!.root.findByProps({
      testID: "engine-controls",
    });

    expect(controls.props.style).toEqual(
      expect.objectContaining({
        position: "absolute",
        zIndex: expect.any(Number),
      })
    );
    expect(controls.props.style.zIndex).toBeGreaterThan(0);
  });

  it("shows an error when XR cannot start", async () => {
    mockCreateDefaultXRExperienceAsync.mockRejectedValue(
      new Error("XR is unavailable")
    );

    let renderer: TestRenderer.ReactTestRenderer;
    await act(async () => {
      renderer = TestRenderer.create(<App />);
    });

    const startXr = renderer!.root.findByProps({ title: "Start XR" });
    await act(async () => {
      await startXr.props.onPress();
    });

    expect(
      renderer!.root.findByProps({ testID: "xr-error" }).props.children
    ).toContain("XR is unavailable");
  });

  it("shows an error when XR cannot stop", async () => {
    let renderer: TestRenderer.ReactTestRenderer;
    await act(async () => {
      renderer = TestRenderer.create(<App />);
    });

    const startXr = renderer!.root.findByProps({ title: "Start XR" });
    await act(async () => {
      await startXr.props.onPress();
    });

    mockExitXRAsync.mockRejectedValue(new Error("XR shutdown failed"));
    const stopXr = renderer!.root.findByProps({ title: "Stop XR" });
    await act(async () => {
      await stopXr.props.onPress();
    });

    expect(
      renderer!.root.findByProps({ testID: "xr-error" }).props.children
    ).toContain("Unable to stop XR: XR shutdown failed");
  });
});
