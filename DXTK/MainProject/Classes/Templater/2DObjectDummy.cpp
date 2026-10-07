//
// ObjectDummy2D.cpp
//

#include "2DObjectDummy.h"

ObjectDummy2D::ObjectDummy2D(std::string instanceName)
    : GameObject("StageData", std::move(instanceName)) {}

void ObjectDummy2D::Initialize()
{
    m_texture = ModelManager::GetInstance()->GetTexture(L"Dummy.png");
    TransToMatrix();
}

void ObjectDummy2D::Update(float deltaTime) 
{

}

void ObjectDummy2D::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
    if (!m_texture) return;

    auto manager = ModelManager::GetInstance();
    auto spriteBatch = manager->GetSpriteBatch();
    auto commandList = DXTK->Command.List;

    if (spriteBatch) {
        ID3D12DescriptorHeap* heaps[] = { manager->GetSrvHeap() };
        commandList->SetDescriptorHeaps(1, heaps);

        spriteBatch->Begin(commandList);
        DirectX::SimpleMath::Vector2 pos(m_position.x, m_position.y);

        spriteBatch->Draw(m_texture->srvHandle, m_texture->size, pos);
        spriteBatch->End();
    }
}