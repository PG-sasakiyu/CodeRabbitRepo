// RigidbodyComponent.cpp
#include "RigidbodyComponent.h"
#include "..\MainProject\External\imgui\imgui.h"

RigidbodyComponent::RigidbodyComponent(GameObject* owner)
    : Component(owner)
{
}

void RigidbodyComponent::Serialize(nlohmann::json& outJson) const
{
    auto& compJson = outJson["RigidbodyComponent"];
    compJson["isStatic"] = m_isStatic;
    compJson["useGravity"] = m_useGravity;
    compJson["restitution"] = m_restitution;
    compJson["friction"] = m_friction;
    compJson["linearDamping"] = m_linearDamping;
    compJson["angularDamping"] = m_angularDamping;

    compJson["velocity"] = { m_velocity.x, m_velocity.y, m_velocity.z };
    compJson["angularVelocity"] = { m_angularVelocity.x, m_angularVelocity.y, m_angularVelocity.z };
}

void RigidbodyComponent::Deserialize(const nlohmann::json& inJson)
{
    if (inJson.contains("RigidbodyComponent")) {
        auto& compJson = inJson["RigidbodyComponent"];

        if (compJson.contains("isStatic")) m_isStatic = compJson["isStatic"];
        if (compJson.contains("useGravity")) m_useGravity = compJson["useGravity"];
        if (compJson.contains("restitution")) m_restitution = compJson["restitution"];
        if (compJson.contains("friction")) m_friction = compJson["friction"];
        if (compJson.contains("linearDamping")) m_linearDamping = compJson["linearDamping"];
        if (compJson.contains("angularDamping")) m_angularDamping = compJson["angularDamping"];

        if (compJson.contains("velocity")) {
            m_velocity.x = compJson["velocity"][0];
            m_velocity.y = compJson["velocity"][1];
            m_velocity.z = compJson["velocity"][2];
        }
        if (compJson.contains("angularVelocity")) {
            m_angularVelocity.x = compJson["angularVelocity"][0];
            m_angularVelocity.y = compJson["angularVelocity"][1];
            m_angularVelocity.z = compJson["angularVelocity"][2];
        }
    }
}

void RigidbodyComponent::DrawInspectorUI()
{
    if (ImGui::TreeNodeEx("Rigidbody", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Checkbox("Is Static", &m_isStatic);
        ImGui::Checkbox("Use Gravity", &m_useGravity);

        ImGui::DragFloat("Restitution (Bounciness)", &m_restitution, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Friction", &m_friction, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Linear Damping", &m_linearDamping, 0.01f, 0.0f, 1.0f);
        ImGui::DragFloat("Angular Damping", &m_angularDamping, 0.01f, 0.0f, 1.0f);

        ImGui::DragFloat3("Velocity", &m_velocity.x, 0.05f);
        ImGui::DragFloat3("Angular Velocity", &m_angularVelocity.x, 0.05f);

        ImGui::TreePop();
    }
}