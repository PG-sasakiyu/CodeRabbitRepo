///
/// GameObject.h
/// コンポーネントを束ねる「器」となるクラス
///
#pragma once

#include "..\MainProject\Base\pch.h"
#include <nlohmann/json.hpp>
#include <DirectXCollision.h>
#include <DirectXMath.h>
#include "SimpleMath.h"
#include <string>
#include <vector>
#include <memory>
#include "Component.h"

class GameObject {
public:
    GameObject(std::string typeName, std::string instanceName);
    // 基底クラスとして継承される必要がなくなったため virtual は外しても構いませんが、今回は安全のため残します
    virtual ~GameObject() = default;

    // 純粋仮想関数(= 0)ではなくなり、自身が持つコンポーネントの処理を呼び出す実体メソッドになります
    virtual void Initialize();
    virtual void Update(float deltaTime);
    virtual void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj);

    virtual void Serialize(nlohmann::json& outJson) const;
    virtual void Deserialize(const nlohmann::json& inJson);

    void TransToMatrix();


    // 指定したコンポーネントをオブジェクトに追加する
    template <typename T, typename... Args>
    std::shared_ptr<T> AddComponent(Args&&... args) {
        // T型がComponentを継承しているかコンパイル時にチェック
        static_assert(std::is_base_of<Component, T>::value, "T must derive from Component");

        // 新しいコンポーネントを生成し、親として this を渡す
        auto newComponent = std::make_shared<T>(this, std::forward<Args>(args)...);
        m_components.push_back(newComponent);
        return newComponent;
    }

    // 指定したコンポーネントを所持していれば取得する
    template <typename T>
    std::shared_ptr<T> GetComponent() const {
        for (auto& comp : m_components) {
            std::shared_ptr<T> casted = std::dynamic_pointer_cast<T>(comp);
            if (casted) {
                return casted; // 見つかったら返す
            }
        }
        return nullptr; // 持っていなければ nullptr を返す
    }

    void RemoveComponent(std::shared_ptr<Component> comp) {
        std::erase(m_components, comp);
    }

    // 所持しているすべてのコンポーネントのリストを取得
    const std::vector<std::shared_ptr<Component>>& GetComponents() const {
        return m_components;
    }

    // -------------------------------------------------------------
    // ▼ 基本情報（コンポーネント化せずに器自体が持つべき情報） ▼
    // -------------------------------------------------------------
    std::string m_typeName;
    std::string m_instanceName;

    DirectX::SimpleMath::Vector3 GetPosition() const { return m_position; }
    DirectX::SimpleMath::Vector3 GetRotation() const { return m_rotation; }
    DirectX::SimpleMath::Vector3 GetScale() const { return m_scale; }

    DirectX::SimpleMath::Vector3& Position() { return m_position; }
    DirectX::SimpleMath::Vector3& Rotation() { return m_rotation; }
    DirectX::SimpleMath::Vector3& Scale() { return m_scale; }

    DirectX::SimpleMath::Matrix& GetWorldMatrix() { return m_modelWorld; }
    DirectX::SimpleMath::Quaternion m_qRotation = DirectX::SimpleMath::Quaternion::Identity;


protected:
    DirectX::SimpleMath::Matrix  m_modelWorld = DirectX::SimpleMath::Matrix::Identity;
    DirectX::SimpleMath::Vector3 m_position = { 0.0f, 0.0f, 0.0f };
    DirectX::SimpleMath::Vector3 m_rotation = { 0.0f, 0.0f, 0.0f };
    DirectX::SimpleMath::Vector3 m_scale = { 1.0f, 1.0f, 1.0f };

    DirectX::SimpleMath::Vector3 m_velocity = { 0.0f, 0.0f, 0.0f };
    DirectX::SimpleMath::Vector3 m_angularVelocity = { 0.0f, 0.0f, 0.0f };

private:
    // アタッチされたコンポーネントを保持するリスト
    std::vector<std::shared_ptr<Component>> m_components;
};