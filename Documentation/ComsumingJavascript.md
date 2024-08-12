# Consuming JavaScript in Babylon Native

Babylon Native allows you to run JavaScript code that uses the Babylon.js API inside a native C++ application. The way that works is that Babylon Native will use a JavaScript engine and provide the required infrastructure for the Babylon.js engine to perform graphics commands. 

Loading the JavaScript files into the engine is completely up to the consuming application and must be done in a way that the engine has all the required dependencies to run each portion of the scripts. There is no real-time dependency resolution, and it is up to the consuming application to ensure that everything is loaded in the correct order.

## Loading Babylon.js using UMD

For simplicity we manually load all the required Babylon.js UMD modules in the example Playground application in this repo. The benefit to this approach is that changes can me made instantaneously in the application script (in this case ```YOUR_SCRIPT.js```) and the application can just be re-launched, and everything will work. No bundling process is required. 

```C++
Babylon::ScriptLoader loader{*runtime};

// Load JS dependencies
loader.LoadScript("app:///Scripts/babylon.max.js");
loader.LoadScript("app:///Scripts/babylonjs.loaders.js");
loader.LoadScript("app:///Scripts/babylonjs.materials.js");
loader.LoadScript("app:///Scripts/babylon.gui.js");
loader.LoadScript("app:///Scripts/meshwriter.min.js");

// Load your script
loader.LoadScript("app:///Scripts/YOUR_SCRIPT.js");
```

However, this is not the recommended way to deliver a production ready Babylon Native application. The bundle size of your application would be unnecessarily big (give it would need to have all required Babylon.js UMD files). Also, your application code would be very easily visible as plain text.

## Using a bundler to create a single JS file

> [!WARNING] Notes on consuming JavaScript code
> Starting with version 7.19.0 Babylon.js started using dynamic imports to achieve smaller bundle size. However, this feature is not currently supported in Babylon Native. Therefore, when consuming Babylon.js using ES6 and creating a bundle for it to be loaded in Babylon Native, one must specify that dynamic import should be turned off. This can be done using the [following instructions for Webpack](https://webpack.js.org/plugins/limit-chunk-count-plugin/) and setting the chunks count to 1. For those using rollupjs you should specify ```inlineDynamicImports = true``` following [this documentation](https://rollupjs.org/configuration-options/#output-inlinedynamicimports)

