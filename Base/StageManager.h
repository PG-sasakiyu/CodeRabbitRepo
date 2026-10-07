///
/// StageManager.h
///
#pragma once

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include "..\Classes\Object\GameObject.h"
#include "..\Classes\StageData.h"
#include "..\Classes\Object\PrimitiveRendererComponent.h"
#include "..\Classes\Object\ColliderComponent.h"
#include "..\Classes\Object\RigidbodyComponent.h"

#include <nlohmann/json.hpp>
#include <memory>
#include <fstream>
#include <vector>
#include <string>

class StageManager {
public:
    void Save(const std::string& filepath) {
        nlohmann::json rootJson;
        rootJson["objects"] = nlohmann::json::array();

        for (const auto& obj : m_objects) {
            nlohmann::json objJson;
            // オブジェクト本体のデータと、アタッチされた全コンポーネントのデータがJSON化される
            obj->Serialize(objJson);
            rootJson["objects"].push_back(objJson);
        }
        std::ofstream file(filepath);
        file << rootJson.dump(4);
    }

    void Load(const std::string& filepath) {
        m_objects.clear();
        std::ifstream file(filepath);
        if (!file.is_open()) return;

        nlohmann::json rootJson;
        file >> rootJson;

        for (const auto& objJson : rootJson["objects"]) {
            std::string type = objJson.value("type", "GameObject");
            auto newObj = CreateObjectByType(type);

            if (newObj) {
                newObj->m_typeName = type;

                // オブジェクト本体（Transformなど）の復元
                newObj->Deserialize(objJson);

                if (objJson.contains("components")) {
                    for (const auto& compJson : objJson["components"]) {
                        // JSONのキーを見て、該当するコンポーネントをアタッチし、データをロードする
                        if (compJson.contains("PrimitiveRendererComponent")) {
                            auto comp = newObj->AddComponent<PrimitiveRendererComponent>(L"");
                            comp->Deserialize(compJson);
                        }
                        else if (compJson.contains("ColliderComponent")) {
                            auto comp = newObj->AddComponent<ColliderComponent>();
                            comp->Deserialize(compJson);
                        }
                        else if (compJson.contains("RigidbodyComponent")) {
                            auto comp = newObj->AddComponent<RigidbodyComponent>();
                            comp->Deserialize(compJson);
                        }
                    }
                }

                newObj->Initialize();
                m_objects.push_back(newObj);
            }
        }
    }

    void AddObject(std::shared_ptr<GameObject> obj) { m_objects.push_back(obj); }

    void RemoveObject(std::shared_ptr<GameObject> obj) { std::erase(m_objects, obj); }

    std::vector<std::shared_ptr<GameObject>>& GetObjects() { return m_objects; }

private:
    std::vector<std::shared_ptr<GameObject>> m_objects;

    std::shared_ptr<GameObject> CreateObjectByType(const std::string& type)
    {
        if (type == "Stage") {
            return std::make_shared<StageData>();
        }
        return std::make_shared<GameObject>("GameObject", "LoadedObject");
    }
};