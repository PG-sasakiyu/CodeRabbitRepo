///
/// ModelManager.h
///
/// 

#pragma once

#include "..\MainProject\Base\pch.h"
#include "..\MainProject\Base\dxtk.h"
#include <unordered_map>
#include <string>
#include <memory>

struct ModelResource {
    std::shared_ptr<DirectX::Model>                  model;
    DirectX::Model::EffectCollection                 effects;
    std::shared_ptr<DirectX::EffectTextureFactory>   textureFactory;
};

struct TextureResource {
    Microsoft::WRL::ComPtr<ID3D12Resource> texture;
    D3D12_GPU_DESCRIPTOR_HANDLE            srvHandle;
    DirectX::XMUINT2                       size;
};

class ModelManager {
public:
    static ModelManager* GetInstance() {
        static ModelManager instance;
        return &instance;
    }

    void CreateDeviceDependentResources();

    std::shared_ptr<ModelResource> GetModel(const wchar_t* modelFilePath);
    std::shared_ptr<TextureResource> GetTexture(const wchar_t* textureFilePath);

    DirectX::SpriteBatch* GetSpriteBatch() { return m_spriteBatch.get(); }
    ID3D12DescriptorHeap* GetSrvHeap() { return m_srvHeap.Get(); }
    DirectX::CommonStates* GetCommonStates() { return m_commonStates.get(); }

private:
    ModelManager() = default;
    ~ModelManager() = default;

    std::unordered_map<std::wstring, std::shared_ptr<ModelResource>>   m_modelPool;
    std::unordered_map<std::wstring, std::shared_ptr<TextureResource>> m_texturePool;

    std::unique_ptr<DirectX::CommonStates>       m_commonStates;
    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_srvHeap;
    std::unique_ptr<DirectX::SpriteBatch>        m_spriteBatch;

    UINT m_currentTextureIndex = 1;
};