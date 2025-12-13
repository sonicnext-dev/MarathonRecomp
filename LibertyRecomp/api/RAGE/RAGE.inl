#pragma once

// =============================================================================
// RAGE Engine Inline Implementations
// =============================================================================
// This file contains inline implementations for RAGE engine structures.
// Include after RAGE.h
// =============================================================================

#include "RAGE.h"
#include <cmath>

namespace rage
{
    // =========================================================================
    // Vector3 Operations
    // =========================================================================

    inline Vector3 operator+(const Vector3& a, const Vector3& b)
    {
        return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
    }

    inline Vector3 operator-(const Vector3& a, const Vector3& b)
    {
        return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
    }

    inline Vector3 operator*(const Vector3& v, f32 scalar)
    {
        return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
    }

    inline Vector3 operator*(f32 scalar, const Vector3& v)
    {
        return Vector3(v.x * scalar, v.y * scalar, v.z * scalar);
    }

    inline Vector3 operator/(const Vector3& v, f32 scalar)
    {
        return Vector3(v.x / scalar, v.y / scalar, v.z / scalar);
    }

    inline f32 Dot(const Vector3& a, const Vector3& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    inline Vector3 Cross(const Vector3& a, const Vector3& b)
    {
        return Vector3(
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        );
    }

    inline f32 Length(const Vector3& v)
    {
        return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    }

    inline f32 LengthSquared(const Vector3& v)
    {
        return v.x * v.x + v.y * v.y + v.z * v.z;
    }

    inline Vector3 Normalize(const Vector3& v)
    {
        f32 len = Length(v);
        if (len > 0.0001f)
            return v / len;
        return Vector3();
    }

    inline f32 Distance(const Vector3& a, const Vector3& b)
    {
        return Length(a - b);
    }

    inline f32 DistanceSquared(const Vector3& a, const Vector3& b)
    {
        return LengthSquared(a - b);
    }

    // =========================================================================
    // Vector4 Operations
    // =========================================================================

    inline Vector4 operator+(const Vector4& a, const Vector4& b)
    {
        return Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
    }

    inline Vector4 operator-(const Vector4& a, const Vector4& b)
    {
        return Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
    }

    inline Vector4 operator*(const Vector4& v, f32 scalar)
    {
        return Vector4(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar);
    }

    inline f32 Dot(const Vector4& a, const Vector4& b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    // =========================================================================
    // Matrix Operations
    // =========================================================================

    inline Matrix34 MatrixIdentity()
    {
        Matrix34 m;
        m.right   = Vector4(1, 0, 0, 0);
        m.forward = Vector4(0, 1, 0, 0);
        m.up      = Vector4(0, 0, 1, 0);
        m.pos     = Vector4(0, 0, 0, 1);
        return m;
    }

    inline Vector3 MatrixGetPosition(const Matrix34& m)
    {
        return Vector3(m.pos.x, m.pos.y, m.pos.z);
    }

    inline void MatrixSetPosition(Matrix34& m, const Vector3& pos)
    {
        m.pos.x = pos.x;
        m.pos.y = pos.y;
        m.pos.z = pos.z;
    }

    inline Vector3 MatrixGetRight(const Matrix34& m)
    {
        return Vector3(m.right.x, m.right.y, m.right.z);
    }

    inline Vector3 MatrixGetForward(const Matrix34& m)
    {
        return Vector3(m.forward.x, m.forward.y, m.forward.z);
    }

    inline Vector3 MatrixGetUp(const Matrix34& m)
    {
        return Vector3(m.up.x, m.up.y, m.up.z);
    }

    inline Vector3 TransformPoint(const Matrix34& m, const Vector3& p)
    {
        return Vector3(
            m.right.x * p.x + m.forward.x * p.y + m.up.x * p.z + m.pos.x,
            m.right.y * p.x + m.forward.y * p.y + m.up.y * p.z + m.pos.y,
            m.right.z * p.x + m.forward.z * p.y + m.up.z * p.z + m.pos.z
        );
    }

    inline Vector3 TransformVector(const Matrix34& m, const Vector3& v)
    {
        return Vector3(
            m.right.x * v.x + m.forward.x * v.y + m.up.x * v.z,
            m.right.y * v.x + m.forward.y * v.y + m.up.y * v.z,
            m.right.z * v.x + m.forward.z * v.y + m.up.z * v.z
        );
    }

} // namespace rage

namespace GTA4
{
    // =========================================================================
    // CTimer Implementation
    // =========================================================================

    inline void CTimer::Update()
    {
        m_frameCounter++;
        // Actual timing handled by the game loop
    }

    // =========================================================================
    // Pool Helper Functions
    // =========================================================================

    template<typename T>
    inline T* GetPoolObject(CPool<T>* pool, u32 handle)
    {
        if (!pool) return nullptr;
        u32 index = handle & 0xFFFF;
        return pool->GetAt(index);
    }

    inline CPed* GetPedFromHandle(u32 handle)
    {
        return GetPoolObject(g_pedPool, handle);
    }

    inline CVehicle* GetVehicleFromHandle(u32 handle)
    {
        return GetPoolObject(g_vehiclePool, handle);
    }

    inline CObject* GetObjectFromHandle(u32 handle)
    {
        return GetPoolObject(g_objectPool, handle);
    }

} // namespace GTA4
