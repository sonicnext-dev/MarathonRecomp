#pragma once

namespace Sonicteam::SoX::Physics
{
    enum ShapeType : uint32_t
    {
        ShapeType_Sphere,
        ShapeType_Box,
        ShapeType_Capsule,
        ShapeType_Cylinder,
        ShapeType_Mesh,
        ShapeType_Convex,
        ShapeType_Other
    };
}
