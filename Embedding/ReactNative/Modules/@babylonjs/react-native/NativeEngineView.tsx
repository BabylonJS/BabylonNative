import { Component, SyntheticEvent } from 'react';
import { requireNativeComponent, ViewProps } from 'react-native';

declare const global: any;

export interface NativeEngineViewProps extends ViewProps {
    isTransparent: boolean;
    antiAliasing: number;
    isTopMost: boolean;
    onSnapshotDataReturned?: (event: SyntheticEvent) => void;
}

export const NativeEngineView: {
    prototype: Component<NativeEngineViewProps>;
    new(props: Readonly<NativeEngineViewProps>): Component<NativeEngineViewProps>;
} = global['EngineView'] || (global['EngineView'] = requireNativeComponent('EngineView'));
