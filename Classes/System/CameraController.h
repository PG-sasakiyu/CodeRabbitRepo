///
/// CameraController.h
///

#pragma once

#include "..\MainProject\Base\pch.h"
#include "..\MainProject\Base\dxtk.h"
#include "..\MainProject\Base\GameBase.h"

using namespace DirectX;
using namespace SimpleMath;


class CameraController {
public:
	enum class Type {
		TPS,
		FPS
	};
	Type GetType() const { return m_type; }
	void SetType(const Type type) { m_type = type; }

	void InitializeCamera();
	void UpdateCamera(const float deltaTime);

	void SetCameraMode(const Type type);

	DirectXTK::Camera GetCamera() const { return m_mainCamera; }

protected:

	Type							m_type;

	Mouse::ButtonStateTracker		m_mouseTracker;

	DirectXTK::Camera               m_mainCamera;

	Vector3							m_mainCameraPosition;
	Quaternion						m_mainCameraRotation;

	float							m_mainCameraTPSDeltaX;
	float							m_mainCameraTPSDeltaY;
	float							m_mainCameraDistance;

	float							m_mainCameraFPSDeltaX;
	float							m_mainCameraFPSDeltaY;

	Mouse::ButtonStateTracker		mouse_tracker_;


private:
	void InitializeTPSCamera();
	void InitializeFPSCamera();

	void UpdateTPSCamera();
	void UpdateFPSCamera();
	void UpdateMoveCamera();
};