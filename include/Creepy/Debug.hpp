#pragma once


namespace Creepy{

    struct Debug
    {
        static void BeginFrame();

        static void DrawFrame();

        static void DrawUniformData(struct UniformData& uniformData);

        static void EndFrame();
    };
    

}