import React, { Component, FunctionComponent, SyntheticEvent, useCallback, useEffect, useState, useRef } from 'react';
import { requireNativeComponent, ViewProps, AppState, AppStateStatus, View, Text, findNodeHandle, UIManager } from 'react-native';
import { Camera } from '@babylonjs/core';
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

export const EngineView: FunctionComponent<EngineViewProps> = (props: EngineViewProps) => {
    const [initialized, setInitialized] = useState<boolean>();
    const [appState, setAppState] = useState(AppState.currentState);
    const [fps, setFps] = useState<number>();
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
            const engine = props.camera.getScene().getEngine() as ReactNativeEngine;

            if (!engine.isDisposed) {
                setFps(engine.getFps());
                const timerHandle = setInterval(() => {
                    setFps(engine.getFps());
                }, 1000);

                return () => {
                    clearInterval(timerHandle);
                };
            }
        }

        setFps(undefined);
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
            <View style={[props.style, { overflow: "hidden" }]}>
                { initialized && <NativeEngineView ref={engineViewRef} style={{ flex: 1 }} onSnapshotDataReturned={snapshotDataReturnedHandler} /> }
                { fps && <Text style={{ color: 'yellow', position: 'absolute', margin: 10, right: 0, top: 0 }}>FPS: {Math.round(fps)}</Text> }
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
