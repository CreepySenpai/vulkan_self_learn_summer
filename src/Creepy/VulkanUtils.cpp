#include <Creepy/VulkanUtils.hpp>
#include <print>
#include <stb/stb_image_write.hpp>

namespace Creepy {

    constexpr int totalChannel{4};

    void SaveImageToFile(const void* data, const uint32_t width, const uint32_t height) {
        if(!stbi_write_png("./res/textures/captures/suck.png", width, height, totalChannel, data, 0)){
            std::println("Failed to capture image");
        }
    }

}