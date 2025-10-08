#pragma once

namespace Sonicteam::Player
{
    // Recreated from sub_822014B8
    inline Player::GroundListener* IPostureSupportSphere::sub_822014B8(SoX::Math::Vector point)
    {
        if (!this->m_spGroundListener.get())
            return 0;

        auto v1ColAngle = point + m_CollisionAllAngle;
        m_spGroundListener->SetPostureRequestFlag(this->m_pPostureRequestFlag.get()->get());

        auto pGroundListener = this->m_spGroundListener.get();
        auto pGroundShapeCastListener = static_cast<SoX::Physics::ShapeCastListener*>(pGroundListener);
        auto pShapeCollisionWalk = m_aShapeCollisionWalk[m_FlagC8.get()].get();

        struct Vec2
        {
            SoX::Math::Vector _vec1;
            SoX::Math::Vector _vec2;
        };
        guest_stack_var<Vec2> gsv_vector =
        {
            v1ColAngle,
            point
        };

        pShapeCollisionWalk->Func38((SoX::Math::Vector*)gsv_vector.get(), pGroundShapeCastListener);
        this->m_spGroundListener->Update();
        if (this->m_spGroundListener->GetCurrentSurfaceContacts() > 0)
            return this->m_spGroundListener.get();
        else
            return 0;

    }

    // Recreated from sub_82202180
    inline ICollisionListener* IPostureSupportSphere::sub_82202180(SoX::Math::Vector position, SoX::Math::Vector endPosition, be<uint32_t>* flag)
    {
        if (!m_spWallListener.get())
            return 0;

        m_spWallListener->SetPostureRequestFlag(m_pPostureRequestFlag->get());

        auto collisionWallAnglePosition = position + m_CollisionWallAngle;
        auto collisionWallAngleEndPosition = endPosition + m_CollisionWallAngle;
        struct _vector_
        {
            SoX::Math::Vector p1;
            SoX::Math::Vector p2;
        };
        guest_stack_var< _vector_> points =
        {
            collisionWallAnglePosition,
            collisionWallAngleEndPosition
        };

        auto shapeCollisionWall = m_aShapeCollisionWall[m_FlagC8];
        shapeCollisionWall->Func38((SoX::Math::Vector*)points.get(), static_cast<SoX::Physics::ShapeCastListener*>(m_spWallListener.get()));

        if (flag)
            *flag = m_spWallListener->GetCollisionFlag();

        if (m_spWallListener->GetCurrentSurfaceContacts() > 0)
            return m_spWallListener.get();

        return 0;
    }

    // Recreated from sub_82200EC0
    inline void IPostureSupportSphere::sub_82200EC0(SoX::Math::Vector invertNormal)
    {
        SoX::Math::Vector normal = invertNormal.InvertNormal();


        m_CollisionAllAngle = normal * m_aCollisionAllRadius[m_FlagC8];
        m_CollisionWalkAngle = normal * m_aCollisionWalkRadius[m_FlagC8];
        m_CollisionWallAngle = normal * m_aCollisionWallRadius[m_FlagC8];

        if (m_spGroundListener.get())
            m_spGroundListener->m_CollisionNormal = normal;

        if (m_spWallListener.get())
            m_spWallListener->m_CollisionNormal = normal;

        if (m_spGroundRayListener.get())
            m_spGroundRayListener->m_CollisionNormal = normal;

        if (m_spRealGroundRayListener.get())
            m_spRealGroundRayListener->m_CollisionNormal = normal;

        if (m_spWallRayListener.get())
            m_spWallRayListener->m_CollisionNormal = normal;
    }

    // Recreated from sub_822024F0
    inline bool IPostureSupportSphere::sub_822024F0(SoX::Math::Vector& position, SoX::Math::Vector& impulse, be<uint32_t>* postureFlag)
    {
        auto listener = this->sub_82202180(position, position + impulse, 0);
        if (listener && (listener->GetCollisionFlag() & 1) != 0)
        {
            auto nImpulse = impulse.NormalizeEX();
            impulse = nImpulse.first;
            auto norm_14 = listener->GetLength(nImpulse.second);
            auto invertNormal = listener->GetContactNormal().InvertNormal();
            if (impulse.Dot(invertNormal).X >= 0.89999998)
                *postureFlag = postureFlag->get() | 0x10; //PostureFlag_HeadOnWall

            position = position + (impulse * norm_14);
            impulse = SoX::Math::Vector::Zero();
            *postureFlag = postureFlag->get() | 8; //PostureFlag_WallBrushing
            return true;
        }
        return false;
    }

    // Recreated from sub_82202300
    inline bool IPostureSupportSphere::sub_82202300(SoX::Math::Vector& position, SoX::Math::Vector& impulse, be<uint32_t>* postureFlag)
    {
        auto listener = this->sub_82202180(position, position + impulse, 0);
        if (listener && (listener->GetCollisionFlag() & 1) != 0)
        {
            auto nImpulse = impulse.NormalizeEX();
            impulse = nImpulse.first;
            auto norm_14 = listener->GetLength(nImpulse.second);
            auto invertNormal = listener->GetContactNormal().InvertNormal();
            if (impulse.Dot(invertNormal).X >= 0.89999998)
                *postureFlag = postureFlag->get() | 0x10; //PostureFlag_HeadOnWall

            position = position + (impulse * norm_14);

            auto v24 = invertNormal.Cross(impulse);
            auto v25 = v24.Cross(invertNormal);

            impulse = v25 * (nImpulse.second - norm_14);

            *postureFlag = postureFlag->get() | 8; //PostureFlag_WallBrushing
            return true;
        }
        return false;
    }

