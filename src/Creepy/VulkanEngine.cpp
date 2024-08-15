#include <utility>
#include <Creepy/VulkanEngine.hpp>
#include <Creepy/VulkanUtils.hpp>
#include <Creepy/VulkanAllocator.hpp>
#include <Creepy/VulkanShader.hpp>

#include <GLFW/glfw3.h>
#include <imgui/imgui.hpp>
#include <imgui/imgui_impl_glfw.hpp>
#include <imgui/imgui_impl_vulkan.hpp>

#include <glm/glm.hpp>

#include <Creepy/Model.hpp>

namespace Creepy {

    VulkanEngine::VulkanEngine() : m_width{600}, m_height{600}{
        this->createWindow();
        this->createInstance();
        this->createDebugMessage();
        this->createSurface();
        this->createDevice();
        this->createAllocator();
        this->createQueue();
        this->createCommandPool();
        this->createSwapchain();
        this->createCommandBuffer();
        this->createSync();
        this->createDescriptorPool();

        this->initImGUI();

        this->createDescriptorSets();

        //Note(Creepy): We need resources before create Pipelines
        this->createResources();

        this->createPipelines();
    }

    VulkanEngine::~VulkanEngine(){
        m_logicalDevice.waitIdle();
        m_clearner.Execute();
    }

    void VulkanEngine::Run(){
        while(!glfwWindowShouldClose(m_window)){

            glfwGetWindowSize(m_window, &m_width, &m_height);
            glfwPollEvents();

            // Draw Here
            this->draw();

            m_currentFrame = (m_currentFrame + 1) % m_totalFrames;
        }
    }

    void VulkanEngine::createWindow() {
        m_window = glfwCreateWindow(m_width, m_height, "Creepy", nullptr, nullptr);
        
        m_clearner.AddJob([this]{
            glfwDestroyWindow(m_window);
        });
    }

    void VulkanEngine::createInstance() {
        vk::ApplicationInfo appInfo{};
        appInfo.pApplicationName = "Creepy";
        appInfo.pEngineName = "Creepy";
        auto apiV = vk::enumerateInstanceVersion().value;   // vk::ApiVersion13
        appInfo.apiVersion = apiV;
        appInfo.engineVersion = apiV;
        
        uint32_t glfwExtensionCount{};
        auto glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector glfwExtensionss(glfwExtensions, glfwExtensions + glfwExtensionCount);
        glfwExtensionss.emplace_back(vk::EXTDebugUtilsExtensionName);

        constexpr std::array layers{
            "VK_LAYER_KHRONOS_validation"
        };

        vk::InstanceCreateInfo instanceInfo{};
        instanceInfo.flags = vk::InstanceCreateFlags{};
        instanceInfo.pApplicationInfo = &appInfo;
        instanceInfo.enabledExtensionCount = static_cast<uint32_t>(glfwExtensionss.size());
        instanceInfo.ppEnabledExtensionNames = glfwExtensionss.data();
        instanceInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
        instanceInfo.ppEnabledLayerNames = layers.data();
        
        {   // For Disable Validation
            // vk::ValidationCheckEXT valicheck = vk::ValidationCheckEXT::eAll;
            // vk::ValidationFlagsEXT validation{};
            // validation.disabledValidationCheckCount = 1;
            // validation.pDisabledValidationChecks = &valicheck;
            // instanceInfo.pNext = &validation;
        }

        {   // For enable some validation features
            // std::array enables{
            //     vk::ValidationFeatureEnableEXT::eBestPractices
            // };
            // std::array disables{
            //     vk::ValidationFeatureDisableEXT::eUniqueHandles
            // };
    
            // vk::ValidationFeaturesEXT validation{};
            // validation.disabledValidationFeatureCount = static_cast<uint32_t>(disables.size());
            // validation.pDisabledValidationFeatures = disables.data();
            // validation.enabledValidationFeatureCount = static_cast<uint32_t>(enables.size());
            // validation.pEnabledValidationFeatures = enables.data();

            // instanceInfo.pNext = &validation;
        }

        auto res = vk::createInstance(instanceInfo);

        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create Instance");
        }

        m_instance = res.value;

        m_dispatcher = vk::DispatchLoaderDynamic{m_instance, vkGetInstanceProcAddr};

