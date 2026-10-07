///
/// Dummy.h
///
/// 

#pragma once

#include "..\MainProject\Base\pch.h"
#include "..\MainProject\Base\dxtk.h"

using namespace DirectX;

class PlayerInput {
public:

	void Initialize(HWND window, int width, int height);


protected:

    std::unique_ptr<DirectX::Mouse>         m_mouse;
    DirectX::Mouse::ButtonStateTracker      m_mouseTracker;
};