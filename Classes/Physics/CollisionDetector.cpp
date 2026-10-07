// CollisionDetector.cpp
#include "CollisionDetector.h"
#include "PhysicsConstants.h"
#include "..\MainProject\Classes\Object\ColliderComponent.h"
#include "..\MainProject\Classes\Object\RigidbodyComponent.h"
#include <cmath>
#include <algorithm>
#include <cfloat>

std::vector<CollisionManifold> CollisionDetector::DetectCollisions(const std::vector<std::shared_ptr<GameObject>>& targetObjects)
{
    std::vector<CollisionManifold> detectedManifolds;
    size_t totalObjectCount = targetObjects.size();

    for (size_t indexFirst = 0; indexFirst < totalObjectCount; ++indexFirst) {
        for (size_t indexSecond = indexFirst + 1; indexSecond < totalObjectCount; ++indexSecond) {
            GameObject* firstObject = targetObjects[indexFirst].get();
            GameObject* secondObject = targetObjects[indexSecond].get();

            // どちらかのオブジェクトがコライダーを持っていなければ判定をスキップ
            auto col1 = firstObject->GetComponent<ColliderComponent>();
            auto col2 = secondObject->GetComponent<ColliderComponent>();
            if (!col1 || !col2 || col1->m_colliderType == ColliderType::None || col2->m_colliderType == ColliderType::None) {
                continue;
            }

            EvaluatePairForCollision(firstObject, secondObject, detectedManifolds);
        }
    }

    return detectedManifolds;
}

void CollisionDetector::EvaluatePairForCollision(GameObject* firstObject, GameObject* secondObject, std::vector<CollisionManifold>& outManifolds)
{
    CollisionManifold currentManifold;
    bool isColliding = CheckOrientedBoundingBoxCollision(firstObject, secondObject, currentManifold);

    if (isColliding) {
        outManifolds.push_back(currentManifold);
    }
}

DirectX::BoundingOrientedBox CollisionDetector::GetAsOrientedBoundingBox(GameObject* targetObject)
{
    auto col = targetObject->GetComponent<ColliderComponent>();

    // オブジェクトがAABBの場合は、OBBの形式に変換して返す
    if (col->m_colliderType == ColliderType::AABB) {
        DirectX::BoundingOrientedBox convertedBox;
        convertedBox.Center = col->m_aabb.Center;
        convertedBox.Extents = col->m_aabb.Extents;
        convertedBox.Orientation = DirectX::SimpleMath::Quaternion::Identity;
        return convertedBox;
    }

    return col->m_obb;
}

void CollisionDetector::ExtractLocalAxes(const DirectX::BoundingOrientedBox& targetBox, DirectX::SimpleMath::Vector3 outAxes[PhysicsConstants::Dimensions])
{
    DirectX::SimpleMath::Matrix rotationMatrix = DirectX::SimpleMath::Matrix::CreateFromQuaternion(targetBox.Orientation);
    outAxes[0] = rotationMatrix.Right();
    outAxes[1] = rotationMatrix.Up();
    outAxes[2] = rotationMatrix.Backward();
}

