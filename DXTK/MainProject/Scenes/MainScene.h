//
// MainScene.h
//

#pragma once
#include <vector>

#include "Scene.h"
#include "..\Classes\System\CameraController.h"
#include "..\Classes\Object\GameObject.h"
#include "..\Classes\Entity\PlayerModel.h"
#include "..\Base\StageManager.h"
#include "..\base\GameBase.h"

#include "..\External\imgui\imgui.h"
#include "..\External\imgui\imgui_impl_dx12.h"
#include "..\External\imgui\imgui_impl_win32.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;
using namespace DirectX;

class MainScene final : public Scene {
public:
	MainScene();
	virtual ~MainScene() { 
		Terminate();
		ImGui_ImplDX12_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	MainScene(MainScene&&) = default;
	MainScene& operator= (MainScene&&) = default;

	MainScene(MainScene const&) = delete;
	MainScene& operator= (MainScene const&) = delete;

	// These are the functions you will implement.
	void Start() override;

	//void LoadAssets() override;
	void CreateDeviceDependentResources() override;
	void CreateResources() override;

	void Initialize() override;
	void Terminate() override;

	void OnDeviceLost() override;
	void OnRestartSound() override;

	NextScene Update(const float deltaTime) override;
	void Render() override;

private:

	CameraController						 m_camera;
	StageManager							 m_stageManager;
	std::vector<std::shared_ptr<GameObject>> m_gameObjects;


	std::shared_ptr<GameObject>              m_selectedObject = nullptr;
};