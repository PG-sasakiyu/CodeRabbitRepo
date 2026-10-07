///
/// PlayerModel.cpp
///

#include "3DObjectDummy.h"

ObjectDummy3D::ObjectDummy3D(std::string instanceName)
    : GameObject("Dummy", std::move(instanceName)) {}

void ObjectDummy3D::Initialize()
{
    m_modelResource = ModelManager::GetInstance()->GetModel(L"Dummy.sdkmesh");
}

void ObjectDummy3D::Update(float deltaTime)
{

}

void ObjectDummy3D::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
{
    if (!m_modelResource || !m_modelResource->model) return;

    auto&& commandList = DXTK->Command.List;

    auto commonStates = ModelManager::GetInstance()->GetCommonStates();
    if (m_modelResource->textureFactory && commonStates) {
        ID3D12DescriptorHeap* heaps[] = {
            m_modelResource->textureFactory->Heap(),
            commonStates->Heap()
        };
        commandList->SetDescriptorHeaps(static_cast<UINT>(std::size(heaps)), heaps);
    }

    DirectX::Model::UpdateEffectMatrices(
        m_modelResource->effects, m_modelWorld, view, proj
    );

    m_modelResource->model->Draw(commandList, m_modelResource->effects.cbegin());
}