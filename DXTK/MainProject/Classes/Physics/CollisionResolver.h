// CollisionResolver.h
#pragma once
#include "..\Object\GameObject.h"
#include "CollisionData.h"
#include <vector>

/// <summary>
/// 衝突情報を元に速度や回転および位置を修正するクラス
/// 逐次インパルス法を用いて、物理的に正しい反発や摩擦をシミュレーションします
/// </summary>
class CollisionResolver {
public:
    // シングルトンインスタンスの取得
    static CollisionResolver* GetInstance() {
        static CollisionResolver instance;
        return &instance;
    }

    /// <summary>
    /// 全ての衝突情報に対して解決処理を実行します
    /// </summary>
    void ResolveCollisions(std::vector<CollisionManifold>& targetManifolds);

private:
    CollisionResolver() = default;
    ~CollisionResolver() = default;

    /// <summary>
    /// 速度の反復解決を行います（反発力と摩擦力を計算して速度を変更）
    /// </summary>
    void ResolveVelocityPhase(std::vector<CollisionManifold>& targetManifolds);

    /// <summary>
    /// 位置と回転の反復解決を行います（めり込みを直接解消）
    /// </summary>
    void ResolvePositionPhase(std::vector<CollisionManifold>& targetManifolds);

    /// <summary>
    /// 一つの接点に対する法線方向の反発力（インパルス）を計算し適用します
    /// </summary>
    void ApplyNormalImpulse(const CollisionManifold& currentManifold, ContactInformation& currentContact);

    /// <summary>
    /// 一つの接点に対する接線方向の摩擦力（インパルス）を計算し適用します
    /// </summary>
    void ApplyFrictionImpulse(const CollisionManifold& currentManifold, ContactInformation& currentContact);

    /// <summary>
    /// 位置のめり込みを解消するための疑似インパルスを計算し適用します
    /// </summary>
    void ApplyPseudoImpulseForPosition(const CollisionManifold& currentManifold, const ContactInformation& currentContact);

    /// <summary>
    /// オブジェクトの静的状態（静止やスリープ）を考慮した逆質量を取得します
    /// </summary>
    float GetInverseMass(GameObject* targetObject);

    /// <summary>
    /// オブジェクトの静的状態を考慮した逆慣性モーメントを取得します
    /// </summary>
    float GetInverseInertia(GameObject* targetObject);

    /// <summary>
    /// オブジェクトの重心から接点までの半径ベクトル（回転のアーム）を計算します
    /// </summary>
    DirectX::SimpleMath::Vector3 CalculateRadiusVector(GameObject* targetObject, const DirectX::SimpleMath::Vector3& contactPosition);

    /// <summary>
    /// 直進速度と角速度を合成し、接点における2物体の相対速度を計算します
    /// </summary>
    DirectX::SimpleMath::Vector3 CalculateRelativeVelocity(GameObject* firstObject, GameObject* secondObject, const DirectX::SimpleMath::Vector3& radiusFirst, const DirectX::SimpleMath::Vector3& radiusSecond);

    /// <summary>
    /// 計算された力積ベクトルを用いて、オブジェクトの速度と角速度を変化させます
    /// </summary>
    void ApplyImpulseToObject(GameObject* targetObject, const DirectX::SimpleMath::Vector3& impulseVector, const DirectX::SimpleMath::Vector3& radiusVector);

    /// <summary>
    /// 疑似インパルスを用いてオブジェクトの位置（座標）を直接移動させます
    /// </summary>
    void ApplyPositionalMovement(GameObject* targetObject, const DirectX::SimpleMath::Vector3& movementVector, float inverseMass);

    /// <summary>
    /// 疑似インパルスを用いてオブジェクトの回転姿勢を直接修正します
    /// </summary>
    void ApplyRotationalMovement(GameObject* targetObject, const DirectX::SimpleMath::Vector3& rotationAxisAndAngle, float inverseInertia);
};