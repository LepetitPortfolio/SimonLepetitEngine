#pragma once
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


class CameraBase
{
public:
    void SetOrthographicProjection(float _Left, float _Right, float _Top, float _Bottom, float _Near, float _Far);
    void SetPerspectiveProjection(float _Fovy, float _Aspect, float _Near, float _Far);

    void SetViewDirection(glm::vec3 _Position, glm::vec3 _Direction, glm::vec3 _Up = glm::vec3{ 0.f, -1.f, 0.f });
    void SetViewTarget( glm::vec3 _Position, glm::vec3 _Target, glm::vec3 _Up = glm::vec3{ 0.f, -1.f, 0.f });
    void SetViewYXZ(glm::vec3 _Position, glm::vec3 _Rotation);

    const glm::mat4& getProjection() const { return m_ProjectionMatrix; }
    const glm::mat4& getView() const { return m_ViewMatrix; }
    const glm::mat4& getInverseView() const { return m_InverseViewMatrix; }
    const glm::vec3 getPosition() const { return glm::vec3(m_InverseViewMatrix[3]); }

private:
    glm::mat4 m_ProjectionMatrix{ 1.f };
    glm::mat4 m_ViewMatrix{ 1.f };
    glm::mat4 m_InverseViewMatrix{ 1.f };
};