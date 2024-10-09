#include <print>
#include <Creepy/Camera.hpp>
#include <Creepy/Input.hpp>
#include <GLFW/glfw3.h>

#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/rotate_vector.hpp>


extern constinit GLFWwindow* g_nativeWindow;

namespace Creepy {

    Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
        : m_fov{fov}, m_aspectRatio{aspectRatio}, m_nearClip{nearClip}, m_farClip{farClip}
    {
        this->updateProjectionMatrix();
    }

    void Camera::OnUpdate(double deltaTime) {
        
        constexpr float moveSpeed{10.0f};

        if(KeyBoard::IsKeyHold(KeyCode::KEY_LEFT)){
            m_position += this->GetLeft() * static_cast<float>(deltaTime) * moveSpeed;
        }
        else if(KeyBoard::IsKeyHold(KeyCode::KEY_RIGHT)){
            m_position += this->GetRight() * static_cast<float>(deltaTime) * moveSpeed;
        }
        else if(KeyBoard::IsKeyHold(KeyCode::KEY_UP)){
            m_position += this->GetForward() * static_cast<float>(deltaTime) * moveSpeed;
        }
        else if(KeyBoard::IsKeyHold(KeyCode::KEY_DOWN)){
            m_position += this->GetBackward() * static_cast<float>(deltaTime) * moveSpeed;
        }
        else if(KeyBoard::IsKeyHold(KeyCode::KEY_SPACE)){
            m_position += this->GetUp() * static_cast<float>(deltaTime) * moveSpeed;
        }
        else if(KeyBoard::IsKeyHold(KeyCode::KEY_Z)){
            m_position += this->GetDown() * static_cast<float>(deltaTime) * moveSpeed;
        }

        if(Mouse::IsMouseHold(MouseButton::RIGHT)){
            glfwSetInputMode(g_nativeWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            const auto deltaMouse = Mouse::GetDeltaMousePosition();
            m_rotation.x += deltaMouse.y * static_cast<float>(deltaTime);   // Pitch
            m_rotation.x = std::clamp(m_rotation.x, -90.0f, 90.0f);
            m_rotation.y += deltaMouse.x * static_cast<float>(deltaTime);   // Yaw
        }
        else{
            glfwSetInputMode(g_nativeWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }

        this->updateViewMatrix();
    }

    const glm::vec3& Camera::GetPosition() const
    {
        return m_position;
    }

    glm::vec3& Camera::GetPosition() {
        return m_position;
    }

    const glm::quat Camera::GetRotation() const
    {
        return glm::quat{m_rotation};
    }

    glm::vec3 Camera::GetUp() const {
        return glm::normalize(glm::rotate(GetRotation(), Camera::Up));
    }

    glm::vec3 Camera::GetDown() const {
        return glm::normalize(glm::rotate(GetRotation(), Camera::Down));
    }

    glm::vec3 Camera::GetRight() const {
        return glm::normalize(glm::rotate(GetRotation(), Camera::Right));
    }

    glm::vec3 Camera::GetLeft() const {
        return glm::normalize(glm::rotate(GetRotation(), Camera::Left));
    }

    glm::vec3 Camera::GetForward() const {
        return glm::normalize(glm::rotate(GetRotation(), Camera::Forward));
    }

    glm::vec3 Camera::GetBackward() const {
        return glm::normalize(glm::rotate(GetRotation(), Camera::BackWard));
    }

    const glm::mat4& Camera::GetViewMatrix() const {
        return m_viewMatrix;
    }

    const glm::mat4& Camera::GetProjectionMatrix() const {
        return m_projectionMatrix;
    }

    void Camera::SetFov(float fov) {
        m_fov = fov;

        this->updateProjectionMatrix();
    }

    void Camera::SetViewport(uint32_t width, uint32_t height) {
        m_aspectRatio = static_cast<float>(width) / static_cast<float>(height);

        this->updateProjectionMatrix();
    }

    void Camera::SetViewFrustum(float nearClip, float farClip) {
        m_nearClip = nearClip;
        m_farClip = farClip;
        
        this->updateProjectionMatrix();
    }

    void Camera::updateViewMatrix() {
        // Translate to view Coord
        m_viewMatrix = glm::inverse(glm::translate(glm::mat4{1.0f}, m_position) * glm::toMat4(this->GetRotation()));
    }

    void Camera::updateProjectionMatrix() {
        
        m_projectionMatrix = glm::perspectiveLH(glm::radians(m_fov), m_aspectRatio, m_nearClip, m_farClip);

        m_projectionMatrix[1][1] *= -1;
    }
}