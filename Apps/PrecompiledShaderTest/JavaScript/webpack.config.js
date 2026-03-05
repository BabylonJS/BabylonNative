import path from "path";
import { fileURLToPath } from "url";

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);

const SRC_DIR = path.resolve(__dirname, "./src");
const DIST_DIR = path.resolve(__dirname, "./dist");
const ENTRY = path.resolve(SRC_DIR, "./index.ts");

const buildConfig = function (env = {}) {
    const isProd = !!env.prod;

    return {
        context: __dirname,
        devtool: isProd ? false : "source-map",
        resolve: {
            extensions: [".ts", ".js"],
        },
        mode: isProd ? "production" : "development",
        entry: {
            index: ENTRY,
        },
        output: {
            path: DIST_DIR,
            filename: "[name].js",
            library: "[name]",
            libraryTarget: "umd",
            globalObject: 'globalThis',
            chunkFormat: false,
        },
        optimization: {
            minimize: true,
            splitChunks: false,
            runtimeChunk: false,
        },
        module: {
            rules: [
                {
                    test: /\.ts$/,
                    loader: "ts-loader",
                },
            ],
        },
    };
};

export default buildConfig;
