///
/// PlayerModel.h
///

#pragma once
#include "..\Object\GameObject.h"
#include "..\Graphics\ModelManager.h"

class PlayerModel : public GameObject {
public:
    PlayerModel(std::string instanceName = "Player");
    ~PlayerModel() override = default;

    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;

private:
    std::shared_ptr<ModelResource> m_modelResource;
};