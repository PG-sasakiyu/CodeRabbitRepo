// CollisionResolver.cpp
#include "CollisionResolver.h"
#include "PhysicsConstants.h"
#include "..\MainProject\Classes\Object\ColliderComponent.h"
#include "..\MainProject\Classes\Object\RigidbodyComponent.h"
#include <algorithm>
#include <cmath>

void CollisionResolver::ResolveCollisions(std::vector<CollisionManifold>& targetManifolds)
{
    for (int iteration = 0; iteration < PhysicsConstants::VelocitySolverIterations; ++iteration) {
        ResolveVelocityPhase(targetManifolds);
    }
    for (int iteration = 0; iteration < PhysicsConstants::PositionSolverIterations; ++iteration) {
        ResolvePositionPhase(targetManifolds);
    }
}

void CollisionResolver::ResolveVelocityPhase(std::vector<CollisionManifold>& targetManifolds)
{
    for (CollisionManifold& currentManifold : targetManifolds) {
        for (ContactInformation& currentContact : currentManifold.ContactPoints) {
            ApplyNormalImpulse(currentManifold, currentContact);
            ApplyFrictionImpulse(currentManifold, currentContact);
        }
    }
}

void CollisionResolver::ResolvePositionPhase(std::vector<CollisionManifold>& targetManifolds)
{
    for (CollisionManifold& currentManifold : targetManifolds) {
        for (ContactInformation& currentContact : currentManifold.ContactPoints) {
            ApplyPseudoImpulseForPosition(currentManifold, currentContact);
        }
    }
}

float CollisionResolver::GetInverseMass(GameObject* targetObject)
{
    auto rb = targetObject->GetComponent<RigidbodyComponent>();
    // Rigidbodyがない、または静的・スリープ中のオブジェクトは動かない（質量無限大）
    if (!rb || rb->m_isStatic || rb->m_isSleeping) return 0.0f;
    return 1.0f / PhysicsConstants::DefaultBaseMass;
}

float CollisionResolver::GetInverseInertia(GameObject* targetObject)
{
    auto rb = targetObject->GetComponent<RigidbodyComponent>();
    if (!rb || rb->m_isStatic || rb->m_isSleeping) return 0.0f;

    auto col = targetObject->GetComponent<ColliderComponent>();
    if (col && col->m_colliderType == ColliderType::AABB) return 0.0f;

    return 1.0f / PhysicsConstants::DefaultInertiaMoment;
}

DirectX::SimpleMath::Vector3 CollisionResolver::CalculateRadiusVector(GameObject* targetObject, const DirectX::SimpleMath::Vector3& contactPosition)
{
    return contactPosition - targetObject->Position();
}

DirectX::SimpleMath::Vector3 CollisionResolver::CalculateRelativeVelocity(GameObject* firstObject, GameObject* secondObject, const DirectX::SimpleMath::Vector3& radiusFirst, const DirectX::SimpleMath::Vector3& radiusSecond)
{
    auto rb1 = firstObject->GetComponent<RigidbodyComponent>();
    auto rb2 = secondObject->GetComponent<RigidbodyComponent>();

    DirectX::SimpleMath::Vector3 velocityFirst = rb1 ? (rb1->Velocity() + rb1->AngularVelocity().Cross(radiusFirst)) : DirectX::SimpleMath::Vector3::Zero;
    DirectX::SimpleMath::Vector3 velocitySecond = rb2 ? (rb2->Velocity() + rb2->AngularVelocity().Cross(radiusSecond)) : DirectX::SimpleMath::Vector3::Zero;

    return velocitySecond - velocityFirst;
}

void CollisionResolver::ApplyImpulseToObject(GameObject* targetObject, const DirectX::SimpleMath::Vector3& impulseVector, const DirectX::SimpleMath::Vector3& radiusVector)
{
    auto rb = targetObject->GetComponent<RigidbodyComponent>();
    if (!rb || rb->m_isStatic) return;

    if (impulseVector.LengthSquared() > PhysicsConstants::WakeUpImpulseThreshold * PhysicsConstants::WakeUpImpulseThreshold) {
        rb->m_isSleeping = false;
        rb->m_sleepTimer = 0.0f;
    }

    float inverseMass = GetInverseMass(targetObject);
    float inverseInertia = GetInverseInertia(targetObject);

    rb->Velocity() += impulseVector * inverseMass;
    rb->AngularVelocity() += radiusVector.Cross(impulseVector) * inverseInertia;
}

