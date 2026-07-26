#pragma once
#define GLFORCE_RADIANS
#include <glm/glm.hpp>

class Transform
{
public:
	Transform();
	~Transform();

	glm::vec3 GetPosition();
	glm::vec3 GetRotation();
	glm::vec3 GetScale();

	void SetPosition(glm::vec3 _Position);
	void SetRotation(glm::vec3 _Rotation);
	void SetScale(glm::vec3 _Scale);

	glm::mat4 TransformMatrix();

	glm::mat3 TransformNormalMatrix();

	Transform operator+(Transform& _Other);
	Transform operator+(Transform* _Other);

	void operator+=(Transform& _Other);
	void operator+=(Transform* _Other);

	Transform operator-(Transform& _Other);
	Transform operator-(Transform* _Other);

	void operator-=(Transform& _Other);
	void operator-=(Transform* _Other);
	

private:
	glm::vec3 m_Position{};
	glm::vec3 m_Rotation{};
	glm::vec3 m_Scale{ 1.f, 1.f, 1.f };
};