#include "PlayerInput.h"

void PlayerInput::Initialize(HWND window, int width, int height)
{
    m_mouse = std::make_unique<DirectX::Mouse>();

    m_mouse->SetWindow(window);
}