//
// MainScene.cpp
//

#include "..\Base\pch.h"
#include "..\Base\dxtk.h"
#include "SceneFactory.h"

// ▼ PrimitiveObject.h を削除し、コンポーネント群をインクルード ▼
#include "..\Classes\Object\PrimitiveRendererComponent.h"
#include "..\Classes\Object\ColliderComponent.h"
#include "..\Classes\Object\RigidbodyComponent.h"

#include "..\Classes\Physics\CollisionManager.h"
#include "..\Classes\Physics\PhysicsManager.h"
#include "..\Base\ImGuiManager.h"

#ifdef _DEBUG
#pragma warning(disable : 4189)
#endif

using namespace SimpleMath;

MainScene::MainScene()
{
}

void MainScene::Start()
{
	LoadAssets();
	Initialize();
}

void MainScene::CreateDeviceDependentResources()
{
}

void MainScene::CreateResources()
{
}

void MainScene::Initialize()
{
	PrimitiveManager::GetInstance()->CreateDeviceDependentResources();

	m_camera.InitializeCamera();

	// -------------------------------------------------------------
	// ▼ 変更点1：コンポーネント指向での Floor（床）の生成
	// -------------------------------------------------------------
	auto floor = std::make_shared<GameObject>("GameObject", "Floor");
	floor->Position() = { 0.0f, -1.0f, 0.0f };
	floor->Scale() = { 50.0f, 1.0f, 50.0f };

	// 描画、当たり判定、物理挙動のコンポーネントをアタッチ
	floor->AddComponent<PrimitiveRendererComponent>(L"Tiles.png", PrimitiveType::Cube);
	floor->AddComponent<ColliderComponent>(ColliderType::AABB);
	auto floorRb = floor->AddComponent<RigidbodyComponent>();
	floorRb->m_isStatic = true; // 物理コンポーネント側で静的オブジェクトとして設定

	floor->Initialize();
	m_stageManager.AddObject(floor);


	// -------------------------------------------------------------
	// ▼ 変更点2：コンポーネント指向での FallingCube（落下する箱）の生成
	// -------------------------------------------------------------
	auto testCube = std::make_shared<GameObject>("GameObject", "FallingCube");
	testCube->Position() = { 0.0f, 5.0f, 0.0f };

	// 描画、当たり判定、物理挙動のコンポーネントをアタッチ
	testCube->AddComponent<PrimitiveRendererComponent>(L"Tiles.png", PrimitiveType::Cube);
	testCube->AddComponent<ColliderComponent>(ColliderType::OBB);
	auto cubeRb = testCube->AddComponent<RigidbodyComponent>();
	cubeRb->AngularVelocity() = { 5.0f, 0.0f, 5.0f }; // 物理コンポーネント側で回転速度を設定

	testCube->Initialize();
	m_stageManager.AddObject(testCube);
}

void MainScene::Terminate()
{
	DXTK->Audio.Engine->Suspend();
	DXTK->Audio.ResetEngine();
	DXTK->WaitForGpu();
	DXTK->DescriptorHeaps[0].reset();
}

void MainScene::OnDeviceLost()
{
	PrimitiveManager::GetInstance()->CreateDeviceDependentResources();
}

void MainScene::OnRestartSound()
{
}

NextScene MainScene::Update(const float deltaTime)
{
	UNREFERENCED_PARAMETER(deltaTime);

	m_camera.UpdateCamera(deltaTime);

	for (auto& obj : m_stageManager.GetObjects()) {
		obj->Update(deltaTime);
	}

	// 正しい物理パイプラインの順序で呼び出し
	PhysicsManager::GetInstance()->UpdateForces(m_stageManager, deltaTime);
	CollisionManager::GetInstance()->UpdateCollisions(m_stageManager);
	PhysicsManager::GetInstance()->UpdateVelocitiesAndPositions(m_stageManager, deltaTime);

	return NextScene::Continue;
}

void MainScene::Render()
{
	DXTK->BeginScene();
	DXTK->ClearRenderTarget(Colors::CornflowerBlue);

	auto cameraData = m_camera.GetCamera();

	for (auto& obj : m_stageManager.GetObjects()) {
		obj->Render(cameraData.ViewMatrix, cameraData.ProjectionMatrix);
	}

	auto imguiMgr = ImGuiManager::GetInstance();
	imguiMgr->RenderEditorUI(m_stageManager, m_selectedObject);
	imguiMgr->Render(DXTK->Command.List);

	DXTK->EndScene();
}