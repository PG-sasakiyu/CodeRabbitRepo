///
/// 3DObjectDummy.h
///

#pragma once

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include "GameObject.h"
#include "ModelManager.h"
using namespace DirectX;

class ObjectDummy3D : public GameObject {
public:

    ObjectDummy3D(std::string instanceName = "Dummy");
    ~ObjectDummy3D() override = default;

    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;

private:
    std::shared_ptr<ModelResource> m_modelResource;

};