// CollisionManager.cpp
#include "CollisionManager.h"
#include "CollisionDetector.h"
#include "CollisionResolver.h"
#include "..\MainProject\Classes\Object\RigidbodyComponent.h"

void CollisionManager::UpdateCollisions(StageManager& stageManager)
{
    const std::vector<std::shared_ptr<GameObject>>& targetObjects = stageManager.GetObjects();

    // Rigidbody を持つオブジェクトの接地判定を初期化します
    for (const std::shared_ptr<GameObject>& objectPointer : targetObjects) {
        auto rb = objectPointer->GetComponent<RigidbodyComponent>();
        if (rb) {
            rb->m_isGrounded = false;
        }
    }

    // 衝突検知と解決
    std::vector<CollisionManifold> manifolds = CollisionDetector::GetInstance()->DetectCollisions(targetObjects);
    CollisionResolver::GetInstance()->ResolveCollisions(manifolds);
}