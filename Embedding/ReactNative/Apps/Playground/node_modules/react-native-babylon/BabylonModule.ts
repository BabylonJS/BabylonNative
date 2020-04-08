import { NativeModules } from 'react-native';

export const BabylonModule: {
    initialize(): Promise<boolean>;
    whenInitialized(): Promise<boolean>;
} = NativeModules.BabylonModule;