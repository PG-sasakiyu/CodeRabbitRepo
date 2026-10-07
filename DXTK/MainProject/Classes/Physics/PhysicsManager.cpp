// PhysicsManager.cpp
#include "PhysicsManager.h"
#include "PhysicsConstants.h"
#include "..\MainProject\Classes\Object\RigidbodyComponent.h"
#include "..\MainProject\Classes\Object\ColliderComponent.h"
#include <cmath>
#include <algorithm>

void PhysicsManager::UpdateForces(StageManager& stageManager, float deltaTime)
{
    if (deltaTime > PhysicsConstants::MaxDeltaTime) {
        deltaTime = PhysicsConstants::FixedDeltaTime;
    }

    const std::vector<std::shared_ptr<GameObject>>& targetObjects = stageManager.GetObjects();

    for (const std::shared_ptr<GameObject>& objectPointer : targetObjects) {
        GameObject* targetObject = objectPointer.get();
        auto rb = targetObject->GetComponent<RigidbodyComponent>();

        // Rigidbody を持っていない、または静的・スリープ中の場合はスキップ
        if (!rb || rb->m_isStatic || rb->m_isSleeping) {
            auto col = targetObject->GetComponent<ColliderComponent>();
            if (col) col->UpdateCollider();
            continue;
        }

        ApplyGravity(targetObject, deltaTime);
        ApplyDamping(targetObject, deltaTime);
    }
}

void PhysicsManager::UpdateVelocitiesAndPositions(StageManager& stageManager, float deltaTime)
{
    if (deltaTime > PhysicsConstants::MaxDeltaTime) {
        deltaTime = PhysicsConstants::FixedDeltaTime;
    }

    const std::vector<std::shared_ptr<GameObject>>& targetObjects = stageManager.GetObjects();

    for (const std::shared_ptr<GameObject>& objectPointer : targetObjects) {
        GameObject* targetObject = objectPointer.get();
        auto rb = targetObject->GetComponent<RigidbodyComponent>();

        // Rigidbody が無い、または静的オブジェクトは更新不要
        if (!rb || rb->m_isStatic) continue;

        EvaluateSleepState(targetObject, deltaTime);

        if (rb->m_isSleeping) continue;

        IntegrateVelocity(targetObject, deltaTime);
        IntegrateRotation(targetObject, deltaTime);
        UpdateEulerAngles(targetObject);

        // 位置の更新が終わったらコライダーを追従させる
        auto col = targetObject->GetComponent<ColliderComponent>();
        if (col) col->UpdateCollider();
    }
}

void PhysicsManager::ApplyGravity(GameObject* targetObject, float deltaTime)
{
    auto rb = targetObject->GetComponent<RigidbodyComponent>();
    if (rb && rb->m_useGravity) {
        rb->Velocity().y -= PhysicsConstants::GravityAcceleration * deltaTime;
    }
}

void PhysicsManager::ApplyDamping(GameObject* targetObject, float deltaTime)
{
    auto rb = targetObject->GetComponent<RigidbodyComponent>();
    if (rb) {
        rb->Velocity() -= rb->Velocity() * rb->m_linearDamping * deltaTime;
        rb->AngularVelocity() -= rb->AngularVelocity() * rb->m_angularDamping * deltaTime;
    }
}

void PhysicsManager::IntegrateVelocity(GameObject* targetObject, float deltaTime)
{
    auto rb = targetObject->GetComponent<RigidbodyComponent>();
    if (rb) {
        targetObject->Position() += rb->Velocity() * deltaTime;
    }
}

void PhysicsManager::IntegrateRotation(GameObject* targetObject, float deltaTime)
{
    auto col = targetObject->GetComponent<ColliderComponent>();
    // AABBは回転しない
    if (col && col->m_colliderType == ColliderType::AABB) {
        return; 
    }

    auto rb = targetObject->GetComponent<RigidbodyComponent>();
    if (!rb) return;

    DirectX::SimpleMath::Vector3 angularVelocityVector = rb->AngularVelocity();
    float rotationAngle = angularVelocityVector.Length() * deltaTime;

    if (rotationAngle <= 0.000001f) return;

    DirectX::SimpleMath::Vector3 rotationAxis = angularVelocityVector;
    rotationAxis.Normalize();

    DirectX::SimpleMath::Quaternion deltaQuaternion = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(rotationAxis, rotationAngle);
    targetObject->m_qRotation = targetObject->m_qRotation * deltaQuaternion;
    targetObject->m_qRotation.Normalize();
}

