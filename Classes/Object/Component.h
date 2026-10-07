// Component.h
#pragma once
#include <nlohmann/json.hpp>
#include <memory>

#define NOMINMAX
#undef min
#undef max

#include <DirectXMath.h>
#include "SimpleMath.h"

// 相互参照を解決するための前方宣言
class GameObject;

/// <summary>
/// すべてのコンポーネント（機能）の基底クラス
/// </summary>
class Component {
public:
    // コンストラクタで親オブジェクト（自身がアタッチされている器）を紐づける
    Component(GameObject* owner) : m_owner(owner) {}
    virtual ~Component() = default;

    // ゲームループごとの処理
    virtual void Initialize() {}
    virtual void Update(float deltaTime) {}
    virtual void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) {}

    // セーブ・ロード用の処理
    virtual void Serialize(nlohmann::json& outJson) const {}
    virtual void Deserialize(const nlohmann::json& inJson) {}

    // ImGuiのインスペクターに自身のパラメータを表示させるためのUI処理
    virtual void DrawInspectorUI() {}

protected:
    // このコンポーネントを所持している親オブジェクトへのポインタ
    GameObject* m_owner = nullptr;
};