cmake_minimum_required(VERSION 3.30.0)

set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 26)

project(SelfLearn1 VERSION 0.1.0 LANGUAGES C CXX)

file(GLOB CORENAPP ${CMAKE_SOURCE_DIR}/src/Creepy/*.cpp)
# file(GLOB VKBOOSTRAP ${CMAKE_SOURCE_DIR}/src/vkbootstrap/*.cpp)
file(GLOB VKALLOC ${CMAKE_SOURCE_DIR}/src/vkmemoryalloc/*.cpp)
file(GLOB IMGUI ${CMAKE_SOURCE_DIR}/src/imgui/*.cpp)

add_executable(SelfLearn1 main.cpp)

target_sources(${PROJECT_NAME} PUBLIC ${CORENAPP} ${VKALLOC} ${IMGUI})
# target_sources(${PROJECT_NAME} PUBLIC ${CORENAPP} ${VKBOOSTRAP} ${VKALLOC} ${IMGUI})

find_package(Vulkan REQUIRED)

target_include_directories(${PROJECT_NAME} PUBLIC 
                    ${CMAKE_SOURCE_DIR}/include
                    ${Vulkan_INCLUDE_DIRS}
                )
                
target_link_directories(${PROJECT_NAME} PUBLIC 
                    ${CMAKE_SOURCE_DIR}/lib
                )

target_link_libraries(${PROJECT_NAME} PUBLIC glfw3dll assimp.dll ${Vulkan_LIBRARIES} stdc++exp)

add_compile_definitions(VULKAN_HPP_NO_EXCEPTIONS)

add_compile_definitions(STB_IMAGE_IMPLEMENTATION)

add_compile_options(-fdiagnostics-color=always)