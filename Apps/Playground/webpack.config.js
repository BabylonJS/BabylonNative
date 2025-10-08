const path = require('path');
const glob = require('glob');
const ForkTsCheckerWebpackPlugin = require('fork-ts-checker-webpack-plugin');

const entries = {};
glob.sync('./Scripts/**/*.ts').forEach(file => {
  const name = path.basename(file, '.ts');
  entries[name] = file;
});

module.exports = {
  entry: entries,
  output: {
    path: path.resolve(__dirname, 'dist'),
    filename: '[name].js',
    chunkFormat: 'array-push',
    environment: {
      arrowFunction: false,
      const: false,
      destructuring: false,
      forOf: false
    }
  },
  resolve: {
    extensions: ['.ts', '.js']
  },
  module: {
    rules: [
      {
        test: /\.[jt]s$/, // handles .js and .ts
        exclude: /(node_modules\/(?!@babylonjs))/,
        use: {
          loader: 'babel-loader',
          options: {
            presets: [
              ['@babel/preset-env', { targets: { ie: '11' } }],
              ['@babel/preset-typescript'],
            ],
            compact: true,
            comments: false,
          },
        },
      },
    ],
  },
  plugins: [
    // Runs tsc type checking
    new ForkTsCheckerWebpackPlugin({
      async: false, // fail build on type errors
      typescript: {
        configFile: path.resolve(__dirname, 'tsconfig.json'),
        diagnosticOptions: {
          syntactic: true,
          semantic: true
        }
      }
    })
  ],
  target: ['webworker'],
  mode: 'production',
  optimization: {
    minimize: false, // syntax error with minification
  }
};