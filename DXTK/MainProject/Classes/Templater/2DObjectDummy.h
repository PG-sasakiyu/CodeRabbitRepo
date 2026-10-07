///
/// ObjectDummy2D.h
///
/// 

#pragma once

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include "..\Classes\GameObject.h"
#include "..\Classes\ModelManager.h"

using namespace DirectX;

class ObjectDummy2D : public GameObject{
public:

    ObjectDummy2D(std::string instanceName = "Dummy");
    ~ObjectDummy2D() override = default;

    void Initialize() override;
    void Update(float deltaTime) override;
    void Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj) override;

protected:

    std::shared_ptr<TextureResource> m_texture;
};