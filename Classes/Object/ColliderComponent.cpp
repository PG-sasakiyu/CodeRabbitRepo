// ColliderComponent.cpp
#include "ColliderComponent.h"
#include "..\Object\GameObject.h"
#include "..\MainProject\External\imgui\imgui.h"
#include <algorithm>

ColliderComponent::ColliderComponent(GameObject* owner, ColliderType type)
    : Component(owner), m_colliderType(type)
{
}

void ColliderComponent::Update(float deltaTime)
{
    // 毎フレーム、親オブジェクトの移動に合わせてコライダーの位置を更新
    UpdateCollider();
}

void ColliderComponent::UpdateCollider()
{
    // 親のGameObjectの最新の座標・スケール・回転を使用する
    auto pos = m_owner->Position();
    auto scale = m_owner->Scale();
    auto rot = m_owner->m_qRotation;

    switch (m_colliderType)
    {
    case ColliderType::AABB:
        m_aabb.Center = pos;
        m_aabb.Extents = DirectX::XMFLOAT3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f);
        break;
    case ColliderType::OBB:
        m_obb.Center = pos;
        m_obb.Extents = DirectX::XMFLOAT3(scale.x * 0.5f, scale.y * 0.5f, scale.z * 0.5f);
        m_obb.Orientation = rot;
        break;
    case ColliderType::Sphere:
        m_sphere.Center = pos;
        m_sphere.Radius = std::max({ scale.x, scale.y, scale.z }) * 0.5f;
        break;
    case ColliderType::None:
    default:
        break;
    }
}

void ColliderComponent::Serialize(nlohmann::json& outJson) const
{
    outJson["ColliderComponent"]["type"] = static_cast<int>(m_colliderType);
}

void ColliderComponent::Deserialize(const nlohmann::json& inJson)
{
    if (inJson.contains("ColliderComponent")) {
        auto& compJson = inJson["ColliderComponent"];
        if (compJson.contains("type")) {
            m_colliderType = static_cast<ColliderType>(compJson["type"].get<int>());
        }
    }
}

void ColliderComponent::DrawInspectorUI()
{
    if (ImGui::TreeNodeEx("Collider", ImGuiTreeNodeFlags_DefaultOpen)) {
        const char* colliderNames[] = { "None", "AABB", "OBB", "Sphere" };
        int currentColliderInt = static_cast<int>(m_colliderType);

        if (ImGui::BeginCombo("Type", colliderNames[currentColliderInt])) {
            for (int i = 0; i < IM_ARRAYSIZE(colliderNames); i++) {
                const bool isSelected = (currentColliderInt == i);
                if (ImGui::Selectable(colliderNames[i], isSelected)) {
                    m_colliderType = static_cast<ColliderType>(i);
                    UpdateCollider();
                }
                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::TreePop();
    }
}