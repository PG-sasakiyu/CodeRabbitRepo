//
// StageData.h
//

#pragma once
#include "..\Classes\Object\GameObject.h"
#include "..\Classes\Graphics\ModelManager.h"

class StageData : public GameObject {
public:
    StageData(std::string instanceName = "StageData_0");
    ~StageData() override = default;

    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;

private:
    std::shared_ptr<TextureResource> m_texture;
};