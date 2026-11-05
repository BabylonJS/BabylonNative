import path from "node:path";

import { generateSolution } from "./node_modules/react-native-test-app/windows/app.mjs";

const destination = path.resolve(process.argv[2] ?? "windows");
const error = await generateSolution(destination, {
    autolink: true,
    useFabric: false,
    useHermes: true,
    useNuGet: true,
});

if (error) {
    throw new Error(error);
}