void PhysicsManager::UpdateEulerAngles(GameObject* targetObject)
{
    float componentX = targetObject->m_qRotation.x;
    float componentY = targetObject->m_qRotation.y;
    float componentZ = targetObject->m_qRotation.z;
    float componentW = targetObject->m_qRotation.w;

    float sinePitch = 2.0f * (componentW * componentX - componentY * componentZ);
    if (std::abs(sinePitch) >= 1.0f) {
        targetObject->Rotation().x = std::copysign(DirectX::XM_PI / 2.0f, sinePitch);
    }
    else {
        targetObject->Rotation().x = std::asin(sinePitch);
    }

    float sineYaw = 2.0f * (componentW * componentY + componentZ * componentX);
    float cosineYaw = 1.0f - 2.0f * (componentX * componentX + componentY * componentY);
    targetObject->Rotation().y = std::atan2(sineYaw, cosineYaw);

    float sineRoll = 2.0f * (componentW * componentZ + componentX * componentY);
    float cosineRoll = 1.0f - 2.0f * (componentX * componentX + componentZ * componentZ);
    targetObject->Rotation().z = std::atan2(sineRoll, cosineRoll);
}

void PhysicsManager::EvaluateSleepState(GameObject* targetObject, float deltaTime)
{
    auto rb = targetObject->GetComponent<RigidbodyComponent>();
    auto col = targetObject->GetComponent<ColliderComponent>();
    if (!rb) return;

    if (col && col->m_colliderType == ColliderType::AABB) {
        rb->AngularVelocity() = DirectX::SimpleMath::Vector3::Zero;
    }

    if (!rb->m_isGrounded) {
        rb->m_sleepTimer = 0.0f;
        rb->m_isSleeping = false;
        return;
    }

    float currentSpeed = rb->Velocity().Length();
    float currentAngularSpeed = rb->AngularVelocity().Length();

    if (currentSpeed < PhysicsConstants::LinearVelocityCutoff && currentAngularSpeed < PhysicsConstants::AngularVelocityCutoff) {
        rb->m_sleepTimer += deltaTime;

        float sleepProgress = rb->m_sleepTimer / PhysicsConstants::SleepWaitTime;
        if (sleepProgress > PhysicsConstants::SleepDampingStartRatio) {
            rb->Velocity() *= PhysicsConstants::SleepDampingFactor;
            rb->AngularVelocity() *= PhysicsConstants::SleepDampingFactor;
        }

        if (rb->m_sleepTimer >= PhysicsConstants::SleepWaitTime) {
            rb->m_isSleeping = true;
            rb->Velocity() = DirectX::SimpleMath::Vector3::Zero;
            rb->AngularVelocity() = DirectX::SimpleMath::Vector3::Zero;

            CorrectRotationToRightAngles(targetObject);
            if (col) col->UpdateCollider();
        }
    }
    else {
        if (currentSpeed > PhysicsConstants::LinearVelocityCutoff * PhysicsConstants::WakeUpVelocityMultiplier ||
            currentAngularSpeed > PhysicsConstants::AngularVelocityCutoff * PhysicsConstants::WakeUpVelocityMultiplier) {
            rb->m_sleepTimer = 0.0f;
            rb->m_isSleeping = false;
        }
        else {
            rb->m_sleepTimer -= deltaTime;
            if (rb->m_sleepTimer < 0.0f) rb->m_sleepTimer = 0.0f;
        }
    }
}

void PhysicsManager::CorrectRotationToRightAngles(GameObject* targetObject)
{
    DirectX::SimpleMath::Vector3 currentEulerAngles = targetObject->Rotation();
    bool isCorrectionRequired = false;
    float rightAngleRadian = DirectX::XM_PI / 2.0f;

    float roundedAngleX = std::round(currentEulerAngles.x / rightAngleRadian) * rightAngleRadian;
    if (std::abs(currentEulerAngles.x - roundedAngleX) < PhysicsConstants::AngleCorrectionTolerance) {
        currentEulerAngles.x = roundedAngleX;
        isCorrectionRequired = true;
    }

    float roundedAngleY = std::round(currentEulerAngles.y / rightAngleRadian) * rightAngleRadian;
    if (std::abs(currentEulerAngles.y - roundedAngleY) < PhysicsConstants::AngleCorrectionTolerance) {
        currentEulerAngles.y = roundedAngleY;
        isCorrectionRequired = true;
    }

    float roundedAngleZ = std::round(currentEulerAngles.z / rightAngleRadian) * rightAngleRadian;
    if (std::abs(currentEulerAngles.z - roundedAngleZ) < PhysicsConstants::AngleCorrectionTolerance) {
        currentEulerAngles.z = roundedAngleZ;
        isCorrectionRequired = true;
    }

    if (isCorrectionRequired) {
        targetObject->Rotation() = currentEulerAngles;
        targetObject->m_qRotation = DirectX::SimpleMath::Quaternion::CreateFromYawPitchRoll(currentEulerAngles.y, currentEulerAngles.x, currentEulerAngles.z);
    }
}