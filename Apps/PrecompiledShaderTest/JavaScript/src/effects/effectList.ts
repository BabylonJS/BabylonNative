import type { AbstractEngine } from "@babylonjs/core/Engines/abstractEngine";
import { createSimpleEffect } from "./simpleEffect";

type EffectInfo = {
    name: string;
    create: (engine: AbstractEngine) => void;
};

export const AllEffects: EffectInfo[] = [
    {
        name: "simple",
        create: (engine) => {
            createSimpleEffect(engine);
        },
    },
];
