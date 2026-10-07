// CollisionManager.h
#pragma once
#include "..\MainProject\Base\StageManager.h"

/// <summary>
/// 衝突の検知と解決を管理するクラス
/// 物理エンジンの衝突フェーズのパイプラインを制御します
/// </summary>
class CollisionManager {
public:
    // シングルトンインスタンスの取得
    static CollisionManager* GetInstance() {
        static CollisionManager instance;
        return &instance;
    }

    /// <summary>
    /// 全オブジェクトの衝突判定と修正処理を毎フレーム実行します
    /// </summary>
    void UpdateCollisions(StageManager& stageManager);

private:
    CollisionManager() = default;
    ~CollisionManager() = default;
};