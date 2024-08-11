#include <utility>
#include <Creepy/VulkanEngine.hpp>
#include <Creepy/VulkanUtils.hpp>
#include <Creepy/VulkanAllocator.hpp>
#include <Creepy/VulkanBuffer.hpp>

#include <GLFW/glfw3.h>
#include <imgui/imgui.hpp>
#include <imgui/imgui_impl_glfw.hpp>
#include <imgui/imgui_impl_vulkan.hpp>

#include <glm/glm.hpp>

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

        // this->createPipelines();

        this->createImageResources();
    }

    VulkanEngine::~VulkanEngine(){
        m_clearner.Execute();
    }

    void VulkanEngine::Run(){
        while(!glfwWindowShouldClose(m_window)){
            glfwGetWindowSize(m_window, &m_width, &m_height);
            glfwPollEvents();
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

        const uint32_t graphicQueueFamilyIndex = findQueueFamilyIndex(m_physicalDevice, vk::QueueFlagBits::eGraphics);
        auto&& surfaceCap = m_physicalDevice.getSurfaceCapabilitiesKHR(m_surface).value;
        
        vk::SwapchainCreateInfoKHR info{};
        
        info.flags = vk::SwapchainCreateFlagsKHR{};
        info.surface = m_surface;
        info.oldSwapchain = nullptr;
        info.presentMode = choosePresentMode(m_physicalDevice, m_surface);
        info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
        info.imageArrayLayers = 1;
        info.imageSharingMode = vk::SharingMode::eExclusive;
        // info.queueFamilyIndexCount = 1;
        // info.pQueueFamilyIndices = &graphicQueueFamilyIndex;

        const uint32_t imageCount{std::min(surfaceCap.maxImageCount, surfaceCap.minImageCount + 1)};
        info.minImageCount = imageCount;
        info.clipped = vk::True;
        info.preTransform = surfaceCap.currentTransform;

        auto&& surfaceFormat = chooseSurfaceFormat(m_physicalDevice, m_surface);
        m_swapchainImageFormat = surfaceFormat.format;
        info.imageFormat = surfaceFormat.format;
        info.imageColorSpace = surfaceFormat.colorSpace;
        
        // Make sure image W / H in bound
        info.imageExtent.width = std::max(surfaceCap.minImageExtent.width, std::min(static_cast<uint32_t>(m_width), surfaceCap.maxImageExtent.width));
        info.imageExtent.height = std::max(surfaceCap.minImageExtent.height, std::min(static_cast<uint32_t>(m_height), surfaceCap.maxImageExtent.height));
        
        std::println("Min: {} - {}", info.imageExtent.width, info.imageExtent.height);
        
        auto res = m_logicalDevice.createSwapchainKHR(info);

        if(res.result != vk::Result::eSuccess){
            std::println("Error createSwapchainKHR");
        }

        m_swapChain = res.value;

        m_clearner.AddJob([this]{
            m_logicalDevice.destroySwapchainKHR(m_swapChain);
        });
        

        m_swapchainImages = m_logicalDevice.getSwapchainImagesKHR(m_swapChain).value;

        std::println("Swapchain Image Info");
        for(int j{}; auto&& i : m_swapchainImages){
            auto memRequire = m_logicalDevice.getImageMemoryRequirements(i);
            std::println("Image: {} - {} - {} - {}", j, memRequire.alignment, memRequire.memoryTypeBits, memRequire.size);
            ++j;
        }

    }

    void VulkanEngine::createCommandBuffer() {
        vk::CommandBufferAllocateInfo info{};
        info.commandBufferCount = 1;
        info.commandPool = m_cmdPool;
        info.level = vk::CommandBufferLevel::ePrimary;

        auto res = m_logicalDevice.allocateCommandBuffers(info);

        if(res.result != vk::Result::eSuccess){
            std::println("Failed Create CmdBuffer");
        }

        m_commandBuffer = res.value.at(0);

        m_clearner.AddJob([this]{
            m_logicalDevice.freeCommandBuffers(m_cmdPool, m_commandBuffer);
        });
    }


    void VulkanEngine::createSync() {
        vk::SemaphoreCreateInfo semInfo{};
        semInfo.flags = vk::SemaphoreCreateFlags{};

        m_acquireImageSemaphore = m_logicalDevice.createSemaphore(semInfo).value;
        m_renderStartSemaphore = m_logicalDevice.createSemaphore(semInfo).value;

        vk::FenceCreateInfo fenceInfo{};
        fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;   // must signal
        
        m_renderCompleteFence = m_logicalDevice.createFence(fenceInfo).value;

        m_clearner.AddJob([this]{
            m_logicalDevice.destroySemaphore(m_acquireImageSemaphore);
            m_logicalDevice.destroySemaphore(m_renderStartSemaphore);
            m_logicalDevice.destroyFence(m_renderCompleteFence);
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
        imguiInfo.MinImageCount = static_cast<uint32_t>(m_swapchainImages.size());
        imguiInfo.ImageCount = static_cast<uint32_t>(m_swapchainImages.size());
        imguiInfo.DescriptorPool = res.value;
        imguiInfo.UseDynamicRendering = true;

        vk::PipelineRenderingCreateInfo renderingInfo{};
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachmentFormats = &m_swapchainImageFormat;

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

        constexpr std::array vertexBindings{
            vk::VertexInputBindingDescription{0, sizeof(glm::vec3), vk::VertexInputRate::eVertex},
            vk::VertexInputBindingDescription{1, sizeof(glm::vec3), vk::VertexInputRate::eVertex},
            vk::VertexInputBindingDescription{2, sizeof(glm::vec2), vk::VertexInputRate::eVertex},
        };
        
        //TODO: Maybe change offset
        constexpr std::array vertexAttributes{
            vk::VertexInputAttributeDescription{0, vertexBindings.at(0).binding, vk::Format::eR32G32B32Sfloat, 0},
            vk::VertexInputAttributeDescription{1, vertexBindings.at(1).binding, vk::Format::eR32G32B32Sfloat, sizeof(glm::vec3)},
            vk::VertexInputAttributeDescription{2, vertexBindings.at(2).binding, vk::Format::eR32G32Sfloat, sizeof(glm::vec3) * 2},
        };
        
        PipelineState backgroundState{};
        backgroundState.InitPipelineLayout({}, {});
        backgroundState.InitShaderStates({}, {});
        backgroundState.InitVertexInputState(vertexBindings, vertexAttributes);
        backgroundState.InitInputAssemblyState(vk::PrimitiveTopology::eTriangleList);
        backgroundState.InitViewportState(static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height));
        backgroundState.InitRasterizationState(vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eClockwise);
        backgroundState.InitMultiSamplerState();
        backgroundState.InitDepthStencilState(vk::CompareOp::eLess);
        backgroundState.InitColorBlendState();
        backgroundState.InitDynamicState(dynamicStates);

        //TODO: use imagess and depth format
        backgroundState.InitRenderingInfo({}, vk::Format::eD24UnormS8Uint);

        m_backgroundPipeline.Build(m_logicalDevice, {});

        m_clearner.AddJob([this]{
            m_backgroundPipeline.Destroy(m_logicalDevice);
        });
    }

    void VulkanEngine::createImageResources() {

        m_colorImage = Image{m_logicalDevice, static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), vk::Format::eR8G8B8A8Unorm, vk::ImageUsageFlagBits::eColorAttachment, vk::ImageAspectFlagBits::eColor};

        m_depthImage = Image{m_logicalDevice, static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), vk::Format::eD24UnormS8Uint, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::ImageAspectFlagBits::eDepth};

        m_clearner.AddJob([this]{
            m_colorImage.Destroy(m_logicalDevice);
            m_depthImage.Destroy(m_logicalDevice);
        });

        Buffer<BufferType::DEVICE_LOCAL> myBuffer1{m_logicalDevice, 160, vk::Format::eR32G32B32A32Sfloat, vk::BufferUsageFlagBits::eIndexBuffer};

        myBuffer1.UploadData(nullptr, 0);

        Buffer<BufferType::HOST_VISIBLE> myBuffer2{m_logicalDevice, 160, vk::Format::eR32G32B32A32Sfloat, vk::BufferUsageFlagBits::eIndexBuffer};

        myBuffer2.UploadData(nullptr, 0);

        Buffer<BufferType::HOST_COHERENT> myBuffer3{m_logicalDevice, 160, vk::Format::eR32G32B32A32Sfloat, vk::BufferUsageFlagBits::eIndexBuffer};

        myBuffer3.UploadData(nullptr, 0);

        myBuffer1.Destroy(m_logicalDevice);
        myBuffer2.Destroy(m_logicalDevice);
        myBuffer3.Destroy(m_logicalDevice);
    }

    void VulkanEngine::draw() {
        m_logicalDevice.waitForFences(m_renderCompleteFence, vk::False, std::numeric_limits<uint64_t>::max());
        m_logicalDevice.resetFences(m_renderCompleteFence);

        //TODO: Maybe don't need
        m_commandBuffer.reset();

        auto waitRes = m_logicalDevice.acquireNextImageKHR(m_swapChain, std::numeric_limits<uint64_t>::max(), m_acquireImageSemaphore);
        
        if(waitRes.result != vk::Result::eSuccess){
            //TODO: Need to recreate swapchain
        }

        const uint32_t imageIndex{waitRes.value};

        vk::CommandBufferBeginInfo beginCmdInfo{};
        beginCmdInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        m_commandBuffer.begin(beginCmdInfo);

        // Begin render pass
        vk::RenderingInfo dynamicRenderInfo{};
        dynamicRenderInfo.flags = vk::RenderingFlags{};
        
        
        m_commandBuffer.beginRendering(dynamicRenderInfo);

        m_commandBuffer.endRendering();

        m_commandBuffer.end();

        vk::SubmitInfo submitInfo{};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_commandBuffer;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &m_acquireImageSemaphore;
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &m_renderStartSemaphore;
        
        auto submitRes = m_presentQueue.submit(submitInfo, m_renderCompleteFence);

        if(submitRes != vk::Result::eSuccess){
            //TODO: Recreate swapchain
        }

        vk::PresentInfoKHR presentInfo{};
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_swapChain;
        presentInfo.pImageIndices = &imageIndex;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &m_renderStartSemaphore;

        auto presentRes = m_presentQueue.presentKHR(presentInfo);

        if(presentRes != vk::Result::eSuccess){
            //TODO: Recreate swapchain
        }
    }
}