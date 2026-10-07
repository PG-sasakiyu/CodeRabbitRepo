//
// TestScene.h
//

#pragma once

#include "Scene.h"
#include "..\base\GameBase.h"

using Microsoft::WRL::ComPtr;
using std::unique_ptr;
using std::make_unique;
using namespace DirectX;

class TestScene final : public Scene {
public:
	TestScene();
	virtual ~TestScene() { Terminate(); }

	TestScene(TestScene&&) = default;
	TestScene& operator= (TestScene&&) = default;

	TestScene(TestScene const&) = delete;
	TestScene& operator= (TestScene const&) = delete;

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

	
};