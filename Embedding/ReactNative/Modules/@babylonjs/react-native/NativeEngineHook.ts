import { useEffect, useState } from 'react';
import { AppState, AppStateStatus } from 'react-native';
import { ensureInitialized } from './BabylonModule';
import { ReactNativeEngine } from './ReactNativeEngine';

import './VersionValidation';

export function useModuleInitializer(): boolean | undefined {
    const [initialized, setInitialized] = useState<boolean>();

    useEffect(() => {
        const abortController = new AbortController();
        (async () => {
            const isInitialized = await ensureInitialized();

            if (!abortController.signal.aborted) {
                setInitialized(isInitialized);
            }
        })();

        return () => {
            abortController.abort();
        }
    }, []);

    return initialized;
}

function useAppState(): string {
    const [appState, setAppState] = useState(AppState.currentState);

    useEffect(() => {
        const onAppStateChanged = (appState: AppStateStatus) => {
            setAppState(appState);
        };

        AppState.addEventListener("change", onAppStateChanged);

        return () => {
            AppState.removeEventListener("change", onAppStateChanged);
        }
    }, []);

    return appState;
}

export function useRenderLoop(engine: ReactNativeEngine | undefined, renderCallback: () => void): void {
    const appState = useAppState();

    useEffect(() => {
        if (engine && appState === "active") {
            if (!engine.isDisposed) {
                engine.runRenderLoop(renderCallback);

                return () => {
                    if (!engine.isDisposed) {
                        engine.stopRenderLoop();
                    }
                };
            }
        }

        return undefined;
    }, [appState, engine]);
}
