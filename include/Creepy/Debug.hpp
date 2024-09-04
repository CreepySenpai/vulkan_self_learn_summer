#pragma once

#include <vulkan/vulkan.hpp>
struct GLFWwindow;

namespace Creepy{

    struct Debug
    {
        static void Init(GLFWwindow* window, const vk::Instance instance, const vk::PhysicalDevice physicalDevice, const vk::Device logicalDevice, const vk::Queue graphicQueue, uint32_t graphicQueueIndex, uint32_t swapchainImageCount, vk::Format swapchainImageFormat);

        static void ShutDown(const vk::Device logicalDevice);

        static void BeginFrame();

        static void DrawFrame();

        static void DrawTransformData(struct TransformData& transformData);

        static void DrawLightData(struct LightData& lightData);

        static void EndFrame();
    };
    

}