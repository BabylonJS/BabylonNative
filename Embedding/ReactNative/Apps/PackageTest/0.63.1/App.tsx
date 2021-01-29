/**
 * Sample React Native App
 * https://github.com/facebook/react-native
 *
 * Generated with the TypeScript template
 * https://github.com/react-native-community/react-native-template-typescript
 *
 * @format
 */

import React, { useState, useEffect } from 'react';
import { SafeAreaView, StatusBar } from 'react-native';

import { EngineView, useEngine } from '@babylonjs/react-native';
import { Scene, Vector3, Mesh, ArcRotateCamera, Camera, PBRMetallicRoughnessMaterial, Color3 } from '@babylonjs/core';


const App = () => {
  const engine = useEngine();
  const [camera, setCamera] = useState<Camera>();

  useEffect(() => {
    if (engine) {
      const scene = new Scene(engine);
      scene.createDefaultCamera(true);
      (scene.activeCamera as ArcRotateCamera).beta -= Math.PI / 8;
      setCamera(scene.activeCamera!);
      scene.createDefaultLight(true);

      const box = Mesh.CreateBox("box", 0.3, scene);
      const mat = new PBRMetallicRoughnessMaterial("mat", scene);
      mat.metallic = 1;
      mat.roughness = 0.5;
      mat.baseColor = Color3.Red();
      box.material = mat;

      scene.beforeRender = function () {
        box.rotate(Vector3.Up(), 0.005 * scene.getAnimationRatio());
      };
    }
  }, [engine]);

  return (
    <>
      <StatusBar barStyle="dark-content" />
      <SafeAreaView style={{flex: 1}}>
        <EngineView style={{flex: 1}} camera={camera} />
      </SafeAreaView>
    </>
  );
};

export default App;