    // Recreated from sub_82201058
    inline ICollisionListener* IPostureSupportSphere::sub_82201058(SoX::Math::Vector position, SoX::Math::Vector endPosition, be<uint32_t>* flag)
    {
        if (!m_spGroundListener.get())
            return 0;

        auto collisionWalkAnglePosition = position + m_CollisionWalkAngle;
        auto collisionWalkAngleEndPosition = endPosition + m_CollisionWalkAngle;

        m_spGroundListener->SetPostureRequestFlag(m_pPostureRequestFlag->get());

        struct _vector_
        {
            SoX::Math::Vector p1;
            SoX::Math::Vector p2;
        };
        guest_heap_var<_vector_> points(collisionWalkAnglePosition, collisionWalkAngleEndPosition);

        auto shapeCollisionWalk = m_aShapeCollisionWalk[m_FlagC8];
        shapeCollisionWalk->Func38((SoX::Math::Vector*)points.get(), static_cast<SoX::Physics::ShapeCastListener*>(m_spGroundListener.get()));
        m_spGroundListener->Update();

        if (flag)
            *flag = m_spGroundListener->GetCollisionFlag();

        if (m_spGroundListener->GetCurrentSurfaceContacts() > 0)
            return m_spGroundListener.get();

        return 0;
    }

    // Recreated from sub_822011E8
    inline ICollisionListener* IPostureSupportSphere::sub_822011E8(SoX::Math::Vector d1, SoX::Math::Vector d2, be<uint32_t>* outFlag1)
    {
        if (!m_spGroundRayListener.get())
            return 0;


        m_spGroundRayListener->SetPostureRequestFlag(m_pPostureRequestFlag->get());

        struct _vector_
        {
            SoX::Math::Vector p1;
            SoX::Math::Vector p2;
        };
        guest_heap_var<_vector_> points(d1, d2);

        m_RayCast->Func14((SoX::Math::Vector*)points.get(), static_cast<SoX::Physics::ShapeCastListener*>(m_spGroundRayListener.get()));
        m_spGroundRayListener->Update();

        if (outFlag1)
            *outFlag1 = m_spGroundRayListener->GetCollisionFlag();

        if (m_spGroundRayListener->GetCurrentSurfaceContacts() > 0)
            return m_spGroundRayListener.get();

        return 0;
    }

    // Recreated from sub_82203840
    inline ICollisionListener* IPostureSupportSphere::sub_82203840(SoX::Math::Vector* d1, SoX::Math::Vector d2, be<uint32_t>* outFlag1)
    {
        if (d1)
        {
            *d1 = d2;
        }

        if (auto listener = m_spAllListener1.get())
        {
            printf("listener %p\n", listener);
            auto v22 = d2 + m_CollisionAllAngle;

            struct _vector_
            {
                SoX::Math::Vector p1;
                SoX::Math::Vector p2;
            };
            guest_heap_var<_vector_> points(v22, v22);
            listener->m_Position = v22;
            listener->m_Radius = m_aCollisionAllRadius[m_FlagC8];
            listener->SetPostureRequestFlag(m_pPostureRequestFlag.get()->get());

            m_aShapeCollisionAll[m_FlagC8]->Func3C((SoX::Math::Vector*)points.get(), static_cast<SoX::Physics::ShapeCastListener*>(listener));
            listener->Update();

            if (outFlag1)
                *outFlag1 = listener->GetCollisionFlag();

            if (d1)
            {
                *d1 = listener->GetContactPosition() - m_CollisionAllAngle;
            }

            if (listener->GetCurrentSurfaceContacts() > 0)
                return listener;

        }
        return 0;
    }

    // Recreated from sub_82201350
    inline ICollisionListener* IPostureSupportSphere::sub_82201350(SoX::Math::Vector& pos1, SoX::Math::Vector& pos2, be<uint32_t>* outFlag1)
    {
        if (!m_spWallRayListener.get())
            return 0;


        m_spWallRayListener->SetPostureRequestFlag(m_pPostureRequestFlag->get());

        struct _vector_
        {
            SoX::Math::Vector p1;
            SoX::Math::Vector p2;
        };
        guest_heap_var<_vector_> points(pos1, pos2);

        m_RayCast->Func14((SoX::Math::Vector*)points.get(), static_cast<SoX::Physics::ShapeCastListener*>(m_spWallRayListener.get()));
        m_spWallRayListener->Update();

        if (outFlag1)
            *outFlag1 = m_spWallRayListener->GetCollisionFlag();

        if (m_spWallRayListener->GetCurrentSurfaceContacts() > 0)
            return m_spWallRayListener.get();

        return 0;
    }
};

