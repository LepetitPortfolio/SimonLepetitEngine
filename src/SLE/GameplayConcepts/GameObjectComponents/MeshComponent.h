#pragma once
#include "LocationComponent.h"
#include "../../Graphics/Mesh.h"

class MeshComponent : public LocationComponent
{
public:
	MeshComponent(Mesh* _Mesh, Transform _LocalTransform = Transform{});
	~MeshComponent();

	Mesh* GetMesh() { return m_Mesh; }
	void SetMesh(Mesh* _Mesh);

	virtual void Init(GameObjectBase* _GameObjectParent) override;
	virtual void Desinit() override;

	void Draw(VulkanFrameInfo& _FrameInfo);

protected:
	Mesh* m_Mesh;

	virtual void EnabledAction() override;
	virtual void DisableAction() override;

};