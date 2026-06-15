#pragma once

class CameraBase
{
	public:
	virtual ~CameraBase() = default;
	virtual void Update(float DeltaTime) = 0;
	virtual glm::mat4 GetViewMatrix() const = 0;
	virtual glm::mat4 GetProjectionMatrix() const = 0;
};