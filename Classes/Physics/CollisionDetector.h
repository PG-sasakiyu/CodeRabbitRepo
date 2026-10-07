// CollisionDetector.h
#pragma once
#include "..\Object\GameObject.h"
#include "CollisionData.h"
#include "PhysicsConstants.h"
#include <vector>
#include <memory>

/// <summary>
/// 衝突を検知するクラス
/// 分離軸定理(SAT)を用いてオブジェクト間の衝突を判定し、接点情報を生成します
/// </summary>
class CollisionDetector {
public:
    // シングルトンインスタンスの取得
    static CollisionDetector* GetInstance() {
        static CollisionDetector instance;
        return &instance;
    }

    /// <summary>
    /// 全てのオブジェクトの組み合わせから回転境界箱(OBB)の衝突を検知します
    /// </summary>
    std::vector<CollisionManifold> DetectCollisions(const std::vector<std::shared_ptr<GameObject>>& targetObjects);

private:
    CollisionDetector() = default;
    ~CollisionDetector() = default;

    /// <summary>
    /// 二つのオブジェクトのペアを評価し、衝突していればリストに追加します
    /// </summary>
    void EvaluatePairForCollision(GameObject* firstObject, GameObject* secondObject, std::vector<CollisionManifold>& outManifolds);

    /// <summary>
    /// 任意のオブジェクトのコライダー情報を回転境界箱(OBB)として統一して取得します
    /// （AABBの場合もOBB形式に変換して統一的に処理できるようにします）
    /// </summary>
    DirectX::BoundingOrientedBox GetAsOrientedBoundingBox(GameObject* targetObject);

    /// <summary>
    /// 回転境界箱同士の分離軸定理(SAT)を用いた衝突判定を行います
    /// </summary>
    bool CheckOrientedBoundingBoxCollision(GameObject* firstObject, GameObject* secondObject, CollisionManifold& outManifold);

    /// <summary>
    /// 回転境界箱の三つのローカル軸（X, Y, Z方向の向きを表すベクトル）を抽出します
    /// </summary>
    void ExtractLocalAxes(const DirectX::BoundingOrientedBox& targetBox, DirectX::SimpleMath::Vector3 outAxes[PhysicsConstants::Dimensions]);

    /// <summary>
    /// 一つのテスト軸に対して分離軸定理を適用し、重なりを検証します
    /// </summary>
    bool TestSeparatingAxis(const DirectX::SimpleMath::Vector3& testAxis, const DirectX::BoundingOrientedBox& firstBox, const DirectX::BoundingOrientedBox& secondBox, const DirectX::SimpleMath::Vector3 axisArrayFirst[PhysicsConstants::Dimensions], const DirectX::SimpleMath::Vector3 axisArraySecond[PhysicsConstants::Dimensions], float& minimumPenetration, DirectX::SimpleMath::Vector3& outCollisionNormal);

    /// <summary>
    /// 衝突情報のリストであるマニフォールドに、具体的な接点（Contact Point）を生成して追加します
    /// </summary>
    void GenerateContactPoints(const DirectX::BoundingOrientedBox& firstBox, const DirectX::BoundingOrientedBox& secondBox, CollisionManifold& outManifold);

    /// <summary>
    /// 相手の境界箱の中に含まれる頂点を探索し、接点として追加します
    /// </summary>
    void FindVerticesInsideTargetBox(const DirectX::BoundingOrientedBox& sourceBox, const DirectX::BoundingOrientedBox& targetBox, const DirectX::SimpleMath::Vector3& collisionNormal, float referenceProjection, CollisionManifold& outManifold, bool isSourceFirstObject);

    /// <summary>
    /// 一つの頂点が目標の境界箱内に存在するか判定し、接点情報を構築します
    /// </summary>
    void EvaluateVertexForContact(const DirectX::SimpleMath::Vector3& vertexPosition, const DirectX::BoundingOrientedBox& targetBox, const DirectX::SimpleMath::Matrix& inverseTargetMatrix, const DirectX::SimpleMath::Vector3& collisionNormal, float referenceProjection, CollisionManifold& outManifold, bool isSourceFirstObject);
};