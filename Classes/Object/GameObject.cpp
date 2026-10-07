//
// GameObject.cpp
//

#include "GameObject.h"
#include <algorithm>

GameObject::GameObject(std::string typeName, std::string instanceName)
    : m_typeName(std::move(typeName)), m_instanceName(std::move(instanceName))
{
}

void GameObject::Initialize()
{
    // 所持している全コンポーネントの初期化を呼び出す
    for (auto& comp : m_components) {
        comp->Initialize();
    }

    TransToMatrix();
}

void GameObject::Update(float deltaTime)
{
    // 所持している全コンポーネントの更新処理を呼び出す
    for (auto& comp : m_components) {
        comp->Update(deltaTime);
    }

    TransToMatrix();
}



void GameObject::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
    // 描画処理が必要なコンポーネント（Renderer系）に処理を委譲する
    for (auto& comp : m_components) {
        comp->Render(view, proj);
    }
}

void GameObject::TransToMatrix()
{
    using namespace DirectX::SimpleMath;
    m_modelWorld =
        DirectX::SimpleMath::Matrix::CreateScale(m_scale)
        * DirectX::SimpleMath::Matrix::CreateFromQuaternion(m_qRotation)
        * DirectX::SimpleMath::Matrix::CreateTranslation(m_position);
}

void GameObject::Serialize(nlohmann::json& outJson) const
{
    outJson["type"] = m_typeName;
    outJson["name"] = m_instanceName;
    outJson["position"] = { m_position.x, m_position.y, m_position.z };
    outJson["rotation"] = { m_rotation.x, m_rotation.y, m_rotation.z };
    outJson["scale"] = { m_scale.x,    m_scale.y,    m_scale.z };

    // 全コンポーネントのデータを保存
    nlohmann::json componentsJson = nlohmann::json::array();
    for (auto& comp : m_components) {
        nlohmann::json compData;
        comp->Serialize(compData);
        if (!compData.empty()) {
            componentsJson.push_back(compData);
        }
    }
    outJson["components"] = componentsJson;
}

void GameObject::Deserialize(const nlohmann::json& inJson)
{
    if (inJson.contains("name")) {
        m_instanceName = inJson["name"].get<std::string>();
    }

    if (inJson.contains("position")) {
        m_position.x = inJson["position"][0];
        m_position.y = inJson["position"][1];
        m_position.z = inJson["position"][2];
    }
    if (inJson.contains("rotation")) {
        m_rotation.x = inJson["rotation"][0];
        m_rotation.y = inJson["rotation"][1];
        m_rotation.z = inJson["rotation"][2];
    }
    if (inJson.contains("scale")) {
        m_scale.x = inJson["scale"][0];
        m_scale.y = inJson["scale"][1];
        m_scale.z = inJson["scale"][2];
    }

    TransToMatrix();
}