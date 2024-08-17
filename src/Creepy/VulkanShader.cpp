#include <print>
#include <Creepy/VulkanShader.hpp>


namespace Creepy{
    Shader::Shader(const vk::Device device, std::span<const char> shaderData, vk::ShaderStageFlagBits shaderStage){

        // vk::ShaderCreateInfoEXT shaderInfo{};
        // shaderInfo.flags = vk::ShaderCreateFlagsEXT{};
        // shaderInfo.stage = shaderStage;
        // shaderInfo.codeType = vk::ShaderCodeTypeEXT::eSpirv;
        // shaderInfo.pCode = shaderData.data();
        // shaderInfo.pName = "main";
        
        vk::ShaderModuleCreateInfo shaderModuleInfo{};
        shaderModuleInfo.flags = vk::ShaderModuleCreateFlags{};
        shaderModuleInfo.codeSize = shaderData.size();
        shaderModuleInfo.pCode = reinterpret_cast<const uint32_t*>(shaderData.data());
        
        
        auto res = device.createShaderModule(shaderModuleInfo);
        
        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create Shader Module");
        }

        m_shaderModule = res.value;
    }


    void Shader::Destroy(const vk::Device device) const {
        device.destroyShaderModule(m_shaderModule);
    }

    vk::ShaderModule Shader::GetShaderModule() const {
        return m_shaderModule;
    }
}