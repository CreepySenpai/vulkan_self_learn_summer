#pragma once

#include <glm/glm.hpp>

namespace Creepy {

    class Camera{
        public:
            Camera() = default;
            Camera(float fov, float aspectRatio, float nearClip, float farClip);
            
            void OnUpdate(double deltaTime);

            const glm::vec3& GetPosition() const;
            glm::vec3& GetPosition();
            const glm::quat GetRotation() const;

            glm::vec3 GetUp() const;
            glm::vec3 GetDown() const;
            glm::vec3 GetRight() const;
            glm::vec3 GetLeft() const;
            glm::vec3 GetForward() const;
            glm::vec3 GetBackward() const;

            const glm::mat4& GetViewMatrix() const;
            const glm::mat4& GetProjectionMatrix() const;

            void SetFov(float fov);
            void SetViewport(uint32_t width, uint32_t height);
            void SetViewFrustum(float nearClip, float farClip);
        private:
            void updateViewMatrix();
            void updateProjectionMatrix();
        public:
            static inline const glm::vec3 Up{0.0f, 1.0f, 0.0f};
            static inline const glm::vec3 Down{0.0f, -1.0f, 0.0f};
            static inline const glm::vec3 Forward{0.0f, 0.0f, 1.0f};
            static inline const glm::vec3 BackWard{0.0f, 0.0f, -1.0f};
            static inline const glm::vec3 Right{1.0f, 0.0f, 0.0f};
            static inline const glm::vec3 Left{-1.0f, 0.0f, 0.0f};

        private:
            float m_fov{}, m_aspectRatio{}, m_nearClip{}, m_farClip{};
            glm::vec3 m_position{0.0f, 0.0f, -5.0f};
            // X = Pitch, Y = Yaw, Z = Roll
            glm::vec3 m_rotation{};
            glm::mat4 m_viewMatrix{1.0f};
            glm::mat4 m_projectionMatrix{1.0f};
        
    };

}