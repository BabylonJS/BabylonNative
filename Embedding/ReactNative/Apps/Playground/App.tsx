/**
 * Generated with the TypeScript template
 * https://github.com/react-native-community/react-native-template-typescript
 *
 * @format
 */

import React, { useState, FunctionComponent, useEffect } from 'react';
import { SafeAreaView, StatusBar, Button, View, Text, ViewProps } from 'react-native';

import { EngineView, useEngine } from 'react-native-babylon';
import { Scene, Vector3, Mesh, ArcRotateCamera, Engine, Camera, PBRMetallicRoughnessMaterial, Color3 } from '@babylonjs/core';
import Slider from '@react-native-community/slider';

const EngineScreen: FunctionComponent<ViewProps> = (props: ViewProps) => {
  const defaultScale = 1;

  const [toggleView, setToggleView] = useState(false);
  const [camera, setCamera] = useState<Camera>();
  const [box, setBox] = useState<Mesh>();
  const [scale, setScale] = useState<number>(defaultScale);
  const [fps, setFps] = useState<number>(0);

  useEngine((engine: Engine) => {
    const scene = new Scene(engine);
    scene.createDefaultCamera(true);
    if (scene.activeCamera != null) {
      (scene.activeCamera as ArcRotateCamera).beta -= Math.PI / 8;
      setCamera(scene.activeCamera);
    }
    scene.createDefaultLight(true);

    const box = Mesh.CreateBox("box", 0.3, scene);
    setBox(box);
    const mat = new PBRMetallicRoughnessMaterial("mat", scene);
    mat.metallic = 1;
    mat.roughness = 0.5;
    mat.baseColor = Color3.Red();
    box.material = mat;

    scene.beforeRender = function () {
      scene.meshes[0].rotate(Vector3.Up(), 0.005 * scene.getAnimationRatio());
    };

    const timerHandle = setInterval(() => {
      setFps(engine.getFps());
    }, 1000);

    return () => {
      clearInterval(timerHandle);
    };
  });

  useEffect(() => {
    if (box) {
      box.scaling = new Vector3(scale, scale, scale);
    }
  }, [box, scale]);

  return (
    <>
      <View style={props.style}>
        <Button title="Toggle EngineView" onPress={() => { setToggleView(!toggleView) }} />
        { !toggleView &&
          <View style={{flex: 1}}>
            <EngineView style={props.style} camera={camera} />
            <Slider style={{position: 'absolute', minHeight: 50, margin: 10, left: 0, right: 0, bottom: 0}} minimumValue={0.2} maximumValue={2} value={defaultScale} onValueChange={setScale} />
            <Text style={{color: 'yellow', position: 'absolute', margin: 10, right: 0, top: 0}}>FPS: {Math.round(fps)}</Text>
          </View>
        }
        { toggleView &&
          <View style={{flex: 1, justifyContent: 'center', alignItems: 'center'}}>
            <Text style={{fontSize: 24}}>EngineView has been removed.</Text>
            <Text style={{fontSize: 12}}>Render loop stopped, but engine is still alive.</Text>
          </View>
        }
      </View>
    </>
  );
};

const App = () => {
  const [toggleScreen, setToggleScreen] = useState(false);

  return (
    <>
      <StatusBar barStyle="dark-content" />
      <SafeAreaView style={{flex: 1}}>
        { !toggleScreen &&
          <EngineScreen style={{flex: 1}} />
        }
        { toggleScreen &&
          <View style={{flex: 1, justifyContent: 'center', alignItems: 'center'}}>
            <Text style={{fontSize: 24}}>EngineScreen has been removed.</Text>
            <Text style={{fontSize: 12}}>Engine has been disposed, and will be recreated.</Text>
          </View>
        }
        <Button title="Toggle EngineScreen" onPress={() => { setToggleScreen(!toggleScreen) }} />
      </SafeAreaView>
    </>
  );
};

export default App;
