import React, { Component, FunctionComponent, SyntheticEvent, useCallback, useEffect, useState, useRef, useMemo } from 'react';
import { ViewProps, View, Text, findNodeHandle, UIManager } from 'react-native';
import { Camera, SceneInstrumentation } from '@babylonjs/core';
import { ReactNativeEngine } from './ReactNativeEngine';
import { useModuleInitializer, useRenderLoop } from './NativeEngineHook';
import { NativeEngineViewProps, NativeEngineView } from './NativeEngineView';

export interface EngineViewProps extends ViewProps {
    camera?: Camera;
    displayFrameRate?: boolean;
    isTransparent?: boolean;
    isTopMost?: boolean;
    antiAliasing?: 0 | 1 | 2 | 4 | 8 | 16;
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
    //const [fps, setFps] = useState<number>();
    const [sceneStats, setSceneStats] = useState<SceneStats>();
    const engineViewRef = useRef<Component<NativeEngineViewProps>>(null);
    const snapshotPromise = useRef<{ promise: Promise<string>, resolve: (data: string) => void }>();
    const isTransparent = props.isTransparent ?? false;
    const antiAliasing = props.antiAliasing ?? 0;
    const isTopMost = props.isTopMost ?? false;

    const initialized = useModuleInitializer();

    const engine = useMemo(() => {
        return props.camera?.getScene().getEngine() as ReactNativeEngine;
    }, [props.camera]);

    const renderLoop = useCallback(() => {
        for (let scene of engine.scenes) {
            scene.render();
        }
    }, [engine]);

    useRenderLoop(engine, renderLoop);

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
                { initialized && <NativeEngineView ref={engineViewRef} style={{ flex: 1 }} onSnapshotDataReturned={snapshotDataReturnedHandler} isTransparent={isTransparent} antiAliasing={antiAliasing} isTopMost={isTopMost}/> }
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
            <View style={{ flex: 1, justifyContent: 'center', alignItems: 'center', backgroundColor: 'white' }}>
                <Text style={{ fontSize: 24, color: 'black' }}>{message}</Text>
                <Text style={{ fontSize: 12, color: 'black' }}>React Native remote debugging does not work with Babylon Native.</Text>
            </View>
        );
    }
}
