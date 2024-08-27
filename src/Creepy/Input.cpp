#include <utility>
#include <unordered_set>
#include <Creepy/Input.hpp>
#include <GLFW/glfw3.h>

namespace Creepy{
    struct MouseData{
        glm::vec2 Position{}, OldPosition{}, Scroll{};
        bool IsInWindow{}, WasInWindow{};
        std::unordered_set<MouseButton> PressedButtons;
        std::unordered_set<MouseButton> ReleasedButtons;
        std::unordered_set<MouseButton> HoldButtons;
    };

    MouseData s_mouseData{};

    struct KeyBoardData{
        uint32_t UnicodeChar{};
        KeyCode RepeatKey{};
        KeyCode CurrentKey{};
        std::unordered_set<KeyCode> PressedKeys;
        std::unordered_set<KeyCode> ReleasedKeys;
        std::unordered_set<KeyCode> HoldKeys;
    };

    KeyBoardData s_keyBoardData{};

    void Mouse::RegisterMouseEvent(GLFWwindow* window) {
        glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xPos, double yPos){
            s_mouseData.Position.x = xPos;
            s_mouseData.Position.y = yPos;
        });
        
        glfwSetCursorEnterCallback(window, [](GLFWwindow* window, int entered){
            s_mouseData.IsInWindow = static_cast<bool>(entered);
        });
        
        glfwSetScrollCallback(window, [](GLFWwindow* window, double xOffset, double yOffset){
            s_mouseData.Scroll.x = xOffset;
            s_mouseData.Scroll.y = yOffset;
        });

        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods){
            if(action == GLFW_PRESS){
                s_mouseData.PressedButtons.insert(static_cast<MouseButton>(button));
                s_mouseData.HoldButtons.insert(static_cast<MouseButton>(button));

            }else if(action == GLFW_RELEASE){
                s_mouseData.ReleasedButtons.insert(static_cast<MouseButton>(button));
                s_mouseData.HoldButtons.erase(static_cast<MouseButton>(button));
            }
        });
    }

    void Mouse::PreProcessEveryFrame() {
        s_mouseData.OldPosition.x = s_mouseData.Position.x;
        s_mouseData.OldPosition.y = s_mouseData.Position.y;

        s_mouseData.WasInWindow = s_mouseData.IsInWindow;
        s_mouseData.PressedButtons.clear();
        s_mouseData.ReleasedButtons.clear();
        s_mouseData.Scroll.x = 0.0f;
        s_mouseData.Scroll.y = 0.0f;
    }

    bool Mouse::IsMousePress(MouseButton button) {
        return s_mouseData.PressedButtons.contains(button);
    }

    bool Mouse::IsMouseHold(MouseButton button) {
        return s_mouseData.HoldButtons.contains(button);
    }

    glm::vec2 Mouse::GetMousePosition() {
        return s_mouseData.Position;
    }

    bool Mouse::IsMouseInWindow() {
        return s_mouseData.IsInWindow;
    }

    glm::vec2 Mouse::GetDeltaMousePosition() {
        return s_mouseData.Position - s_mouseData.OldPosition;
    }

    void KeyBoard::RegisterKeyEvent(GLFWwindow* window) {
        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scanCode, int action, int mods){
            if(action == GLFW_PRESS){
                s_keyBoardData.PressedKeys.insert(static_cast<KeyCode>(key));
                s_keyBoardData.HoldKeys.insert(static_cast<KeyCode>(key));
                s_keyBoardData.CurrentKey = static_cast<KeyCode>(key);
            }
            else if(action == GLFW_REPEAT){
                s_keyBoardData.RepeatKey = static_cast<KeyCode>(key);
            }
            else if(action == GLFW_RELEASE){
                s_keyBoardData.ReleasedKeys.insert(static_cast<KeyCode>(key));
                s_keyBoardData.HoldKeys.erase(static_cast<KeyCode>(key));
            }
        });

        glfwSetCharCallback(window, [](GLFWwindow* window, uint32_t codePoint){
            s_keyBoardData.UnicodeChar = codePoint;
        });
    }

    void KeyBoard::PreProcessEveryFrame() {
        s_keyBoardData.RepeatKey = KeyCode::NONE;
        s_keyBoardData.CurrentKey = KeyCode::NONE;
        s_keyBoardData.UnicodeChar = 0u;
        s_keyBoardData.PressedKeys.clear();
        s_keyBoardData.ReleasedKeys.clear();
    }

    bool KeyBoard::IsKeyPress(KeyCode keyCode) {
        return s_keyBoardData.PressedKeys.contains(keyCode);
    }

    bool KeyBoard::IsKeyHold(KeyCode keyCode) {
        return s_keyBoardData.HoldKeys.contains(keyCode);
    }

    bool KeyBoard::IsKeyRepeat(KeyCode keyCode) {
        return s_keyBoardData.RepeatKey == keyCode;
    }

    char KeyBoard::GetChar() {
        auto character = std::to_underlying(s_keyBoardData.CurrentKey);
        // Not character
        if(character > 128u){
            return 0;
        }
        return static_cast<char>(character);
    }
};