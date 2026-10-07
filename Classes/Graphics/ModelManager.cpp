///
/// ModelManager.cpp
///

#include "ModelManager.h"
#include "ResourceUploadBatch.h"
#include "RenderTargetState.h"
#include "EffectPipelineStateDescription.h"
#include <SpriteBatch.h>
#include <dxgiformat.h>

void ModelManager::CreateDeviceDependentResources()
{
    auto&& device = DXTK->Device;
    m_commonStates = DirectXTK::CreateCommonStates(device);

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.NumDescriptors = 128;
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    DX::ThrowIfFailed(device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(m_srvHeap.ReleaseAndGetAddressOf())));

    ResourceUploadBatch upload(device);
    upload.Begin();
    DirectX::RenderTargetState rtState(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_FORMAT_D32_FLOAT);
    DirectX::SpriteBatchPipelineStateDescription pd(rtState);
    m_spriteBatch = std::make_unique<DirectX::SpriteBatch>(device, upload, pd);
    auto finish = upload.End(DXTK->Command.Queue);
    finish.wait();
}

std::shared_ptr<ModelResource> ModelManager::GetModel(const wchar_t* modelFilePath)
{
    std::wstring key(modelFilePath);

    auto it = m_modelPool.find(key);
    if (it != m_modelPool.end()) return it->second;

    auto&& device = DXTK->Device;
    auto&& commandQueue = DXTK->Command.Queue;

    m_commonStates = DirectXTK::CreateCommonStates(device);
    auto resource = std::make_shared<ModelResource>();
    resource->model = DirectX::Model::CreateFromSDKMESH(device, modelFilePath);

    ResourceUploadBatch upload(device);
    upload.Begin();

    resource->textureFactory = resource->model->LoadTextures(device, upload);
    auto effectFactory = DirectXTK::CreateEffectFactory(
        resource->textureFactory->Heap(), m_commonStates->Heap()
    );

    auto uploadResourcesFinished = upload.End(commandQueue);
    uploadResourcesFinished.wait();

    RenderTargetState rtState(DXTK->SwapChain.Format, DXTK->SwapChain.DepthFormat);
    EffectPipelineStateDescription pd(
        nullptr, CommonStates::Opaque, CommonStates::DepthDefault, CommonStates::CullCounterClockwise, rtState
    );

    resource->effects = resource->model->CreateEffects(*effectFactory, pd, pd);

    m_modelPool[key] = resource;
    return resource;
}

std::shared_ptr<TextureResource> ModelManager::GetTexture(const wchar_t* textureFilePath)
{
    std::wstring key(textureFilePath);
    auto it = m_texturePool.find(key);
    if (it != m_texturePool.end()) return it->second;

    auto&& device = DXTK->Device;
    auto&& commandQueue = DXTK->Command.Queue;
    auto resource = std::make_shared<TextureResource>();

    ResourceUploadBatch upload(device);
    upload.Begin();
    std::wstring fullPath = key;

    DX::ThrowIfFailed(DirectX::CreateWICTextureFromFileEx(
        device,
        upload,
        fullPath.c_str(),
        0,
        D3D12_RESOURCE_FLAG_NONE,
        DirectX::WIC_LOADER_FORCE_RGBA32,
        resource->texture.ReleaseAndGetAddressOf()
    ));

    auto finish = upload.End(commandQueue);
    finish.wait();

    D3D12_RESOURCE_DESC desc = resource->texture->GetDesc();
    resource->size.x = static_cast<UINT>(desc.Width);
    resource->size.y = static_cast<UINT>(desc.Height);

    UINT incSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(m_srvHeap->GetCPUDescriptorHandleForHeapStart(), m_currentTextureIndex, incSize);
    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(m_srvHeap->GetGPUDescriptorHandleForHeapStart(), m_currentTextureIndex, incSize);

    device->CreateShaderResourceView(resource->texture.Get(), nullptr, cpuHandle);
    resource->srvHandle = gpuHandle;
    ++m_currentTextureIndex;

    m_texturePool[key] = resource;
    return resource;
}