void CollisionResolver::ApplyNormalImpulse(const CollisionManifold& currentManifold, ContactInformation& currentContact)
{
    auto rb1 = currentManifold.FirstObject->GetComponent<RigidbodyComponent>();
    auto rb2 = currentManifold.SecondObject->GetComponent<RigidbodyComponent>();

    if (rb1 && currentManifold.CollisionNormal.y < -PhysicsConstants::GroundNormalThreshold) {
        rb1->m_isGrounded = true;
    }
    if (rb2 && currentManifold.CollisionNormal.y > PhysicsConstants::GroundNormalThreshold) {
        rb2->m_isGrounded = true;
    }

    DirectX::SimpleMath::Vector3 radiusFirst = CalculateRadiusVector(currentManifold.FirstObject, currentContact.Position);
    DirectX::SimpleMath::Vector3 radiusSecond = CalculateRadiusVector(currentManifold.SecondObject, currentContact.Position);

    DirectX::SimpleMath::Vector3 relativeVelocity = CalculateRelativeVelocity(currentManifold.FirstObject, currentManifold.SecondObject, radiusFirst, radiusSecond);
    float velocityAlongNormal = relativeVelocity.Dot(currentManifold.CollisionNormal);

    if (velocityAlongNormal > -PhysicsConstants::VelocitySlop) return;

    float combinedRestitution = currentManifold.CombinedRestitution;
    if (std::abs(velocityAlongNormal) < PhysicsConstants::RestingVelocityThreshold) {
        combinedRestitution = 0.0f;
    }

    float rotationalEffectiveMassFirst = radiusFirst.Cross(currentManifold.CollisionNormal).LengthSquared() * GetInverseInertia(currentManifold.FirstObject);
    float rotationalEffectiveMassSecond = radiusSecond.Cross(currentManifold.CollisionNormal).LengthSquared() * GetInverseInertia(currentManifold.SecondObject);
    float totalEffectiveMass = GetInverseMass(currentManifold.FirstObject) + GetInverseMass(currentManifold.SecondObject) + rotationalEffectiveMassFirst + rotationalEffectiveMassSecond;

    if (totalEffectiveMass == 0.0f) return;

    float impulseMagnitude = -(1.0f + combinedRestitution) * velocityAlongNormal / totalEffectiveMass;

    float previousNormalImpulse = currentContact.AccumulatedNormalImpulse;
    currentContact.AccumulatedNormalImpulse = std::max(previousNormalImpulse + impulseMagnitude, 0.0f);
    float deltaImpulse = currentContact.AccumulatedNormalImpulse - previousNormalImpulse;

    DirectX::SimpleMath::Vector3 impulseVector = currentManifold.CollisionNormal * deltaImpulse;

    ApplyImpulseToObject(currentManifold.FirstObject, -impulseVector, radiusFirst);
    ApplyImpulseToObject(currentManifold.SecondObject, impulseVector, radiusSecond);
}

void CollisionResolver::ApplyFrictionImpulse(const CollisionManifold& currentManifold, ContactInformation& currentContact)
{
    DirectX::SimpleMath::Vector3 radiusFirst = CalculateRadiusVector(currentManifold.FirstObject, currentContact.Position);
    DirectX::SimpleMath::Vector3 radiusSecond = CalculateRadiusVector(currentManifold.SecondObject, currentContact.Position);

    DirectX::SimpleMath::Vector3 relativeVelocity = CalculateRelativeVelocity(currentManifold.FirstObject, currentManifold.SecondObject, radiusFirst, radiusSecond);
    DirectX::SimpleMath::Vector3 tangentDirection = relativeVelocity - currentManifold.CollisionNormal * relativeVelocity.Dot(currentManifold.CollisionNormal);
    float slipSpeed = tangentDirection.Length();

    if (slipSpeed <= PhysicsConstants::VelocitySlop) return;
    tangentDirection.Normalize();

    float rotationalTangentFirst = radiusFirst.Cross(tangentDirection).LengthSquared() * GetInverseInertia(currentManifold.FirstObject);
    float rotationalTangentSecond = radiusSecond.Cross(tangentDirection).LengthSquared() * GetInverseInertia(currentManifold.SecondObject);
    float tangentEffectiveMass = GetInverseMass(currentManifold.FirstObject) + GetInverseMass(currentManifold.SecondObject) + rotationalTangentFirst + rotationalTangentSecond;

    if (tangentEffectiveMass == 0.0f) return;

    float tangentImpulseMagnitude = -slipSpeed / tangentEffectiveMass;

    float maxFriction = currentManifold.CombinedFriction * currentContact.AccumulatedNormalImpulse;
    float previousTangentImpulse = currentContact.AccumulatedTangentImpulse;
    currentContact.AccumulatedTangentImpulse = std::clamp(previousTangentImpulse + tangentImpulseMagnitude, -maxFriction, maxFriction);
    float deltaTangentImpulse = currentContact.AccumulatedTangentImpulse - previousTangentImpulse;

    DirectX::SimpleMath::Vector3 frictionImpulseVector = tangentDirection * deltaTangentImpulse;

    ApplyImpulseToObject(currentManifold.FirstObject, -frictionImpulseVector, radiusFirst);
    ApplyImpulseToObject(currentManifold.SecondObject, frictionImpulseVector, radiusSecond);
}

