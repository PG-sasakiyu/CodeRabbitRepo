///
/// PrimitiveManager.h
///
/// 

#pragma once
#include "..\MainProject\Base\pch.h"
#include "..\MainProject\Base\dxtk.h"
#include <GeometricPrimitive.h>
#include <Effects.h>
#include <memory>

enum class PrimitiveType {
    Cube,
    Sphere,
    Cylinder,
    Cone,
    Teapot
};

class PrimitiveManager {
public:
    static PrimitiveManager* GetInstance() {
        static PrimitiveManager instance;
        return &instance;
    }

    PrimitiveManager() = default;
    ~PrimitiveManager() = default;

    void CreateDeviceDependentResources();

    DirectX::GeometricPrimitive* GetPrimitive(PrimitiveType type);
    DirectX::BasicEffect* GetEffect() { return m_effect.get(); }

private:
    std::unordered_map<PrimitiveType, std::unique_ptr<DirectX::GeometricPrimitive>> m_primitives;
    std::shared_ptr<DirectX::BasicEffect> m_effect;
};