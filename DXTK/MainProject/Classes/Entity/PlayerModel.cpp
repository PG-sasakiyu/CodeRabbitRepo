///
/// PlayerModel.cpp
///

#include "PlayerModel.h"

PlayerModel::PlayerModel(std::string instanceName)
    : GameObject("PlayerModel", std::move(instanceName)) {}

void PlayerModel::Initialize()
{
    m_modelResource = ModelManager::GetInstance()->GetModel(L"cup.sdkmesh");
}

void PlayerModel::Update(float deltaTime)
{
    
}

void PlayerModel::Render(const DirectX::SimpleMath::Matrix& view, const DirectX::SimpleMath::Matrix& proj)
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