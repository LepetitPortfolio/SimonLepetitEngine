#include "CameraBase.h"

#include <cassert>
#include <limits>

void CameraBase::SetOrthographicProjection(float _Left, float _Right, float _Top, float _Bottom, float _Near, float _Far)
{
	m_ProjectionMatrix = glm::mat4{ 1.0f };
	m_ProjectionMatrix[0][0] = 2.f / (_Right - _Left);
	m_ProjectionMatrix[1][1] = 2.f / (_Bottom - _Top);
	m_ProjectionMatrix[2][2] = 1.f / (_Far - _Near);
	m_ProjectionMatrix[3][0] = -(_Right + _Left) / (_Right - _Left);
	m_ProjectionMatrix[3][1] = -(_Bottom + _Top) / (_Bottom - _Top);
	m_ProjectionMatrix[3][2] = -_Near / (_Far - _Near);
}

void CameraBase::SetPerspectiveProjection(float _Fovy, float _Aspect, float _Near, float _Far)
{
	assert(glm::abs(_Aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
	const float tanHalfFovy = tan(_Fovy / 2.f);
	m_ProjectionMatrix = glm::mat4{ 0.0f };
	m_ProjectionMatrix[0][0] = 1.f / (_Aspect * tanHalfFovy);
	m_ProjectionMatrix[1][1] = 1.f / (tanHalfFovy);
	m_ProjectionMatrix[2][2] = _Far / (_Far - _Near);
	m_ProjectionMatrix[2][3] = 1.f;
	m_ProjectionMatrix[3][2] = -(_Far * _Near) / (_Far - _Near);
}

void CameraBase::SetViewDirection(glm::vec3 _Position, glm::vec3 _Direction, glm::vec3 _Up)
{
	const glm::vec3 w{ glm::normalize(_Direction) };
	const glm::vec3 u{ glm::normalize(glm::cross(w, _Up)) };
	const glm::vec3 v{ glm::cross(w, u) };

	m_ViewMatrix = glm::mat4{ 1.f };
	m_ViewMatrix[0][0] = u.x;
	m_ViewMatrix[1][0] = u.y;
	m_ViewMatrix[2][0] = u.z;
	m_ViewMatrix[0][1] = v.x;
	m_ViewMatrix[1][1] = v.y;
	m_ViewMatrix[2][1] = v.z;
	m_ViewMatrix[0][2] = w.x;
	m_ViewMatrix[1][2] = w.y;
	m_ViewMatrix[2][2] = w.z;
	m_ViewMatrix[3][0] = -glm::dot(u, _Position);
	m_ViewMatrix[3][1] = -glm::dot(v, _Position);
	m_ViewMatrix[3][2] = -glm::dot(w, _Position);

	m_InverseViewMatrix = glm::mat4{ 1.f };
	m_InverseViewMatrix[0][0] = u.x;
	m_InverseViewMatrix[0][1] = u.y;
	m_InverseViewMatrix[0][2] = u.z;
	m_InverseViewMatrix[1][0] = v.x;
	m_InverseViewMatrix[1][1] = v.y;
	m_InverseViewMatrix[1][2] = v.z;
	m_InverseViewMatrix[2][0] = w.x;
	m_InverseViewMatrix[2][1] = w.y;
	m_InverseViewMatrix[2][2] = w.z;
	m_InverseViewMatrix[3][0] = _Position.x;
	m_InverseViewMatrix[3][1] = _Position.y;
	m_InverseViewMatrix[3][2] = _Position.z;
}

void CameraBase::SetViewTarget(glm::vec3 _Position, glm::vec3 _Target, glm::vec3 _Up)
{
	SetViewDirection(_Position, _Target - _Position, _Up);
}

void CameraBase::SetViewYXZ(glm::vec3 _Position, glm::vec3 _Rotation)
{
	const float c3 = glm::cos(_Rotation.z);
	const float s3 = glm::sin(_Rotation.z);
	const float c2 = glm::cos(_Rotation.x);
	const float s2 = glm::sin(_Rotation.x);
	const float c1 = glm::cos(_Rotation.y);
	const float s1 = glm::sin(_Rotation.y);
	const glm::vec3 u{ (c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1) };
	const glm::vec3 v{ (c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3) };
	const glm::vec3 w{ (c2 * s1), (-s2), (c1 * c2) };
	m_ViewMatrix = glm::mat4{ 1.f };
	m_ViewMatrix[0][0] = u.x;
	m_ViewMatrix[1][0] = u.y;
	m_ViewMatrix[2][0] = u.z;
	m_ViewMatrix[0][1] = v.x;
	m_ViewMatrix[1][1] = v.y;
	m_ViewMatrix[2][1] = v.z;
	m_ViewMatrix[0][2] = w.x;
	m_ViewMatrix[1][2] = w.y;
	m_ViewMatrix[2][2] = w.z;
	m_ViewMatrix[3][0] = -glm::dot(u, _Position);
	m_ViewMatrix[3][1] = -glm::dot(v, _Position);
	m_ViewMatrix[3][2] = -glm::dot(w, _Position);

	m_InverseViewMatrix = glm::mat4{ 1.f };
	m_InverseViewMatrix[0][0] = u.x;
	m_InverseViewMatrix[0][1] = u.y;
	m_InverseViewMatrix[0][2] = u.z;
	m_InverseViewMatrix[1][0] = v.x;
	m_InverseViewMatrix[1][1] = v.y;
	m_InverseViewMatrix[1][2] = v.z;
	m_InverseViewMatrix[2][0] = w.x;
	m_InverseViewMatrix[2][1] = w.y;
	m_InverseViewMatrix[2][2] = w.z;
	m_InverseViewMatrix[3][0] = _Position.x;
	m_InverseViewMatrix[3][1] = _Position.y;
	m_InverseViewMatrix[3][2] = _Position.z;
}



