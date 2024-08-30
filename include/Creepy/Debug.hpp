#pragma once


namespace Creepy{

    struct Debug
    {
        static void BeginFrame();

        static void DrawFrame();

        static void DrawTransformData(struct TransformData& transformData);

        static void DrawLightData(struct LightData& lightData);

        static void EndFrame();
    };
    

}