bool CollisionDetector::CheckOrientedBoundingBoxCollision(GameObject* firstObject, GameObject* secondObject, CollisionManifold& outManifold)
{
    DirectX::BoundingOrientedBox boxFirst = GetAsOrientedBoundingBox(firstObject);
    DirectX::BoundingOrientedBox boxSecond = GetAsOrientedBoundingBox(secondObject);

    DirectX::SimpleMath::Vector3 axesFirst[PhysicsConstants::Dimensions];
    DirectX::SimpleMath::Vector3 axesSecond[PhysicsConstants::Dimensions];
    ExtractLocalAxes(boxFirst, axesFirst);
    ExtractLocalAxes(boxSecond, axesSecond);

    float minimumPenetration = FLT_MAX;
    DirectX::SimpleMath::Vector3 collisionNormal;

    for (int index = 0; index < PhysicsConstants::Dimensions; ++index) {
        if (!TestSeparatingAxis(axesFirst[index], boxFirst, boxSecond, axesFirst, axesSecond, minimumPenetration, collisionNormal)) return false;
    }
    for (int index = 0; index < PhysicsConstants::Dimensions; ++index) {
        if (!TestSeparatingAxis(axesSecond[index], boxFirst, boxSecond, axesFirst, axesSecond, minimumPenetration, collisionNormal)) return false;
    }
    for (int indexFirst = 0; indexFirst < PhysicsConstants::Dimensions; ++indexFirst) {
        for (int indexSecond = 0; indexSecond < PhysicsConstants::Dimensions; ++indexSecond) {
            DirectX::SimpleMath::Vector3 crossAxis = axesFirst[indexFirst].Cross(axesSecond[indexSecond]);
            if (!TestSeparatingAxis(crossAxis, boxFirst, boxSecond, axesFirst, axesSecond, minimumPenetration, collisionNormal)) return false;
        }
    }

    outManifold.FirstObject = firstObject;
    outManifold.SecondObject = secondObject;
    outManifold.CollisionNormal = collisionNormal;
    outManifold.PenetrationDepth = minimumPenetration;

    // Rigidbodyコンポーネントから摩擦と反発係数を取得（持っていない場合は0とする）
    auto rb1 = firstObject->GetComponent<RigidbodyComponent>();
    auto rb2 = secondObject->GetComponent<RigidbodyComponent>();
    float friction1 = rb1 ? rb1->m_friction : 0.0f;
    float friction2 = rb2 ? rb2->m_friction : 0.0f;
    float restitution1 = rb1 ? rb1->m_restitution : 0.0f;
    float restitution2 = rb2 ? rb2->m_restitution : 0.0f;

    outManifold.CombinedFriction = friction1 * friction2;
    outManifold.CombinedRestitution = restitution1 * restitution2;

    GenerateContactPoints(boxFirst, boxSecond, outManifold);

    return true;
}

bool CollisionDetector::TestSeparatingAxis(const DirectX::SimpleMath::Vector3& testAxis, const DirectX::BoundingOrientedBox& firstBox, const DirectX::BoundingOrientedBox& secondBox, const DirectX::SimpleMath::Vector3 axisArrayFirst[PhysicsConstants::Dimensions], const DirectX::SimpleMath::Vector3 axisArraySecond[PhysicsConstants::Dimensions], float& minimumPenetration, DirectX::SimpleMath::Vector3& outCollisionNormal)
{
    float squaredLength = testAxis.LengthSquared();
    if (squaredLength < PhysicsConstants::SmallEpsilonValue) return true;

    DirectX::SimpleMath::Vector3 normalizedAxis = testAxis / std::sqrt(squaredLength);
    DirectX::SimpleMath::Vector3 centerFirst = firstBox.Center;
    DirectX::SimpleMath::Vector3 centerSecond = secondBox.Center;
    DirectX::SimpleMath::Vector3 centerDifference = centerSecond - centerFirst;

    float distanceBetweenCenters = std::abs(centerDifference.Dot(normalizedAxis));
    float projectionRadiusFirst =
        firstBox.Extents.x * std::abs(axisArrayFirst[0].Dot(normalizedAxis)) +
        firstBox.Extents.y * std::abs(axisArrayFirst[1].Dot(normalizedAxis)) +
        firstBox.Extents.z * std::abs(axisArrayFirst[2].Dot(normalizedAxis));

    float projectionRadiusSecond =
        secondBox.Extents.x * std::abs(axisArraySecond[0].Dot(normalizedAxis)) +
        secondBox.Extents.y * std::abs(axisArraySecond[1].Dot(normalizedAxis)) +
        secondBox.Extents.z * std::abs(axisArraySecond[2].Dot(normalizedAxis));

    if (distanceBetweenCenters > projectionRadiusFirst + projectionRadiusSecond + PhysicsConstants::ContactTolerance) {
        return false;
    }

    float currentPenetration = (projectionRadiusFirst + projectionRadiusSecond) - distanceBetweenCenters;

    if (currentPenetration < minimumPenetration) {
        minimumPenetration = currentPenetration;
        if (centerDifference.Dot(normalizedAxis) < 0.0f) {
            outCollisionNormal = -normalizedAxis;
        }
        else {
            outCollisionNormal = normalizedAxis;
        }
    }
    return true;
}

