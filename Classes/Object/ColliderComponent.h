// ColliderComponent.h
#pragma once
#include "Component.h"
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include "SimpleMath.h"

// コライダーの種類を定義（GameObject.hにあったものをこちらへ移動）
enum class ColliderType {
    None, AABB, OBB, Sphere
};

class ColliderComponent : public Component {
public:
    ColliderComponent(GameObject* owner, ColliderType type = ColliderType::AABB);
    ~ColliderComponent() override = default;

    void Update(float deltaTime) override;
    void Serialize(nlohmann::json& outJson) const override;
    void Deserialize(const nlohmann::json& inJson) override;
    void DrawInspectorUI() override;

    void UpdateCollider();

    ColliderType m_colliderType;
    DirectX::BoundingBox         m_aabb;
    DirectX::BoundingOrientedBox m_obb;
    DirectX::BoundingSphere      m_sphere;
};