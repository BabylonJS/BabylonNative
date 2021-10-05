import React, { Component, FunctionComponent, SyntheticEvent, useCallback, useEffect, useState, useRef } from 'react';
import { requireNativeComponent, ViewProps, AppState, AppStateStatus, View, Text, findNodeHandle, UIManager } from 'react-native';
import { Camera, SceneInstrumentation } from '@babylonjs/core';
import { ensureInitialized } from './BabylonModule';
import { ReactNativeEngine } from './ReactNativeEngine';

declare const global: any;

interface NativeEngineViewProps extends ViewProps {
    onSnapshotDataReturned: (event: SyntheticEvent) => void;
}

const NativeEngineView: {
    prototype: Component<NativeEngineViewProps>;
    new(props: Readonly<NativeEngineViewProps>): Component<NativeEngineViewProps>;
} = global['EngineView'] || (global['EngineView'] = requireNativeComponent('EngineView'));

export interface EngineViewProps extends ViewProps {
    camera?: Camera;
    displayFrameRate?: boolean;
    onInitialized?: (view: EngineViewCallbacks) => void;
}

export interface EngineViewCallbacks {
    takeSnapshot: () => Promise<string>;
}

interface SceneStats {
    frameRate: number,
    frameTime: number,
}

export const EngineView: FunctionComponent<EngineViewProps> = (props: EngineViewProps) => {
    const [initialized, setInitialized] = useState<boolean>();
    const [appState, setAppState] = useState(AppState.currentState);
    //const [fps, setFps] = useState<number>();
    const [sceneStats, setSceneStats] = useState<SceneStats>();
    const engineViewRef = useRef<Component<NativeEngineViewProps>>(null);
    const snapshotPromise = useRef<{ promise: Promise<string>, resolve: (data: string) => void }>();

    useEffect(() => {
        (async () => {
            setInitialized(await ensureInitialized());
        })();
    }, []);

    useEffect(() => {
        const onAppStateChanged = (appState: AppStateStatus) => {
            setAppState(appState);
        };

        AppState.addEventListener("change", onAppStateChanged);

        return () => {
            AppState.removeEventListener("change", onAppStateChanged);
        }
    }, []);

    useEffect(() => {
        if (props.camera && appState === "active") {
            const engine = props.camera.getScene().getEngine() as ReactNativeEngine;

            if (!engine.isDisposed) {
                engine.runRenderLoop(() => {
                    for (let scene of engine.scenes) {
                        scene.render();
                    }
                });

                return () => {
                    if (!engine.isDisposed) {
                        engine.stopRenderLoop();
                    }
                };
            }
        }

        return undefined;
    }, [props.camera, appState]);

    useEffect(() => {
        if (props.camera && (props.displayFrameRate ?? __DEV__)) {
            const scene = props.camera.getScene();
            const engine = scene.getEngine() as ReactNativeEngine;

            if (!engine.isDisposed) {
                setSceneStats({frameRate: 0, frameTime: 0});

                const sceneInstrumentation = new SceneInstrumentation(scene);
                sceneInstrumentation.captureFrameTime = true;

                const timerHandle = setInterval(() => {
                    setSceneStats({frameRate: engine.getFps(), frameTime: sceneInstrumentation.frameTimeCounter.lastSecAverage});
                }, 1000);

                return () => {
                    clearInterval(timerHandle);
                    setSceneStats(undefined);
                    sceneInstrumentation.dispose();
                };
            }
        }

        return undefined;
    }, [props.camera, props.displayFrameRate]);

    // Call onInitialized if provided, and include the callback for takeSnapshot.
    useEffect(() => {
        if (props.onInitialized) {
            props.onInitialized({
                takeSnapshot: (): Promise<string> => {
                    if (!snapshotPromise.current) {
                        let resolveFunction: ((data: string) => void) | undefined;
                        const promise = new Promise<string>((resolutionFunc) => {
                            resolveFunction = resolutionFunc;
                        });

                        // Resolution functions should always be initialized.
                        if (resolveFunction) {
                            snapshotPromise.current = { promise: promise, resolve: resolveFunction };
                        }
                        else {
                            throw new Error("Resolution functions not initialized after snapshot promise creation.");
                        }

                        UIManager.dispatchViewManagerCommand(
                            findNodeHandle(engineViewRef.current),
                            "takeSnapshot",
                            []);
                    }

                    return snapshotPromise.current.promise;
                }
            });
        }
    }, [props.onInitialized]);

    // Handle snapshot data returned.
    const snapshotDataReturnedHandler = useCallback((event: SyntheticEvent) => {
        // The nativeEvent is a DOMEvent which doesn't have a typescript definition. Cast it to an Event object with a data property.
        const { data } = event.nativeEvent as Event & { data: string };
        if (snapshotPromise.current) {
            snapshotPromise.current.resolve(data);
            snapshotPromise.current = undefined;
        }
    }, []);

    if (initialized !== false) {
        return (
            <View style={[{ flex: 1 }, props.style, { overflow: "hidden" }]}>
                { initialized && <NativeEngineView ref={engineViewRef} style={{ flex: 1 }} onSnapshotDataReturned={snapshotDataReturnedHandler} /> }
                { sceneStats !== undefined &&
                <View style={{ backgroundColor: '#00000040', opacity: 1, position: 'absolute', right: 0, left: 0, top: 0, flexDirection: 'row-reverse' }}>
                    <Text style={{ color: 'yellow', alignSelf: 'flex-end', margin: 3, fontVariant: ['tabular-nums'] }}>FPS: {sceneStats.frameRate.toFixed(0)}</Text>
                    {/* Frame time seems wonky... it goes down when manipulating the scaling slider in the Playground app. Investigate this before showing it so we don't show data that can't be trusted. */}
                    {/* <Text style={{ color: 'yellow', alignSelf: 'flex-end', margin: 3, fontVariant: ['tabular-nums'] }}> | </Text>
                    <Text style={{ color: 'yellow', alignSelf: 'flex-end', margin: 3, fontVariant: ['tabular-nums'] }}>Frame Time: {sceneStats.frameTime.toFixed(1)}ms</Text> */}
                </View>
                }
            </View>
        );
    } else {
        const message = "Could not initialize Babylon Native.";
        if (!__DEV__) {
            throw new Error(message);
        }

        return (
            <View style={{ flex: 1, justifyContent: 'center', alignItems: 'center' }}>
                <Text style={{ fontSize: 24 }}>{message}</Text>
                <Text style={{ fontSize: 12 }}>React Native remote debugging does not work with Babylon Native.</Text>
            </View>
        );
    }
}
