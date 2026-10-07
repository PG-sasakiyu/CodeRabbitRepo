// RigidbodyComponent.h
#pragma once
#include "Component.h"
#include <DirectXMath.h>
#include "SimpleMath.h"

class RigidbodyComponent : public Component {
public:
    RigidbodyComponent(GameObject* owner);
    ~RigidbodyComponent() override = default;

    void Serialize(nlohmann::json& outJson) const override;
    void Deserialize(const nlohmann::json& inJson) override;
    void DrawInspectorUI() override;

    DirectX::SimpleMath::Vector3& Velocity() { return m_velocity; }
    DirectX::SimpleMath::Vector3& AngularVelocity() { return m_angularVelocity; }

    bool m_isStatic = false;
    bool m_useGravity = true;
    bool m_isGrounded = false;

    float m_restitution = 0.2f;
    float m_friction = 0.8f;
    float m_linearDamping = 0.5f;
    float m_angularDamping = 1.0f;

    float m_sleepTimer = 0.0f;
    bool m_isSleeping = false;

private:
    DirectX::SimpleMath::Vector3 m_velocity = { 0.0f, 0.0f, 0.0f };
    DirectX::SimpleMath::Vector3 m_angularVelocity = { 0.0f, 0.0f, 0.0f };
};