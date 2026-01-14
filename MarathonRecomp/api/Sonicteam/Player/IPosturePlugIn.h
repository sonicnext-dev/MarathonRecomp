#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/RefCountObject.h>
#include <Sonicteam/SoX/RefSharedPointer.h>
#include <Sonicteam/SoX/Math/Vector.h>

namespace Sonicteam::Player
{
    class IPosturePlugIn
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpDestroy;
            be<uint32_t> fpFunc4;
            be<uint32_t> fpSetControllerMovement;
            be<uint32_t> fpIsInContact;
            be<uint32_t> fpIsOffContact;
            be<uint32_t> fpGetContactFlag;
            be<uint32_t> fpOutContactPosition;
            be<uint32_t> fpOutContactUP;
            be<uint32_t> fpFunc20;
            be<uint32_t> fpFunc24;
            be<uint32_t> fpFunc28;
        };
        xpointer<Vftable> m_pVftable;

        bool SetControllerMovement(SoX::RefSharedPointer<SoX::RefCountObject>* RefObj, SoX::Math::Quaternion* v);

        bool SetControllerMovement(SoX::RefSharedPointer<SoX::RefCountObject>& RefObj, SoX::Math::Quaternion& v);

        bool IsInContact();

        bool IsOffContact();

        uint32_t GetContactFlag();

        void OutContactPosition(SoX::Math::Vector* vector);

        void OutContactUP(SoX::Math::Vector* quat);

        uint32_t Func20(SoX::Math::Vector* impulseForward, SoX::Math::Vector* impulseVertical, SoX::Math::Vector* impulseForward2, SoX::Math::Vector* impulse, double delta);

        uint32_t Func24(SoX::Math::Vector* vector, SoX::Math::Quaternion* quat);

        void Func28(SoX::Math::Vector* v1, SoX::Math::Quaternion* n1);
    };

    MARATHON_ASSERT_OFFSETOF(IPosturePlugIn, m_pVftable, 0x00);
    MARATHON_ASSERT_SIZEOF(IPosturePlugIn, 4);
}

#include "IPosturePlugIn.inl"
