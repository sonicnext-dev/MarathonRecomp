#pragma once

#include <hk330/hkArray.h>
#include <hk330/hkConvexShape.h>
#include <Sonicteam/SoX/Math/Math.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace hk330
{
    class hkConvexVerticesShape : public hkConvexShape
    {
    public:
        struct FourVectors
        {
            enum FourVectorsAxis
            {
                FourVectorsAxis_X,
                FourVectorsAxis_Y,
                FourVectorsAxis_Z
            };

            Sonicteam::SoX::Math::Vector m_x;
            Sonicteam::SoX::Math::Vector m_y;
            Sonicteam::SoX::Math::Vector m_z;

            float getAxis(const FourVectorsAxis in_axis, const int in_index)
            {
                Sonicteam::SoX::Math::Vector* pAxis{};

                switch (in_axis)
                {
                    case FourVectorsAxis_X: pAxis = &m_x; break;
                    case FourVectorsAxis_Y: pAxis = &m_y; break;
                    case FourVectorsAxis_Z: pAxis = &m_z; break;
                    
                    default:
                        return 0.0f;
                }

                switch (in_index)
                {
                    case 0: return pAxis->X;
                    case 1: return pAxis->Y;
                    case 2: return pAxis->Z;
                    case 3: return pAxis->W;
                }

                return 0.0f;
            }

            float x(const int in_index)
            {
                return getAxis(FourVectorsAxis_X, in_index);
            }

            float y(const int in_index)
            {
                return getAxis(FourVectorsAxis_Y, in_index);
            }

            float z(const int in_index)
            {
                return getAxis(FourVectorsAxis_Z, in_index);
            }
        };

        Sonicteam::SoX::Math::Vector m_aabbHalfExtents;
        Sonicteam::SoX::Math::Vector m_aabbCenter;
        hkArray<FourVectors> m_rotatedVertices;
        be<int32_t> m_numVertices;
        hkArray<Sonicteam::SoX::Math::Vector> m_planeEquations;

        // Reference: https://www.reddit.com/r/godot/comments/t4ly0g/comment/hz380ef
        bool hasPoint(const Sonicteam::SoX::Math::Vector& in_rPoint)
        {
            constexpr auto meter = Sonicteam::SoX::Math::Meter;

            // Check if point is inside AABB first.
            if (std::abs(in_rPoint.X - m_aabbCenter.X * meter) > m_aabbHalfExtents.X * meter ||
                std::abs(in_rPoint.Y - m_aabbCenter.Y * meter) > m_aabbHalfExtents.Y * meter ||
                std::abs(in_rPoint.Z - m_aabbCenter.Z * meter) > m_aabbHalfExtents.Z * meter)
            {
                return false;
            }

            for (const auto& rPlane : m_planeEquations)
            {
                if (in_rPoint.Dot(rPlane) + rPlane.W > 0.0f)
                    return false;
            }

            return true;
        }
    };

    MARATHON_ASSERT_OFFSETOF(hkConvexVerticesShape::FourVectors, m_x, 0x00);
    MARATHON_ASSERT_OFFSETOF(hkConvexVerticesShape::FourVectors, m_y, 0x10);
    MARATHON_ASSERT_OFFSETOF(hkConvexVerticesShape::FourVectors, m_z, 0x20);
    MARATHON_ASSERT_SIZEOF(hkConvexVerticesShape::FourVectors, 0x30);

    MARATHON_ASSERT_OFFSETOF(hkConvexVerticesShape, m_aabbHalfExtents, 0x10);
    MARATHON_ASSERT_OFFSETOF(hkConvexVerticesShape, m_aabbCenter, 0x20);
    MARATHON_ASSERT_OFFSETOF(hkConvexVerticesShape, m_rotatedVertices, 0x30);
    MARATHON_ASSERT_OFFSETOF(hkConvexVerticesShape, m_numVertices, 0x3C);
    MARATHON_ASSERT_OFFSETOF(hkConvexVerticesShape, m_planeEquations, 0x40);
    MARATHON_ASSERT_SIZEOF(hkConvexVerticesShape, 0x50);
}
