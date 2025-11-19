const path = require('path');
const webpack = require('webpack');

module.exports = {
  target: 'web',
  mode: 'development', // or 'production'
  devtool: false,
  entry: {
<<<<<<< HEAD:Apps/UnitTests/webpack.config.js
    tests: './Scripts/tests.ts',
    unittests_performance_shadercache: './Scripts/unittests_performance_shadercache.ts'
=======
    "tests.javaScript.all": './src/tests.javaScript.all.ts',
    "tests.nativeEngine.shaderCache": './src/tests.nativeEngine.shaderCache.ts',
  },
  externals: {
    "@babylonjs/core": "BABYLON",
    "@babylonjs/materials": "BABYLON"
>>>>>>> 4c76fc4efb820f6f793f3165dd5f12f3f59af702:Apps/UnitTests/JavaScript/webpack.config.js
  },
  output: {
    filename: '[name].js',
    path: path.resolve(__dirname, 'dist'),
    chunkFormat: false,
  },
  plugins: [
    new webpack.ProvidePlugin({
      process: 'process/browser',
      Buffer: ['buffer', 'Buffer'],
    }),
    new webpack.optimize.LimitChunkCountPlugin({
      maxChunks: 1, // ensures all code is in a single chunk
    }),
    new webpack.NormalModuleReplacementPlugin(
      /^node:(.+)$/,
      (resource) => {
        resource.request = resource.request.replace(/^node:/, '');
      }
    ),
  ],
  resolve: {
    extensions: ['.ts', '.js'],
    fallback: {
        'fs': false,
        'path': false,
        'util': false,
        'process': false,
        'buffer': false,
        stream: require.resolve('stream-browserify'),
        'events': false,
        'assert': false,
        'crypto': false,
        'url': false,
        'querystring': false,
        'http': false,
        'https': false,
        'zlib': false,
        'tty': false,
        'os': false,
        'child_process': false,
        'net': false,
        'dgram': false,
        'dns': false,
        'readline': false,
        'repl': false,
        'cluster': false,
        'vm': false,
        'domain': false,
        'constants': false,
        'module': false,
        'timers': false
    }
  },
  module: {
    rules: [
        {
          test: /\.(js|ts)$/,
          exclude: (modulePath) => {
            return (
              /node_modules/.test(modulePath) &&
              !/node_modules[\\/](?:@babylonjs|mocha|chai)/.test(modulePath)
            );
          },
          use: {
          loader: 'babel-loader',
          options: {
            compact: false,
            presets: [
              ['@babel/preset-env', {
                targets: {
                  ie: '11'
                }
              }]
            ],
            plugins: [
              path.resolve(__dirname, './babel-plugin-replace-bigint')
            ]
          }
        },
      }
      ],
  },
  watch: false, // enables file watcher
};
