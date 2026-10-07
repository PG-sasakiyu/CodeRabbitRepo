///
/// ImGuiManager.cpp
///
#include "ImGuiManager.h"
#include "..\MainProject\Classes\Object\PrimitiveManager.h"
#include "..\MainProject\Classes\Object\PrimitiveRendererComponent.h"
#include "..\MainProject\Classes\Object\ColliderComponent.h"
#include "..\MainProject\Classes\Object\RigidbodyComponent.h"
#include "..\External\imgui\imgui.h"
#include "..\External\imgui\imgui_impl_win32.h"
#include "..\External\imgui\imgui_impl_dx12.h"

void ImGuiManager::Initialize(HWND window, ID3D12Device* device, int swapBufferCount, ID3D12DescriptorHeap* srvHeap)
{
    m_srvHeap = srvHeap;
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(window);

    CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(srvHeap->GetCPUDescriptorHandleForHeapStart());
    CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(srvHeap->GetGPUDescriptorHandleForHeapStart());

    ImGui_ImplDX12_Init(device, swapBufferCount, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, srvHeap, cpuHandle, gpuHandle);
    ImGui::GetIO().FontGlobalScale = 1.0f;

    unsigned char* pixels;
    int texWidth, texHeight;
    ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &texWidth, &texHeight);
}

void ImGuiManager::NewFrame() {
    ImGui_ImplDX12_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiManager::Render(ID3D12GraphicsCommandList* commandList) {
    ImGui::Render();
    ID3D12DescriptorHeap* heaps[] = { m_srvHeap };
    commandList->SetDescriptorHeaps(1, heaps);
    ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
}

void ImGuiManager::Shutdown() {
    ImGui_ImplDX12_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiManager::RenderEditorUI(
    StageManager& stageManager,
    std::shared_ptr<GameObject>& selectedObject)
{
    ImGui::Begin("Stage Master Editor", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save Stage")) {
                stageManager.Save("stage.json");
            }
            if (ImGui::MenuItem("Load Stage")) {
                stageManager.Load("stage.json");
                selectedObject = nullptr;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::BeginChild("Hierarchy", ImVec2(250, 0), true);

    // 【変更点1】空のオブジェクト（Transformのみ）を生成するように変更
    if (ImGui::Button("+ Add Empty Object", ImVec2(-1, 0))) {
        m_branchCount++;
        std::string uniqueName = "Object_" + std::to_string(m_branchCount);

        auto newObject = std::make_shared<GameObject>("GameObject", uniqueName);
        newObject->Initialize();

        stageManager.AddObject(newObject);
        selectedObject = newObject;
    }

    ImGui::Separator();

    auto& gameObjects = stageManager.GetObjects();
    for (size_t i = 0; i < gameObjects.size(); ++i) {
        bool isSelected = (selectedObject == gameObjects[i]);
        if (ImGui::Selectable(gameObjects[i]->m_instanceName.c_str(), isSelected)) {
            selectedObject = gameObjects[i];
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    ImGui::BeginChild("Inspector", ImVec2(0, 0), true);

    if (selectedObject) {
        char nameBuffer[256];
        strcpy_s(nameBuffer, sizeof(nameBuffer), selectedObject->m_instanceName.c_str());
        if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer))) {
            selectedObject->m_instanceName = nameBuffer;
        }

        ImGui::Text("Type: %s", selectedObject->m_typeName.c_str());
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::DragFloat3("Position", &selectedObject->Position().x, 0.05f);

        DirectX::XMFLOAT3& rot = selectedObject->Rotation();
        float displayRot[3] = {
            DirectX::XMConvertToDegrees(rot.x),
            DirectX::XMConvertToDegrees(rot.y),
            DirectX::XMConvertToDegrees(rot.z)
        };
        if (ImGui::DragFloat3("Rotation", displayRot, 0.5f)) {
            rot.x = DirectX::XMConvertToRadians(displayRot[0]);
            rot.y = DirectX::XMConvertToRadians(displayRot[1]);
            rot.z = DirectX::XMConvertToRadians(displayRot[2]);
        }
        ImGui::DragFloat3("Scale", &selectedObject->Scale().x, 0.05f);

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        // 削除対象のコンポーネントを一時保存するポインタ
        std::shared_ptr<Component> componentToRemove = nullptr;

        // アタッチされている全コンポーネントのUIを自動描画する
        for (auto& comp : selectedObject->GetComponents()) {
            ImGui::PushID(comp.get()); // 同名のボタン等でImGuiのIDが衝突するのを防ぐ

            comp->DrawInspectorUI();

            // 【変更点2】コンポーネントを削除するボタンを追加
            if (ImGui::Button("Remove Component", ImVec2(-1, 0))) {
                componentToRemove = comp;
            }

            ImGui::PopID();
            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();
        }

        // ループ内で要素を削除するとエラーになるため、ループ外で安全に削除処理を実行
        if (componentToRemove) {
            selectedObject->RemoveComponent(componentToRemove);
        }

        if (ImGui::Button("Add Component", ImVec2(-1, 0))) {
            ImGui::OpenPopup("AddComponentMenu");
        }
        if (ImGui::BeginPopup("AddComponentMenu")) {
            if (ImGui::MenuItem("Primitive Renderer")) {
                if (!selectedObject->GetComponent<PrimitiveRendererComponent>()) {
                    selectedObject->AddComponent<PrimitiveRendererComponent>()->Initialize();
                }
            }
            if (ImGui::MenuItem("Collider")) {
                if (!selectedObject->GetComponent<ColliderComponent>()) {
                    selectedObject->AddComponent<ColliderComponent>();
                }
            }
            if (ImGui::MenuItem("Rigidbody")) {
                if (!selectedObject->GetComponent<RigidbodyComponent>()) {
                    selectedObject->AddComponent<RigidbodyComponent>();
                }
            }
            ImGui::EndPopup();
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("Delete Object", ImVec2(-1, 0))) {
            std::erase(gameObjects, selectedObject);
            selectedObject = nullptr;
        }
    }
    else {
        ImGui::TextDisabled("Select an object from the hierarchy list.");
    }

    ImGui::EndChild();
    ImGui::End();
}

bool ImGuiManager::WantCaptureMouse() const { return ImGui::GetIO().WantCaptureMouse; }
bool ImGuiManager::WantCaptureKeyboard() const { return ImGui::GetIO().WantCaptureKeyboard; }