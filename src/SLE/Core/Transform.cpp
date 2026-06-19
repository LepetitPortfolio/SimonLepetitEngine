#include "Transform.h"

Transform::Transform()
{
}

Transform::~Transform()
{
}

const glm::vec3 Transform::GetPosition()
{
	return m_Position;
}

const glm::vec3 Transform::GetRotation()
{
	return m_Rotation;
}

const glm::vec3 Transform::GetScale()
{
	return m_Scale;
}

void Transform::SetPosition(glm::vec3 _Position)
{
    m_Position = _Position;
}

void Transform::SetRotation(glm::vec3 _Rotation)
{
    m_Rotation = _Rotation;
}

void Transform::SetScale(glm::vec3 _Scale)
{
    m_Scale = _Scale;
}

glm::mat4 Transform::TransformMatrix()
{
    const float c3 = glm::cos(m_Rotation.z);
    const float s3 = glm::sin(m_Rotation.z);
    const float c2 = glm::cos(m_Rotation.x);
    const float s2 = glm::sin(m_Rotation.x);
    const float c1 = glm::cos(m_Rotation.y);
    const float s1 = glm::sin(m_Rotation.y);
    return glm::mat4
    {
        {
            m_Scale.x * (c1 * c3 + s1 * s2 * s3),
            m_Scale.x * (c2 * s3),
            m_Scale.x * (c1 * s2 * s3 - c3 * s1),
            0.0f,
        },
        {
            m_Scale.y * (c3 * s1 * s2 - c1 * s3),
            m_Scale.y * (c2 * c3),
            m_Scale.y * (c1 * c3 * s2 + s1 * s3),
            0.0f,
        },
        {
            m_Scale.z * (c2 * s1),
            m_Scale.z * (-s2),
            m_Scale.z * (c1 * c2),
            0.0f,
        },
        {m_Position.x, m_Position.y, m_Position.z, 1.0f}
    };
}

glm::mat3 Transform::TransformNormalMatrix()
{
    const float c3 = glm::cos(m_Rotation.z);
    const float s3 = glm::sin(m_Rotation.z);
    const float c2 = glm::cos(m_Rotation.x);
    const float s2 = glm::sin(m_Rotation.x);
    const float c1 = glm::cos(m_Rotation.y);
    const float s1 = glm::sin(m_Rotation.y);
    const glm::vec3 invScale = 1.0f / m_Scale;

    return glm::mat3
    {
        {
            invScale.x * (c1 * c3 + s1 * s2 * s3),
            invScale.x * (c2 * s3),
            invScale.x * (c1 * s2 * s3 - c3 * s1),
        },
        {
            invScale.y * (c3 * s1 * s2 - c1 * s3),
            invScale.y * (c2 * c3),
            invScale.y * (c1 * c3 * s2 + s1 * s3),
        },
        {
            invScale.z * (c2 * s1),
            invScale.z * (-s2),
            invScale.z * (c1 * c2),
        },
    };
}
