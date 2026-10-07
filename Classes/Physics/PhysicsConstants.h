// PhysicsConstants.h
#pragma once

/// <summary>
/// 物理演算の定数を管理する名前空間
/// </summary>
namespace PhysicsConstants {
    // 下方向にかかる重力加速度（m/s^2）
    constexpr float GravityAcceleration = 9.8f;

    // 基本となる1フレームのタイムステップ（秒）。約60FPS（1/60 ≒ 0.016秒）を想定
    constexpr float TimeStepSeconds = 0.016f;

    // 許容されるオブジェクト同士のめり込み量（この値以下なら押し出し補正を行わない）
    constexpr float AllowedPenetrationDepth = 0.02f;

    // めり込みを1フレームでどれだけ解消するか（0.2 = 20%）。1.0にすると弾けて振動する原因になるため抑えめに設定
    constexpr float PenetrationCorrectionRatio = 0.2f;

    // 反発係数を0にし、バウンドを停止させる（静止状態とみなす）相対速度の閾値
    constexpr float RestingVelocityThreshold = 0.5f;

    // オブジェクトのデフォルト質量（重さ）
    constexpr float DefaultBaseMass = 1.0f;

    // オブジェクトのデフォルト慣性モーメント（回転しにくさ）
    constexpr float DefaultInertiaMoment = 0.5f;

    // ゼロ除算の回避や、計算上の微小な誤差を無視するための極小値（イプシロン）
    constexpr float SmallEpsilonValue = 0.0001f;

    // 接点（Contact Point）を生成する際に、完全に触れていなくても接触とみなす許容距離
    constexpr float ContactTolerance = 0.05f;

    // 反発や摩擦を計算する際、計算をスキップする微小な速度差の許容誤差（スロップ）
    constexpr float VelocitySlop = 0.02f;

    // 速度（反発力・摩擦力）を正確に計算するためのソルバーの反復処理回数（多いほど正確だが重い）
    constexpr int VelocitySolverIterations = 8;

    // 位置（めり込み解消）を正確に計算するためのソルバーの反復処理回数
    constexpr int PositionSolverIterations = 3;

    // スリープ判定に用いる、直進速度が「停止している」とみなされる閾値（カットオフ値）
    constexpr float LinearVelocityCutoff = 0.05f;

    // スリープ判定に用いる、角速度が「停止している」とみなされる閾値（カットオフ値）
    constexpr float AngularVelocityCutoff = 0.05f;

    // 速度がカットオフ値を下回ってから、完全にスリープ（物理演算停止）するまでの待機時間（秒）
    constexpr float SleepWaitTime = 0.5f;

    // スリープ中のオブジェクトに外部から力が加わった際、スリープを強制解除する力積の閾値
    constexpr float WakeUpImpulseThreshold = 0.1f;

    // スリープ時にオブジェクトの角度を綺麗な直角に補正する際の、許容されるズレの角度（0.087rad ≒ 約5度）
    constexpr float AngleCorrectionTolerance = 0.087f;

    // 3D空間の次元数（X, Y, Z）
    constexpr int Dimensions = 3;

    // OBB（有向境界箱）が持つ頂点の数
    constexpr int BoxVertexCount = 8;

    // 物理シミュレーションにおける許容される最大のフレーム間遅延時間（秒）
    constexpr float MaxDeltaTime = 0.05f;

    // フレーム落ちが発生した際に使用される代替の固定フレーム時間
    constexpr float FixedDeltaTime = 0.016f;

    // オブジェクトが接地していると判定するための、衝突法線のY成分の閾値
    //（0.7f ≒ 約45度。この値より平らな面であれば接地しているとみなす）
    constexpr float GroundNormalThreshold = 0.7f;

    // スリープ状態へ移行する際、速度の減衰を開始するタイミング（スリープ待機時間の割合）
    constexpr float SleepDampingStartRatio = 0.5f;

    // スリープ移行中にかかる速度の減衰率
    constexpr float SleepDampingFactor = 0.9f;

    // スリープ状態を強制解除するための速度の値
    constexpr float WakeUpVelocityMultiplier = 2.0f;
}