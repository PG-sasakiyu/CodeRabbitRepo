// PrimitiveRendererComponent.cpp
#include "PrimitiveRendererComponent.h"
#include "..\Object\GameObject.h"
#include "..\MainProject\External\imgui\imgui.h"

PrimitiveRendererComponent::PrimitiveRendererComponent(GameObject* owner, std::wstring textureName, PrimitiveType shapeType)
    : Component(owner), m_textureName(std::move(textureName)), m_shapeType(shapeType)
{
}

void PrimitiveRendererComponent::Initialize()
{
    // ModelManagerからテクスチャリソースを取得
    m_textureResource = ModelManager::GetInstance()->GetTexture(m_textureName.c_str());
}

void PrimitiveRendererComponent::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
    if (!m_textureResource) return;

    auto modelMgr = ModelManager::GetInstance();
    auto primMgr = PrimitiveManager::GetInstance();
    auto commandList = DXTK->Command.List;

    ID3D12DescriptorHeap* heaps[] = { modelMgr->GetSrvHeap(), modelMgr->GetCommonStates()->Heap() };
    commandList->SetDescriptorHeaps(2, heaps);

    auto effect = primMgr->GetEffect();
    auto primitive = primMgr->GetPrimitive(m_shapeType);

    // 親のGameObjectが計算したワールド行列を受け取って描画に使う
    effect->SetWorld(m_owner->GetWorldMatrix());
    effect->SetView(view);
    effect->SetProjection(proj);
    effect->SetTexture(m_textureResource->srvHandle, modelMgr->GetCommonStates()->LinearWrap());

    effect->Apply(commandList);
    primitive->Draw(commandList);
}

void PrimitiveRendererComponent::Serialize(nlohmann::json& outJson) const
{
    // 自身のクラス名をキーにしてデータを保存
    outJson["PrimitiveRendererComponent"]["texture"] = std::string(m_textureName.begin(), m_textureName.end());
    outJson["PrimitiveRendererComponent"]["shape"] = static_cast<int>(m_shapeType);
}

void PrimitiveRendererComponent::Deserialize(const nlohmann::json& inJson)
{
    if (inJson.contains("PrimitiveRendererComponent")) {
        auto& compJson = inJson["PrimitiveRendererComponent"];

        if (compJson.contains("texture")) {
            std::string tex = compJson["texture"].get<std::string>();
            m_textureName = std::wstring(tex.begin(), tex.end());
        }
        if (compJson.contains("shape")) {
            m_shapeType = static_cast<PrimitiveType>(compJson["shape"].get<int>());
        }
    }
}

void PrimitiveRendererComponent::DrawInspectorUI()
{
    // 折りたたみ可能なヘッダーとしてUIを表示
    if (ImGui::TreeNodeEx("Primitive Renderer", ImGuiTreeNodeFlags_DefaultOpen)) {
        const char* shapeNames[] = { "Cube", "Sphere", "Cylinder", "Cone", "Teapot" };
        int currentShapeInt = static_cast<int>(m_shapeType);

        if (ImGui::BeginCombo("Shape", shapeNames[currentShapeInt])) {
            for (int i = 0; i < IM_ARRAYSIZE(shapeNames); i++) {
                const bool isSelected = (currentShapeInt == i);

                if (ImGui::Selectable(shapeNames[i], isSelected)) {
                    m_shapeType = static_cast<PrimitiveType>(i);
                }

                if (isSelected) {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        ImGui::TreePop();
    }
}