void CollisionResolver::ApplyPseudoImpulseForPosition(const CollisionManifold& currentManifold, const ContactInformation& currentContact)
{
    auto rb1 = currentManifold.FirstObject->GetComponent<RigidbodyComponent>();
    auto rb2 = currentManifold.SecondObject->GetComponent<RigidbodyComponent>();

    bool isFirstFixed = !rb1 || rb1->m_isStatic || rb1->m_isSleeping;
    bool isSecondFixed = !rb2 || rb2->m_isStatic || rb2->m_isSleeping;
    if (isFirstFixed && isSecondFixed) return;

    float depthToCorrect = std::max(currentContact.PenetrationDepth - PhysicsConstants::AllowedPenetrationDepth, 0.0f);
    if (depthToCorrect <= 0.0f) return;

    DirectX::SimpleMath::Vector3 radiusFirst = CalculateRadiusVector(currentManifold.FirstObject, currentContact.Position);
    DirectX::SimpleMath::Vector3 radiusSecond = CalculateRadiusVector(currentManifold.SecondObject, currentContact.Position);

    float rotationalEffectiveMassFirst = radiusFirst.Cross(currentManifold.CollisionNormal).LengthSquared() * GetInverseInertia(currentManifold.FirstObject);
    float rotationalEffectiveMassSecond = radiusSecond.Cross(currentManifold.CollisionNormal).LengthSquared() * GetInverseInertia(currentManifold.SecondObject);
    float totalEffectiveMass = GetInverseMass(currentManifold.FirstObject) + GetInverseMass(currentManifold.SecondObject) + rotationalEffectiveMassFirst + rotationalEffectiveMassSecond;

    if (totalEffectiveMass == 0.0f) return;

    float positionImpulseMagnitude = (depthToCorrect / totalEffectiveMass) * PhysicsConstants::PenetrationCorrectionRatio;
    DirectX::SimpleMath::Vector3 correctionVector = currentManifold.CollisionNormal * positionImpulseMagnitude;

    ApplyPositionalMovement(currentManifold.FirstObject, -correctionVector, GetInverseMass(currentManifold.FirstObject));
    ApplyPositionalMovement(currentManifold.SecondObject, correctionVector, GetInverseMass(currentManifold.SecondObject));

    ApplyRotationalMovement(currentManifold.FirstObject, radiusFirst.Cross(-correctionVector), GetInverseInertia(currentManifold.FirstObject));
    ApplyRotationalMovement(currentManifold.SecondObject, radiusSecond.Cross(correctionVector), GetInverseInertia(currentManifold.SecondObject));

    // 位置が修正されたらコライダーを追従させる
    auto col1 = currentManifold.FirstObject->GetComponent<ColliderComponent>();
    auto col2 = currentManifold.SecondObject->GetComponent<ColliderComponent>();
    if (col1) col1->UpdateCollider();
    if (col2) col2->UpdateCollider();
}

void CollisionResolver::ApplyPositionalMovement(GameObject* targetObject, const DirectX::SimpleMath::Vector3& movementVector, float inverseMass)
{
    auto rb = targetObject->GetComponent<RigidbodyComponent>();
    if (!rb || rb->m_isStatic || rb->m_isSleeping) return;
    targetObject->Position() += movementVector * inverseMass;
}

void CollisionResolver::ApplyRotationalMovement(GameObject* targetObject, const DirectX::SimpleMath::Vector3& rotationAxisAndAngle, float inverseInertia)
{
    auto rb = targetObject->GetComponent<RigidbodyComponent>();
    if (!rb || rb->m_isStatic || rb->m_isSleeping) return;

    DirectX::SimpleMath::Vector3 rotationVector = rotationAxisAndAngle * inverseInertia;
    float rotationAngle = rotationVector.Length();

    if (rotationAngle > PhysicsConstants::SmallEpsilonValue) {
        DirectX::SimpleMath::Vector3 normalizedAxis = rotationVector / rotationAngle;

        DirectX::SimpleMath::Quaternion deltaRotation = DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(normalizedAxis, rotationAngle);
        targetObject->m_qRotation = targetObject->m_qRotation * deltaRotation;
        targetObject->m_qRotation.Normalize();
    }
}