void CollisionDetector::GenerateContactPoints(const DirectX::BoundingOrientedBox& firstBox, const DirectX::BoundingOrientedBox& secondBox, CollisionManifold& outManifold)
{
    DirectX::XMFLOAT3 cornersFirst[PhysicsConstants::BoxVertexCount];
    DirectX::XMFLOAT3 cornersSecond[PhysicsConstants::BoxVertexCount];
    firstBox.GetCorners(cornersFirst);
    secondBox.GetCorners(cornersSecond);

    float maxProjectionFirst = -FLT_MAX;
    float minProjectionSecond = FLT_MAX;

    for (int index = 0; index < PhysicsConstants::BoxVertexCount; ++index) {
        DirectX::SimpleMath::Vector3 vertexFirst(cornersFirst[index].x, cornersFirst[index].y, cornersFirst[index].z);
        float projectionFirst = vertexFirst.Dot(outManifold.CollisionNormal);
        if (projectionFirst > maxProjectionFirst) maxProjectionFirst = projectionFirst;

        DirectX::SimpleMath::Vector3 vertexSecond(cornersSecond[index].x, cornersSecond[index].y, cornersSecond[index].z);
        float projectionSecond = vertexSecond.Dot(outManifold.CollisionNormal);
        if (projectionSecond < minProjectionSecond) minProjectionSecond = projectionSecond;
    }

    FindVerticesInsideTargetBox(secondBox, firstBox, outManifold.CollisionNormal, maxProjectionFirst, outManifold, false);
    FindVerticesInsideTargetBox(firstBox, secondBox, outManifold.CollisionNormal, minProjectionSecond, outManifold, true);

    if (outManifold.ContactPoints.empty()) {
        ContactInformation fallbackContact;
        fallbackContact.Position = (outManifold.FirstObject->Position() + outManifold.SecondObject->Position()) * 0.5f;
        fallbackContact.PenetrationDepth = outManifold.PenetrationDepth;
        outManifold.ContactPoints.push_back(fallbackContact);
    }
}

void CollisionDetector::FindVerticesInsideTargetBox(const DirectX::BoundingOrientedBox& sourceBox, const DirectX::BoundingOrientedBox& targetBox, const DirectX::SimpleMath::Vector3& collisionNormal, float referenceProjection, CollisionManifold& outManifold, bool isSourceFirstObject)
{
    DirectX::XMFLOAT3 sourceCorners[PhysicsConstants::BoxVertexCount];
    sourceBox.GetCorners(sourceCorners);

    DirectX::SimpleMath::Matrix inverseTargetMatrix =
        DirectX::SimpleMath::Matrix::CreateTranslation(-DirectX::SimpleMath::Vector3(targetBox.Center)) * DirectX::SimpleMath::Matrix::CreateFromQuaternion(targetBox.Orientation).Invert();

    for (int index = 0; index < PhysicsConstants::BoxVertexCount; ++index) {
        DirectX::SimpleMath::Vector3 vertexPosition(sourceCorners[index].x, sourceCorners[index].y, sourceCorners[index].z);
        EvaluateVertexForContact(vertexPosition, targetBox, inverseTargetMatrix, collisionNormal, referenceProjection, outManifold, isSourceFirstObject);
    }
}

void CollisionDetector::EvaluateVertexForContact(const DirectX::SimpleMath::Vector3& vertexPosition, const DirectX::BoundingOrientedBox& targetBox, const DirectX::SimpleMath::Matrix& inverseTargetMatrix, const DirectX::SimpleMath::Vector3& collisionNormal, float referenceProjection, CollisionManifold& outManifold, bool isSourceFirstObject)
{
    DirectX::SimpleMath::Vector3 localPosition = DirectX::SimpleMath::Vector3::Transform(vertexPosition, inverseTargetMatrix);
    DirectX::SimpleMath::Vector3 extents(targetBox.Extents);

    if (std::abs(localPosition.x) <= extents.x + PhysicsConstants::ContactTolerance &&
        std::abs(localPosition.y) <= extents.y + PhysicsConstants::ContactTolerance &&
        std::abs(localPosition.z) <= extents.z + PhysicsConstants::ContactTolerance)
    {
        ContactInformation newContact;
        newContact.Position = vertexPosition;

        float vertexProjection = vertexPosition.Dot(collisionNormal);
        if (isSourceFirstObject) {
            newContact.PenetrationDepth = std::max(vertexProjection - referenceProjection, 0.0f);
        }
        else {
            newContact.PenetrationDepth = std::max(referenceProjection - vertexProjection, 0.0f);
        }

        outManifold.ContactPoints.push_back(newContact);
    }
}