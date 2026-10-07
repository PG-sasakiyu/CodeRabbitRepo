//
// pch.h
// Header for standard system include files.
//

#pragma once

#include <winsdkver.h>
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include <sdkddkver.h>

// Use the C++ standard templated min/max
#define NOMINMAX

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// Include <mcx.h> if you need this
#define NOMCX

// Include <winsvc.h> if you need this
#define NOSERVICE

// WinHelp is deprecated
#define NOHELP

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wrl/client.h>
#include <wrl/event.h>

#ifdef USING_DIRECTX_HEADERS
#include <directx/dxgiformat.h>
#include <directx/d3d12.h>
#else
#include <d3d12.h>
#endif

#include <dxgi1_6.h>

#include <DirectXMath.h>
#include <DirectXColors.h>

#define D3DX12_NO_STATE_OBJECT_HELPERS
#define D3DX12_NO_CHECK_FEATURE_SUPPORT_CLASS
#include "DX\d3dx12.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <exception>
#include <memory>
#include <stdexcept>

#include <pix3.h>

#ifdef _DEBUG
    #define _CRTDBG_MAP_ALLOC
    #include <crtdbg.h>
    #include <dxgidebug.h>
#endif

#include "BufferHelpers.h"
#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "DescriptorHeap.h"
#include "DirectXHelpers.h"
#include "EffectPipelineStateDescription.h"
#include "Effects.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Model.h"
#include "PostProcess.h"
#include "PrimitiveBatch.h"
#include "RenderTargetState.h"
#include "ResourceUploadBatch.h"
#include "ScreenGrab.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "WICTextureLoader.h"

#include "SimpleMath.h"

//#include "Keyboard.h"
//#include "Mouse.h"
//#include "GamePad.h"
#include "Toolkits\InputSystem.h"

#include "Audio.h"

// #define DML_TARGET_VERSION_USE_LATEST
// #include <DirectML.h> // The DirectML header from the Windows SDK.
// #include "DX\DirectMLX.h"

#include <omp.h>

//#ifdef _DEBUG
//    #undef _DEBUG
//    #define NDEBUG
//
//    #include <amp.h>
//
//    #undef NDEBUG
//    #define _DEBUG
//#else
//    #include <amp.h>
//#endif

namespace DX
{
    inline void ThrowIfFailed(HRESULT hr)
    {
        if (FAILED(hr))
        {
#ifdef _DEBUG
            TCHAR err_msg[64]{};
            _stprintf_s(err_msg, _T("ERROR: Failure with HRESULT of %08X: "), (UINT)hr);
            OutputDebugString(err_msg);

            LPVOID string;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                reinterpret_cast<LPTSTR>(&string), 0, nullptr);
            OutputDebugString(reinterpret_cast<LPTSTR>(string));
            LocalFree(string);
#endif
            // Set a breakpoint on this line to catch DirectX API errors
            throw std::exception();
        }
    }
}

#include "Toolkits\DXTKMathematics.h"
#include "Toolkits\DXTKHelpers.h"
#include "Toolkits\DXTKCamera.h"
