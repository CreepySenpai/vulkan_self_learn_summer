#include <GLFW/glfw3.h>
#include <Creepy/VulkanEngine.hpp>

int main(){
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    
    Creepy::VulkanEngine engine{};
    engine.Run();
    
    glfwTerminate();
    return 0;
}