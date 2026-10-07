#include "CameraController.h"

#include <string>
#include <windows.h>

using namespace SimpleMath;

void CameraController::InitializeCamera() 
{

	m_mainCameraPosition = Vector3(0.0f, 0.0f, 0.0f);
	m_mainCameraRotation = Quaternion::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);

	m_type = Type::FPS;

	switch (m_type)
	{
	case CameraController::Type::TPS:
		InitializeTPSCamera();
		break;
	case CameraController::Type::FPS:
		InitializeFPSCamera();
		break;
	}
	
}

void CameraController::InitializeTPSCamera()
{
	
	m_mainCameraTPSDeltaX = 0.0f;
	m_mainCameraTPSDeltaY = 0.0f;
	m_mainCameraDistance = 5.0f;
	m_mainCamera.SetViewLookAt(
		Vector3(2.0f, 2.0f, -2.0f), Vector3::Zero, Vector3::UnitY
	);
	m_mainCamera.SetPerspectiveFieldOfView(
		Mathf::PI / 4.0f,
		(float)DXTK->SwapChain.Width / (float)DXTK->SwapChain.Height,
		0.1f, 10000.0f
	);
}

void CameraController::InitializeFPSCamera()
{
	m_mainCameraFPSDeltaX = 0.0f;
	m_mainCameraFPSDeltaY = 0.0f;
	m_mainCamera.SetViewLookAt(
		Vector3(2.0f, 2.0f, -2.0f), Vector3::Zero, Vector3::UnitY
	);
	m_mainCamera.SetPerspectiveFieldOfView(
		Mathf::PI / 4.0f,
		(float)DXTK->SwapChain.Width / (float)DXTK->SwapChain.Height,
		0.1f, 10000.0f
	);
}

void CameraController::UpdateCamera(const float deltaTime)
{

	if (InputSystem.Mouse.was.middleButton) {

		auto mouseState = InputSystem.Mouse.mouse->GetState();
		if (mouseState.positionMode != DirectX::Mouse::MODE_RELATIVE) {
			InputSystem.Mouse.mouse->SetMode(DirectX::Mouse::MODE_RELATIVE);
			return;
		}
	}
	else {
		InputSystem.Mouse.mouse->SetMode(DirectX::Mouse::MODE_ABSOLUTE);
		return;
	}

	//char debugText[256];
	//sprintf_s(debugText, "DeltaX: %f\n", m_mainCamera.GetForwardVector());
	//OutputDebugStringA(debugText);

	switch (m_type)
	{
	case CameraController::Type::TPS:
		UpdateTPSCamera();
		break;
	case CameraController::Type::FPS:
		UpdateFPSCamera();
		UpdateMoveCamera();
		break;
	default:
		break;
	}
}

void CameraController::UpdateTPSCamera()
{
	float mouseDeltaX = static_cast<float>(InputSystem.Mouse.position.x / 300.0f);
	float mouseDeltaY = static_cast<float>(InputSystem.Mouse.position.y / 300.0f);
	m_mainCameraTPSDeltaX += mouseDeltaX * -1.0f;

	if (mouseDeltaY > 0.0f && m_mainCameraTPSDeltaY < 1.4f) {
		m_mainCameraTPSDeltaY += mouseDeltaY;
	}
	else if (mouseDeltaY < 0.0f && m_mainCameraTPSDeltaY > -1.4f) {
		m_mainCameraTPSDeltaY += mouseDeltaY;
	}

	float camera_distance = m_mainCameraDistance * cos(m_mainCameraTPSDeltaY);
	float camera_y = m_mainCameraDistance * sin(m_mainCameraTPSDeltaY);
	float camera_x = camera_distance * cos(m_mainCameraTPSDeltaX);
	float camera_z = camera_distance * sin(m_mainCameraTPSDeltaX);

	m_mainCameraPosition = Vector3(camera_x, camera_y, camera_z);
	m_mainCamera.SetViewLookAt(
		m_mainCameraPosition, Vector3::Zero, Vector3::UnitY
	);
}

void CameraController::UpdateFPSCamera()
{
	float mouseDeltaX = static_cast<float>(InputSystem.Mouse.position.x / 300.0f);
	float mouseDeltaY = static_cast<float>(InputSystem.Mouse.position.y / 300.0f);

	m_mainCameraFPSDeltaX += mouseDeltaX;
	m_mainCameraFPSDeltaY += mouseDeltaY;

	m_mainCameraFPSDeltaY = std::max(-1.5f, std::min(1.5f, m_mainCameraFPSDeltaY));

	m_mainCamera.SetRotation(m_mainCameraFPSDeltaY, m_mainCameraFPSDeltaX, 0.0f);
}

void CameraController::UpdateMoveCamera()
{
	if (InputSystem.Keyboard.isPressed.W)
	{
		m_mainCamera.Move(0.0f, 0.0f, 0.1f);
	}
	if (InputSystem.Keyboard.isPressed.S)
	{
		m_mainCamera.Move(0.0f, 0.0f, -0.1f);
	}
	if (InputSystem.Keyboard.isPressed.A)
	{
		m_mainCamera.Move(-0.1f, 0.0f, 0.0f);
	}
	if (InputSystem.Keyboard.isPressed.D)
	{
		m_mainCamera.Move(0.1f, 0.0f, 0.0f);
	}
}

/// <summary>
///	ÉJÉÅÉâÇÃéÌóﬁÇêÿÇËë÷Ç¶ÇÈ
/// </summary>
/// <param name="type">ÉJÉÅÉâÇÃéÌóﬁ</param>
void CameraController::SetCameraMode(const Type type)
{
	m_type = type;
	switch (m_type)
	{
	case CameraController::Type::TPS:
		InitializeCamera();
		break;
	case CameraController::Type::FPS:
		InitializeCamera();
		break;
	default:
		break;
	}
}