        m_clearner.AddJob([this]{
            m_instance.destroy();
        });

        
    }

    void VulkanEngine::createDebugMessage() {
        vk::DebugUtilsMessengerCreateInfoEXT debugInfo{};
        debugInfo.flags = vk::DebugUtilsMessengerCreateFlagsEXT{};
        debugInfo.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eError | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning;
        debugInfo.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;
        debugInfo.pfnUserCallback = debugMessageCallback;
        

        auto res = m_instance.createDebugUtilsMessengerEXT(debugInfo, nullptr, m_dispatcher);

        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create Debug");
        }

        m_debugUtils = res.value;

        m_clearner.AddJob([this]{
            m_instance.destroyDebugUtilsMessengerEXT(m_debugUtils, nullptr, m_dispatcher);
        });
    }

    void VulkanEngine::createSurface() {
        VkSurfaceKHR tempSurface;
        if(glfwCreateWindowSurface(m_instance, m_window, nullptr, &tempSurface) != VK_SUCCESS){
            std::println("Failed Create Surface");
        }
        m_surface = vk::SurfaceKHR{tempSurface};

        m_clearner.AddJob([this]{
            m_instance.destroySurfaceKHR(m_surface);
        });
    }

    bool selectPhysicalDevice(const vk::PhysicalDevice physicalDev){
        auto queues = physicalDev.getQueueFamilyProperties();
        auto supportExtensions = physicalDev.enumerateDeviceExtensionProperties().value;

        constexpr std::array requireExtensions{
            vk::KHRSwapchainExtensionName
        };

        auto isSupportRequireExtensions = std::all_of(requireExtensions.begin(), requireExtensions.end(), [&supportExtensions](auto requireExtension){
            return std::any_of(supportExtensions.begin(), supportExtensions.end(), [&requireExtension](auto extension){
                return std::string{extension.extensionName.data()} == requireExtension;
            });
        });

        if(isSupportRequireExtensions){
            std::println("Support All");
        }

        for(auto queue : queues){
            if(queue.queueFlags & vk::QueueFlagBits::eGraphics){
                return true;
            }
        }

        return false;
    }

    void VulkanEngine::createDevice() {
        auto physicalDevs = m_instance.enumeratePhysicalDevices().value;

        for(auto physicalDev : physicalDevs){
            auto properties = physicalDev.getProperties();
            std::println("apiVersion: {}", properties.apiVersion);
            std::println("deviceID: {}", properties.deviceID);
            std::println("deviceName: {}", properties.deviceName.data());
            std::println("driverVersion: {}", properties.driverVersion);
            std::println("------------------------------------------");
            
            auto [i, j, k] = physicalDev.getProperties2<vk::PhysicalDeviceProperties2, vk::PhysicalDeviceVulkan11Properties, vk::PhysicalDeviceVulkan12Properties>();

            std::println("Ok :{}", k.driverName.data());

            if(selectPhysicalDevice(physicalDev)){
                m_physicalDevice = physicalDev;
            }
        }

        constexpr std::array layers{
           "VK_LAYER_KHRONOS_validation"
        };

        constexpr std::array extensions{
            vk::KHRSwapchainExtensionName
        };
        
        std::println("Graphic Queue: {}", findQueueFamilyIndex(m_physicalDevice, vk::QueueFlagBits::eGraphics));
        std::println("Compute Queue: {}", findQueueFamilyIndex(m_physicalDevice, vk::QueueFlagBits::eCompute));
        std::println("Transfer Queue: {}", findQueueFamilyIndex(m_physicalDevice, vk::QueueFlagBits::eTransfer));
        std::println("Present Queue: {}", findPresentQueueFamilyIndex(m_physicalDevice, m_surface));

        const std::array totalCreateQueues{
            findQueueFamilyIndex(m_physicalDevice, vk::QueueFlagBits::eGraphics),
            // findPresentQueueFamilyIndex(m_physicalDevice, m_surface),
        };

        constexpr float queuePriorities[]{1.0f};

        //TODO: Separete  
        vk::DeviceQueueCreateInfo queueInfo{};
        queueInfo.flags = vk::DeviceQueueCreateFlags{};
        queueInfo.queueCount = static_cast<uint32_t>(totalCreateQueues.size());
        queueInfo.queueFamilyIndex = 0;
        queueInfo.pQueuePriorities = queuePriorities;

        vk::StructureChain<vk::DeviceCreateInfo, vk::PhysicalDeviceVulkan12Features, vk::PhysicalDeviceVulkan13Features> deviceChain;
        
        auto& deviceInfo = deviceChain.get<vk::DeviceCreateInfo>();
        deviceInfo.flags = vk::DeviceCreateFlags{};
        deviceInfo.enabledLayerCount = static_cast<uint32_t>(layers.size());
        deviceInfo.ppEnabledLayerNames = layers.data();
        deviceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        deviceInfo.ppEnabledExtensionNames = extensions.data();
        deviceInfo.queueCreateInfoCount = 1;
        deviceInfo.pQueueCreateInfos = &queueInfo;

        // Enable Features
        auto& vulkan12Features = deviceChain.get<vk::PhysicalDeviceVulkan12Features>();
        vulkan12Features.bufferDeviceAddress = vk::True;
        auto& vulkan13Features = deviceChain.get<vk::PhysicalDeviceVulkan13Features>();
        vulkan13Features.dynamicRendering = vk::True;
        vulkan13Features.synchronization2 = vk::True;

        auto res = m_physicalDevice.createDevice(deviceInfo);

        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create Device");
        }

        m_logicalDevice = res.value;

        m_clearner.AddJob([this]{
            m_logicalDevice.destroy();
        });
    }
    
    void VulkanEngine::createAllocator() {

        InitAllocator(m_instance, m_logicalDevice, m_physicalDevice);

        m_clearner.AddJob([this]{
            DestroyAllocator();
        });
    }

    void VulkanEngine::createQueue() {
        
        m_graphicQueue = m_logicalDevice.getQueue(findQueueFamilyIndex(m_physicalDevice, vk::QueueFlagBits::eGraphics), 0);
        
        vk::DeviceQueueInfo2 queueInfo{};
        queueInfo.flags = vk::DeviceQueueCreateFlags{};
        queueInfo.queueFamilyIndex = findQueueFamilyIndex(m_physicalDevice, vk::QueueFlagBits::eGraphics);
        queueInfo.queueIndex = 0;
        m_graphicQueue = m_logicalDevice.getQueue2(queueInfo);

        // Because Graphic Queue & Present Queue Same Family Index So We Skip
        queueInfo.queueFamilyIndex = findPresentQueueFamilyIndex(m_physicalDevice, m_surface);
        queueInfo.queueIndex = 0;
        m_presentQueue = m_logicalDevice.getQueue2(queueInfo);
    }

    void VulkanEngine::createCommandPool() {

        vk::CommandPoolCreateInfo info{};
        info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        info.queueFamilyIndex = findQueueFamilyIndex(m_physicalDevice, vk::QueueFlagBits::eGraphics);
        
        m_cmdPool = m_logicalDevice.createCommandPool(info).value;

        m_clearner.AddJob([this]{
            m_logicalDevice.destroyCommandPool(m_cmdPool);
        });
    }

    vk::PresentModeKHR choosePresentMode(const vk::PhysicalDevice physicDev, const vk::SurfaceKHR surface){
        auto&& presentList = physicDev.getSurfacePresentModesKHR(surface).value;

        for(auto presentMode : presentList){
            if(presentMode == vk::PresentModeKHR::eMailbox){
                std::println("Support Mailbox");
                return presentMode;
            }
            if(presentMode == vk::PresentModeKHR::eFifo){
                std::println("Support eFifo");
            }
            if(presentMode == vk::PresentModeKHR::eFifoRelaxed){
                std::println("Support eFifoRelaxed");
            }
            if(presentMode == vk::PresentModeKHR::eImmediate){
                std::println("Support eImmediate");
            }
            if(presentMode == vk::PresentModeKHR::eSharedContinuousRefresh){
                std::println("Support eSharedContinuousRefresh");
            }
            if(presentMode == vk::PresentModeKHR::eSharedDemandRefresh){
                std::println("Support eSharedDemandRefresh");
            }
        }

        return presentList.at(0);
    }

    vk::SurfaceFormatKHR chooseSurfaceFormat(const vk::PhysicalDevice physicDev, const vk::SurfaceKHR surface){
        auto&& surfaceFormats = physicDev.getSurfaceFormatsKHR(surface).value;

        for(auto sformat : surfaceFormats){
            if(sformat.format == vk::Format::eR8G8B8A8Unorm && sformat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear){
                std::println("Choose eR8G8B8A8Unorm");
                return sformat;
            }
        }

        return surfaceFormats.at(0);
    }
    

    void VulkanEngine::createSwapchain() {
        auto&& surfaceCap = m_physicalDevice.getSurfaceCapabilitiesKHR(m_surface).value;
        auto&& surfaceFormat = chooseSurfaceFormat(m_physicalDevice, m_surface);

        m_swapchain = Swapchain{m_logicalDevice, m_surface, 
            surfaceFormat.format, surfaceFormat.colorSpace, {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)}, 
            choosePresentMode(m_physicalDevice, m_surface), surfaceCap};
        
        m_clearner.AddJob([this]{
            m_swapchain.Destroy(m_logicalDevice);
        });

        m_totalFrames = m_swapchain.GetSwapchainImages().size();

        m_renderFrames.resize(m_totalFrames);

        std::println("Swapchain Image Info");
        for(int j{}; auto&& i : m_swapchain.GetSwapchainImages()){
            auto memRequire = m_logicalDevice.getImageMemoryRequirements(i);
            std::println("Image: {} - {} - {} - {}", j, memRequire.alignment, memRequire.memoryTypeBits, memRequire.size);
            ++j;
        }

    }

    void VulkanEngine::createCommandBuffer() {

        for(auto& frame : m_renderFrames){
            vk::CommandBufferAllocateInfo info{};
            info.commandBufferCount = 1;
            info.commandPool = m_cmdPool;
            info.level = vk::CommandBufferLevel::ePrimary;

            auto res = m_logicalDevice.allocateCommandBuffers(info);

            if(res.result != vk::Result::eSuccess){
                std::println("Failed Create CmdBuffer");
            }

            frame.m_commandBuffer = res.value.at(0);
        }

        m_clearner.AddJob([this]{
            for(auto& frame : m_renderFrames){
                m_logicalDevice.freeCommandBuffers(m_cmdPool, frame.m_commandBuffer);
            }
        });
    }


    void VulkanEngine::createSync() {
        
        for(auto& frame : m_renderFrames){
            vk::SemaphoreCreateInfo semInfo{};
            semInfo.flags = vk::SemaphoreCreateFlags{};

            frame.m_imageAvailableSemaphore = m_logicalDevice.createSemaphore(semInfo).value;
            frame.m_imageRenderedSemaphore = m_logicalDevice.createSemaphore(semInfo).value;

            vk::FenceCreateInfo fenceInfo{};
            fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;   // must signal
            
            frame.m_renderCompleteFence = m_logicalDevice.createFence(fenceInfo).value;
        }

        m_clearner.AddJob([this]{
            for(auto& frame : m_renderFrames){
                m_logicalDevice.destroySemaphore(frame.m_imageAvailableSemaphore);
                m_logicalDevice.destroySemaphore(frame.m_imageRenderedSemaphore);
                m_logicalDevice.destroyFence(frame.m_renderCompleteFence);
            }
        });
        
    }


    void VulkanEngine::createDescriptorPool() {
        
        constexpr uint32_t maxSets{100u};

        constexpr std::array descSizes{
            vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, 100u},
            vk::DescriptorPoolSize{vk::DescriptorType::eSampler, 100u},
            vk::DescriptorPoolSize{vk::DescriptorType::eStorageBuffer, 100u},
            vk::DescriptorPoolSize{vk::DescriptorType::eStorageImage, 100u},
            vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, 100u}
        };

        vk::DescriptorPoolCreateInfo descInfo{};
        descInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        descInfo.maxSets = maxSets;
        descInfo.poolSizeCount = static_cast<uint32_t>(descSizes.size());
        descInfo.pPoolSizes = descSizes.data();
        
        auto res = m_logicalDevice.createDescriptorPool(descInfo);

        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create DescriptorPool");
        }
        
        m_descriptorPool = res.value;

        m_clearner.AddJob([this]{
            m_logicalDevice.destroyDescriptorPool(m_descriptorPool);
        });
    }

    void VulkanEngine::initImGUI() {
        ImGui::CreateContext();

        auto& io = ImGui::GetIO();
        io.ConfigFlags = ImGuiConfigFlags_DockingEnable;

        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForVulkan(m_window, false);

        // Big Pool For ImGui
        constexpr std::array imguiDescriptorPoolSizes{
            vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eSampledImage, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eStorageImage, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eUniformTexelBuffer, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eStorageTexelBuffer, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eUniformBuffer, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eUniformBufferDynamic, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eStorageBufferDynamic, 1000},
            vk::DescriptorPoolSize{vk::DescriptorType::eInputAttachment, 1000},
        };

        constexpr uint32_t imguiMaxSets{1000u};

        vk::DescriptorPoolCreateInfo imguiDescPoolInfo{};
        imguiDescPoolInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
        imguiDescPoolInfo.maxSets = imguiMaxSets;
        imguiDescPoolInfo.poolSizeCount = static_cast<uint32_t>(imguiDescriptorPoolSizes.size());
        imguiDescPoolInfo.pPoolSizes = imguiDescriptorPoolSizes.data();

        auto res = m_logicalDevice.createDescriptorPool(imguiDescPoolInfo);

        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create ImGui Descriptor Pool");
        }

        ImGui_ImplVulkan_InitInfo imguiInfo{};
        imguiInfo.Instance = m_instance;
        imguiInfo.PhysicalDevice = m_physicalDevice;
        imguiInfo.Device = m_logicalDevice;
        imguiInfo.Queue = m_graphicQueue;
        //TODO: Store Queue Family Index
        imguiInfo.QueueFamily = findQueueFamilyIndex(m_physicalDevice, vk::QueueFlagBits::eGraphics);
        imguiInfo.MinImageCount = static_cast<uint32_t>(m_swapchain.GetSwapchainImages().size());
        imguiInfo.ImageCount = static_cast<uint32_t>(m_swapchain.GetSwapchainImages().size());
        imguiInfo.DescriptorPool = res.value;
        imguiInfo.UseDynamicRendering = true;


        const std::array swapchainImageFormat{
            m_swapchain.GetSwapchainImageFormat()
        };
        
        vk::PipelineRenderingCreateInfo renderingInfo{};
        renderingInfo.colorAttachmentCount = static_cast<uint32_t>(swapchainImageFormat.size());
        
        renderingInfo.pColorAttachmentFormats = swapchainImageFormat.data();

        imguiInfo.PipelineRenderingCreateInfo = renderingInfo;

        imguiInfo.MSAASamples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;

        ImGui_ImplVulkan_Init(&imguiInfo);
        ImGui_ImplVulkan_CreateFontsTexture();

        m_clearner.AddJob([this, imguiDescPool = res.value]{
            ImGui_ImplVulkan_Shutdown();
            
            ImGui_ImplGlfw_Shutdown();

            ImGui::DestroyContext();

            m_logicalDevice.destroyDescriptorPool(imguiDescPool);
        });
    }

    void VulkanEngine::createDescriptorSets() {

    }

    void VulkanEngine::createPipelines() {
        
        constexpr std::array dynamicStates{
            vk::DynamicState::eViewport, vk::DynamicState::eScissor
        };

        // Because we use interleave buffer type -> only need 1 binding
        constexpr std::array vertexBindings{
            vk::VertexInputBindingDescription{0, sizeof(Vertex), vk::VertexInputRate::eVertex}
        };
        
        //TODO: Maybe change offset
        constexpr std::array vertexAttributes{
            vk::VertexInputAttributeDescription{0, vertexBindings.at(0).binding, vk::Format::eR32G32B32Sfloat, 0},
            vk::VertexInputAttributeDescription{1, vertexBindings.at(0).binding, vk::Format::eR32G32B32Sfloat, sizeof(glm::vec3)},
            vk::VertexInputAttributeDescription{2, vertexBindings.at(0).binding, vk::Format::eR32G32Sfloat, sizeof(glm::vec3) * 2},
        };

        const Shader vertexShader{m_logicalDevice, readShaderSPVFile("./res/shaders/vertexShader.spv"), vk::ShaderStageFlagBits::eVertex};
        
        const Shader fragmentShader{m_logicalDevice, readShaderSPVFile("./res/shaders/fragmentShader.spv"), vk::ShaderStageFlagBits::eFragment};
    
        PipelineState backgroundState{};
        backgroundState.InitPipelineLayout({}, {});
        backgroundState.InitShaderStates(vertexShader.GetShaderModule(), fragmentShader.GetShaderModule());
        backgroundState.InitVertexInputState(vertexBindings, vertexAttributes);
        backgroundState.InitInputAssemblyState(vk::PrimitiveTopology::eTriangleList);
        backgroundState.InitViewportState(static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height));
        backgroundState.InitRasterizationState(vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise);
        backgroundState.InitMultiSamplerState();
        backgroundState.InitDepthStencilState(vk::CompareOp::eLess);
        backgroundState.InitColorBlendState();
        backgroundState.InitDynamicState(dynamicStates);

        const std::array colorAttachmentFormats{
            m_colorImage.GetImageFormat(),
            //  m_swapchainImageFormat
        };

        //TODO: use images and depth format
        backgroundState.InitRenderingInfo(colorAttachmentFormats, m_depthImage.GetImageFormat());

        m_backgroundPipeline.Build(m_logicalDevice, backgroundState);

        m_clearner.AddJob([this]{
            m_backgroundPipeline.Destroy(m_logicalDevice);
        });
        

        vertexShader.Destroy(m_logicalDevice);
        fragmentShader.Destroy(m_logicalDevice);
    }
    
    void VulkanEngine::recreateSwapchain() {
        if(m_isSwapchainResizing){
            return;
        }
        
        m_logicalDevice.waitIdle();
        
        m_isSwapchainResizing = true;
        // auto&& surfaceCap = m_physicalDevice.getSurfaceCapabilitiesKHR(m_surface).value;
        auto&& surfaceFormat = chooseSurfaceFormat(m_physicalDevice, m_surface);

        m_swapchain.Recreate(m_logicalDevice, m_surface, 
        surfaceFormat.format, surfaceFormat.colorSpace, 
        {static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height)}, 
        choosePresentMode(m_physicalDevice, m_surface), 
         m_physicalDevice.getSurfaceCapabilitiesKHR(m_surface).value);

        m_isSwapchainResizing = false;
    }

    void VulkanEngine::createResources() {
        this->createImageResources();
        this->createBufferResources();
    }

    void VulkanEngine::createImageResources() {
        
        m_colorImage = Image{m_logicalDevice, static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), 
            vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc 
            | vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eStorage
            , vk::ImageAspectFlagBits::eColor};

        m_depthImage = Image{m_logicalDevice, static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), 
            vk::Format::eD24UnormS8Uint, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth};

        m_clearner.AddJob([this]{
            m_colorImage.Destroy(m_logicalDevice);
            m_depthImage.Destroy(m_logicalDevice);
        });
    }

    // Note(Creepy): All device local buffer must be upload data in begin()/end() command buffer block -> recording state
    void VulkanEngine::createBufferResources() {

        Model sick{"./res/models/shiba.gltf", m_logicalDevice, m_cmdPool, m_graphicQueue};


        const std::array vertices{
            Vertex{.Position = glm::vec3{0.0f, -0.5f, 0.0f}, .Normal = glm::vec3{1.0f, 1.0f, 1.0f}},
            Vertex{.Position = glm::vec3{0.5f, 0.5f, 0.0f}, .Normal = glm::vec3{0.0f, 1.0f, 1.0f}},
            Vertex{.Position = glm::vec3{-0.5f, 0.5f, 0.0f}, .Normal = glm::vec3{0.0f, 0.0f, 1.0f}},
        };

        m_triangleVertexBuffer = VertexBuffer{m_logicalDevice, vertices.size() * sizeof(Vertex)};
        m_triangleVertexBuffer.UploadData(m_logicalDevice, m_cmdPool, m_graphicQueue, vertices);

        m_clearner.AddJob([this]{
            m_triangleVertexBuffer.Destroy(m_logicalDevice);
        });

        sick.Destroy(m_logicalDevice);
    }

    void VulkanEngine::draw() {
        auto&& currentCommandBuffer = m_renderFrames.at(m_currentFrame).m_commandBuffer;
        auto&& currentFence = m_renderFrames.at(m_currentFrame).m_renderCompleteFence;
        auto&& currentImageAvailableSemaphore = m_renderFrames.at(m_currentFrame).m_imageAvailableSemaphore;
        auto&& currentImageRenderedSemaphore = m_renderFrames.at(m_currentFrame).m_imageRenderedSemaphore;
        

        m_logicalDevice.waitForFences(currentFence, vk::False, std::numeric_limits<uint64_t>::max());
        m_logicalDevice.resetFences(currentFence);

        auto waitRes = m_logicalDevice.acquireNextImageKHR(m_swapchain.GetSwapchainHandle(), std::numeric_limits<uint64_t>::max(), currentImageAvailableSemaphore);
        
        if(waitRes.result != vk::Result::eSuccess){
            //TODO: Need to recreate swapchain
            if(waitRes.result == vk::Result::eErrorOutOfDateKHR){
                this->recreateSwapchain();
            }
        }

        const uint32_t imageIndex{waitRes.value};
        const auto& currentSwapchainImage = m_swapchain.GetSwapchainImages().at(imageIndex);

        vk::CommandBufferBeginInfo beginCmdInfo{};
        beginCmdInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        currentCommandBuffer.begin(beginCmdInfo);

        // Begin render pass
        vk::RenderingInfo dynamicRenderInfo{};
        dynamicRenderInfo.flags = vk::RenderingFlags{};
        // dynamicRenderInfo.

        // Draw Call Here

        //TODO: Change
        imageLayoutTransition(currentCommandBuffer, currentSwapchainImage, 
            vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral, 
            vk::AccessFlagBits2::eMemoryWrite, vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryRead,
            vk::PipelineStageFlagBits2::eAllCommands, vk::PipelineStageFlagBits2::eAllCommands);

        vk::ClearColorValue clearValue{1.0f, 0.0f, 0.0f, 1.0f};

        vk::ImageSubresourceRange someTest{};
        someTest.aspectMask = vk::ImageAspectFlagBits::eColor;
        someTest.baseArrayLayer = 0;
        someTest.baseMipLevel = 0;
        someTest.levelCount = vk::RemainingMipLevels;
        someTest.layerCount = vk::RemainingArrayLayers;
        
        currentCommandBuffer.clearColorImage(currentSwapchainImage, vk::ImageLayout::eGeneral, clearValue, someTest);

        // We cannot call image layout transition in here
        // currentCommandBuffer.beginRendering(dynamicRenderInfo);

        // currentCommandBuffer.endRendering();

         // End Draw Call
        imageLayoutTransition(currentCommandBuffer, currentSwapchainImage, 
            vk::ImageAspectFlagBits::eColor, vk::ImageLayout::eGeneral, vk::ImageLayout::ePresentSrcKHR, 
            vk::AccessFlagBits2::eMemoryRead, vk::AccessFlagBits2::eMemoryRead | vk::AccessFlagBits2::eMemoryWrite,
            vk::PipelineStageFlagBits2::eAllCommands, vk::PipelineStageFlagBits2::eBottomOfPipe);

        currentCommandBuffer.end();


        constexpr std::array<vk::PipelineStageFlags, 1> waitStages{
            vk::PipelineStageFlagBits::eColorAttachmentOutput
        };

        vk::SubmitInfo submitInfo{};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &currentCommandBuffer;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &currentImageAvailableSemaphore;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &currentImageRenderedSemaphore;
        submitInfo.pWaitDstStageMask = waitStages.data();
        
        auto submitRes = m_presentQueue.submit(submitInfo, currentFence);

        if(submitRes != vk::Result::eSuccess){
            //TODO: Recreate swapchain
            if(submitRes == vk::Result::eErrorOutOfDateKHR){
                this->recreateSwapchain();
            }
        }

        const std::array presentSwapchains{
            m_swapchain.GetSwapchainHandle()
        };
        vk::PresentInfoKHR presentInfo{};
        presentInfo.swapchainCount = static_cast<uint32_t>(presentSwapchains.size());
        presentInfo.pSwapchains = presentSwapchains.data();
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &currentImageRenderedSemaphore;
        

        auto presentRes = m_presentQueue.presentKHR(presentInfo);

        if(presentRes != vk::Result::eSuccess){
            //TODO: Recreate swapchain
            if(presentRes == vk::Result::eErrorOutOfDateKHR){
                this->recreateSwapchain();
            }
        }
    }

    void VulkanEngine::drawBackground() {
        
    }
}