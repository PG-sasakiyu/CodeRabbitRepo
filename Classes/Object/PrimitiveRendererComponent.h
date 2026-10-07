// PrimitiveRendererComponent.h
#pragma once
#include "Component.h"
#include "..\Graphics\ModelManager.h"
#include "PrimitiveManager.h"
#include <string>

/// <summary>
/// 基本図形（Cube, Sphereなど）を描画するためのコンポーネント
/// </summary>
class PrimitiveRendererComponent : public Component {
public:
    PrimitiveRendererComponent(GameObject* owner, std::wstring textureName = L"BrickTiles.png", PrimitiveType shapeType = PrimitiveType::Cube);
    ~PrimitiveRendererComponent() override = default;

    void Initialize() override;
    void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;

    void Serialize(nlohmann::json& outJson) const override;
    void Deserialize(const nlohmann::json& inJson) override;

    // ImGui用のインスペクターUI描画
    void DrawInspectorUI() override;

    PrimitiveType m_shapeType;
    std::wstring m_textureName;

private:
    std::shared_ptr<TextureResource> m_textureResource;
};