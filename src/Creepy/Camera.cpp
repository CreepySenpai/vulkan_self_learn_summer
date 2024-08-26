#include <Creepy/Camera.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>


namespace Creepy {

    Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
        : m_fov{fov}, m_aspectRatio{aspectRatio}, m_nearClip{nearClip}, m_farClip{farClip}
    {

    }

    void Camera::Update(double deltaTime) {

    }

    const glm::vec3& Camera::GetPosition() const
    {
        return m_position;
    }

    const glm::quat Camera::GetRotation() const
    {
        return glm::quat{m_rotation};
    }

    glm::vec3 Camera::GetUp() const {
        return glm::rotate(GetRotation(), Camera::Up);
    }

    glm::vec3 Camera::GetRight() const {
        return glm::rotate(GetRotation(), Camera::Right);
    }

    glm::vec3 Camera::GetLeft() const {
        return glm::rotate(GetRotation(), Camera::Left);
    }

    glm::vec3 Camera::GetForward() const {
        return glm::rotate(GetRotation(), Camera::Forward);
    }

    glm::vec3 Camera::GetBackward() const {
        return glm::rotate(GetRotation(), Camera::BackWard);
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
        m_viewMatrix = glm::lookAt(m_position, m_position, this->GetUp());
    }

    void Camera::updateProjectionMatrix() {
        m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_nearClip, m_farClip);
    }
}