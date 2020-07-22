import React, { FunctionComponent, Component, useEffect, useState } from 'react';
import { requireNativeComponent, NativeModules, ViewProps, AppState, AppStateStatus, View, Text } from 'react-native';
import { Camera } from '@babylonjs/core';
import { IsEngineDisposed } from './EngineHelpers';
import { BabylonModule } from './BabylonModule';

declare const global: any;
const isRemoteDebuggingEnabled = !global['nativeCallSyncHook'];

const EngineViewManager: {
    setJSThread(): void;
} = NativeModules.EngineViewManager;

// Not all platforms need this, but for those that do, this is intended to be a synchronous call to boostrap the ability to run native code on the JavaScript thread.
if (EngineViewManager && EngineViewManager.setJSThread && !isRemoteDebuggingEnabled) {
    EngineViewManager.setJSThread();
}

interface NativeEngineViewProps extends ViewProps {
}

const NativeEngineView: {
    prototype: Component<NativeEngineViewProps>;
    new(props: Readonly<NativeEngineViewProps>): Component<NativeEngineViewProps>;
} = requireNativeComponent('EngineView');

export interface EngineViewProps extends ViewProps {
    camera?: Camera;
    displayFrameRate?: boolean;
}

export const EngineView: FunctionComponent<EngineViewProps> = (props: EngineViewProps) => {
    const [failedInitialization, setFailedInitialization] = useState(false);
    const [fps, setFps] = useState<number>();

    useEffect(() => {
        (async () => {
            if (!await BabylonModule.whenInitialized()) {
                setFailedInitialization(true);
            }
        })();
    }, []);

    useEffect(() => {
        if (props.camera) {
            const engine = props.camera.getScene().getEngine();

            if (!IsEngineDisposed(engine)) {
                const onAppStateChanged = (appState: AppStateStatus) => {
                    if (appState === "active") {
                        engine.runRenderLoop(() => {
                            for (let scene of engine.scenes) {
                                scene.render();
                            }
                        });
                    } else {
                        engine.stopRenderLoop();
                    }
                };

                onAppStateChanged(AppState.currentState);
                AppState.addEventListener("change", onAppStateChanged);

                return () => {
                    if (!IsEngineDisposed(engine)) {
                        engine.stopRenderLoop();
                    }
                    AppState.removeEventListener("change", onAppStateChanged);
                };
            }
        }
    }, [props.camera]);

    useEffect(() => {
        if (props.camera && (props.displayFrameRate ?? __DEV__)) {
            const engine = props.camera.getScene().getEngine();

            if (!IsEngineDisposed(engine)) {
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
    }, [props.camera, props.displayFrameRate]);

    if (!failedInitialization) {
        return (
            <View style={props.style}>
                <NativeEngineView style={{flex: 1}} />
                { fps && <Text style={{color: 'yellow', position: 'absolute', margin: 10, right: 0, top: 0}}>FPS: {Math.round(fps)}</Text> }
            </View>
        );
    } else {
        return (
            <View style={{flex: 1, justifyContent: 'center', alignItems: 'center'}}>
                <Text style={{fontSize: 24}}>Could not initialize Babylon Native.</Text>
                { isRemoteDebuggingEnabled && <Text style={{fontSize: 12}}>React Native remote debugging does not work with Babylon Native.</Text> }
            </View>
        );
    }
}