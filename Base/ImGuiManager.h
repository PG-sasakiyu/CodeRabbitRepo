///
/// ImGuiManager.h
///
#pragma once
#include "..\Base\pch.h"
#include "..\Classes\Object\GameObject.h"
#include "..\Base\StageManager.h"
#include <d3d12.h>
#include <vector>
#include <memory>
#include <string>

class ImGuiManager {
public:

    ImGuiManager() = default;
    ~ImGuiManager() = default;

    static ImGuiManager* GetInstance() {
        static ImGuiManager instance;
        return &instance;
    }

    void Initialize(HWND window, ID3D12Device* device, int swapBufferCount, ID3D12DescriptorHeap* srvHeap);
    void NewFrame();
    void Render(ID3D12GraphicsCommandList* commandList);
    void Shutdown();
    void RenderEditorUI(
        StageManager& stageManager,
        std::shared_ptr<GameObject>& selectedObject
    );

    bool WantCaptureMouse() const;
    bool WantCaptureKeyboard() const;

private:
    void SaveStage(const std::string& filepath, const std::vector<std::shared_ptr<GameObject>>& gameObjects);
    void LoadStage(const std::string& filepath, std::vector<std::shared_ptr<GameObject>>& gameObjects, std::shared_ptr<GameObject>& selectedObject);

    ID3D12DescriptorHeap* m_srvHeap = nullptr;
    int m_branchCount = 0;
};