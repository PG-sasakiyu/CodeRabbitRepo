///
/// PrimitiveManager.cpp
///
#include "PrimitiveManager.h"
#include "RenderTargetState.h"
#include "EffectPipelineStateDescription.h"

void PrimitiveManager::CreateDeviceDependentResources()
{
    auto&& device = DXTK->Device;

    DirectX::RenderTargetState rtState(DXTK->SwapChain.Format, DXTK->SwapChain.DepthFormat);
    DirectX::EffectPipelineStateDescription pd(
        &DirectX::GeometricPrimitive::VertexType::InputLayout,
        DirectX::CommonStates::Opaque,
        DirectX::CommonStates::DepthDefault,
        DirectX::CommonStates::CullCounterClockwise,
        rtState
    );

    m_effect = std::make_shared<DirectX::BasicEffect>(device, DirectX::EffectFlags::Texture, pd);

    GetPrimitive(PrimitiveType::Cube);
}

DirectX::GeometricPrimitive* PrimitiveManager::GetPrimitive(PrimitiveType type)
{
    auto it = m_primitives.find(type);
    if (it != m_primitives.end())
    {
        return it->second.get();
    }

    auto&& device = DXTK->Device;
    std::unique_ptr<DirectX::GeometricPrimitive> newPrim;

    switch (type)
    {
    case PrimitiveType::Cube:
        newPrim = DirectX::GeometricPrimitive::CreateCube(1.0f, false, device);
        break;

    case PrimitiveType::Sphere:
        newPrim = DirectX::GeometricPrimitive::CreateSphere(1.0f, 16, false, false, device);
        break;

    case PrimitiveType::Cylinder:
        newPrim = DirectX::GeometricPrimitive::CreateCylinder(1.0f, 1.0f, 16, false, device);
        break;

    case PrimitiveType::Cone:
        newPrim = DirectX::GeometricPrimitive::CreateCone(1.0f, 1.0f, 16, false, device);
        break;

    case PrimitiveType::Teapot:
        newPrim = DirectX::GeometricPrimitive::CreateTeapot(1.0f, 8, false, device);
        break;

    default:
        newPrim = DirectX::GeometricPrimitive::CreateCube(1.0f, false, device);
        break;
    }

    auto rawPointer = newPrim.get();
    m_primitives[type] = std::move(newPrim);
    return rawPointer;
}