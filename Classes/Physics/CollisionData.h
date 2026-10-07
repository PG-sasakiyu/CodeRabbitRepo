// CollisionData.h
#pragma once
#include "..\Object\GameObject.h"
#include <DirectXMath.h>
#include "SimpleMath.h"
#include <vector>

/// <summary>
/// 一つの接点情報を保持する構造体
/// 衝突した具体的な座標と、そこでの反発力や摩擦力の蓄積状態を記録します
/// </summary>
struct ContactInformation {
    // 接点のワールド座標
    DirectX::SimpleMath::Vector3 Position;

    // 接点におけるめり込み深さ
    float PenetrationDepth = 0.0f;

    // 法線方向（押し返す方向）の蓄積力積
    // 逐次インパルス法において、複数回の反復計算で蓄積された反発力を保持し、
    // 引っ張る力（マイナス）にならないようクランプするために使用します
    float AccumulatedNormalImpulse = 0.0f;

    // 接線方向（摩擦方向）の蓄積力積
    // 同様に、蓄積された摩擦力を保持し、最大静止摩擦力を超えないよう制限するために使用します
    float AccumulatedTangentImpulse = 0.0f;
};

/// <summary>
/// 衝突情報を保持する構造体（マニフォールド）
/// 二つのオブジェクト間の衝突全体を管理し、複数の接点情報をまとめます
/// </summary>
struct CollisionManifold {
    // 衝突した一つ目のオブジェクト
    GameObject* FirstObject = nullptr;

    // 衝突した二つ目のオブジェクト
    GameObject* SecondObject = nullptr;

    // 押し出し方向を表す衝突法線（FirstObjectからSecondObjectを押し出す方向）
    DirectX::SimpleMath::Vector3 CollisionNormal;

    // 衝突の最小めり込み深さ（全体での代表値）
    float PenetrationDepth = 0.0f;

    // 検出された接点情報のリスト（箱同士の衝突では面でぶつかるため、複数の接点が生まれます）
    std::vector<ContactInformation> ContactPoints;

    // 合成された摩擦係数（両者の係数を掛け合わせたもの）
    float CombinedFriction = 0.0f;

    // 合成された反発係数（両者の係数を掛け合わせたもの）
    float CombinedRestitution = 0.0f;
};