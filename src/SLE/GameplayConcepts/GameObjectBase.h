#pragma once
#include "../System/VulkanIncludes.h"

#include "../Core/AssetData.h"

#include "GameObjectComponentBase.h"

#include "../Core/Transform.h"
#include "../Core/Delegate/DelegateInclude.h"

#include <unordered_map>



class GameObjectBase : public AssetData
{
public:
	GameObjectBase();
	virtual ~GameObjectBase() override;

	GameObjectBase(const GameObjectBase&) = delete;
	GameObjectBase& operator=(const GameObjectBase&) = delete;
	
	Transform* GetTransform() { return &m_Transform; }
	
	virtual void FirstUpdate() = 0;
	virtual void Update() = 0;
	virtual void LastUpdate() = 0;

	void AddComponent(GameObjectComponentBase* _Component);
	void RemoveComponent(GameObjectComponentBase* _Component);
	void ClearComponents();
	void UpdateComponents();
	void DrawGameObject(VulkanFrameInfo _FrameInfo);
	void AddDrawCallback(const std::function<void(VulkanFrameInfo)>& _Callback);
	void RemoveDrawCallback(const std::function<void(VulkanFrameInfo)>& _Callback);

protected:

	Transform m_Transform{};
	std::unordered_map<uint64_t, GameObjectComponentBase*> m_Components;

	DelegateMulticast<VulkanFrameInfo> m_DrawCallback;

	uint32_t m_UID;

	bool m_Enabled = true;
};