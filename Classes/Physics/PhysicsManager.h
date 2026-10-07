// PhysicsManager.h
#pragma once
#include "..\MainProject\Base\StageManager.h"
#include "..\Object\GameObject.h"

/// <summary>
/// 物理演算の全体を管理するクラス
/// 外力の適用、積分（座標と回転の更新）、スリープ状態の管理などを制御します
/// </summary>
class PhysicsManager {
public:
    // シングルトンインスタンスの取得
    static PhysicsManager* GetInstance() {
        static PhysicsManager instance;
        return &instance;
    }

    /// <summary>
    /// 各オブジェクトに対して重力や空気抵抗などの外力を適用するフェーズ
    /// </summary>
    void UpdateForces(StageManager& stageManager, float deltaTime);

    /// <summary>
    /// 速度・角速度を用いて座標・回転を更新し、スリープ状態を評価するフェーズ
    /// </summary>
    void UpdateVelocitiesAndPositions(StageManager& stageManager, float deltaTime);

private:
    PhysicsManager() = default;
    ~PhysicsManager() = default;

    /// <summary>
    /// 重力による下向きの速度加算を行います
    /// </summary>
    void ApplyGravity(GameObject* targetObject, float deltaTime);

    /// <summary>
    /// 空気抵抗や摩擦による速度と角速度の自然減衰（ダンピング）を行います
    /// </summary>
    void ApplyDamping(GameObject* targetObject, float deltaTime);

    /// <summary>
    /// 速度を用いて位置座標を更新します（オイラー積分）
    /// </summary>
    void IntegrateVelocity(GameObject* targetObject, float deltaTime);

    /// <summary>
    /// 角速度を用いてクォータニオン（回転姿勢）を更新します
    /// </summary>
    void IntegrateRotation(GameObject* targetObject, float deltaTime);

    /// <summary>
    /// クォータニオンから、エディタ等での表示用オイラー角(Pitch, Yaw, Roll)を再計算して同期します
    /// </summary>
    void UpdateEulerAngles(GameObject* targetObject);

    /// <summary>
    /// 動きが止まりかけているかを評価し、条件を満たせばオブジェクトをスリープ状態（計算停止）にします
    /// </summary>
    void EvaluateSleepState(GameObject* targetObject, float deltaTime);

    /// <summary>
    /// オブジェクトが静止する際、角度が直角（90度）に近い場合はピタッと直角に補正します
    /// </summary>
    void CorrectRotationToRightAngles(GameObject* targetObject);
};