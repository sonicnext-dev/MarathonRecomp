#pragma once
#include <Sonicteam/SoX/Physics/Havok/RayCastHavok.h>
#include <Sonicteam/Player/Zock.h>
#include <hk330/hkpBvShape.h>
#include <hk330/hkpShapeSphere.h>
#include <algorithm>
#include <app.h>

namespace Sonicteam::Player
{
    // Recreate from sub_82200538, Full 0.9:1 recreated for debugging purposes
    // Silver doesn't stand on marked objects during Psycho Move
    // Character doesn't stand on rct conveyors
    inline void Sonicteam::Player::PostureControl::RecreatedUpdate(double deltaTime)
    {
        this->m_DisableAccuratePosture = false;

        auto r1_0xB0 = this->m_GroundNormal; //??
        IPostureControl::RecreatedUpdate(deltaTime);

        //Jump Direction
        if ((this->m_PostureFlag118.get() & 8) != 0)
        {
            this->m_PostureFlag118 = this->m_PostureFlag118.get() & ~8;
            this->m_GroundUp = this->m_LocalRotation * Sonicteam::SoX::Math::Vector::Up();
        }

        //m_FlagC8 small, normal collision
        this->m_FlagC8 = (this->m_PostureRequestFlag.get() & 0x4000) != 0; //sub_82200EA8(a1 + 0x120, (*(_DWORD *)(a1 + 0x114) >> 0xE) & 1);
        auto PostureContactID = this->GetPostureContact();
        auto pPosturePath = this->GetPosturePath(PostureContactID);
        auto postureContactFlag = this->OutPostureContactFlag(pPosturePath); //v19 = sub_821F1578(a1, pPosturePath);
        this->m_PostureFlag = postureContactFlag;

        //Impulse Direction
        if (this->m_PostureRequestFlag.get() & 0x40000)
        {
            this->m_ImpulseUP = Sonicteam::SoX::Math::Vector::Up();
        }

        this->OutPostureContactPosition(pPosturePath, &this->m_Position);
        this->OutPostureContactPosition(this->m_spImpulse.get(), &this->m_Position);
        this->OutPostureContactUP(pPosturePath, &this->m_ImpulseUP);
        this->OutPostureContactUP(this->m_spImpulse.get(), &this->m_ImpulseUP);

        auto v0 = this->m_ImpulseUP;

        // Lerp Impulse
        Sonicteam::SoX::Math::Vector r1_0x60 = Sonicteam::SoX::Math::Vector::Zero();
        auto r1_0x50 = this->m_Position;
        auto r1_0x80 = this->m_Position;

        Sonicteam::SoX::Math::Vector r1_0xC0;
        Sonicteam::SoX::Math::Vector r1_0x90;
        Sonicteam::SoX::Math::Vector r1_0x70;

        //forward impulse
        Sonicteam::SoX::Math::Vector r1_0x120;

        Sonicteam::SoX::Math::Vector r1_0xF0;
        Sonicteam::SoX::Math::Vector r1_0xE0;
        Sonicteam::SoX::Math::Vector r1_0x110;
        Sonicteam::SoX::Math::Vector r1_0x1B0;
        Sonicteam::SoX::Math::Vector r1_0x140;
        Sonicteam::SoX::Math::Vector r1_0x170;
        Sonicteam::SoX::Math::Vector r1_0x1C0;
        Sonicteam::SoX::Math::Vector r1_0x190;
        Sonicteam::SoX::Math::Vector r1_0x1A0;
        Sonicteam::SoX::Math::Quaternion r1_0x130;
        Sonicteam::SoX::Math::Quaternion r1_0x160;
        Sonicteam::SoX::Math::Vector r1_0x150;

        //Can QuickRotate
        if (this->sub_82201E78(this->m_Rotation))
            this->m_PostureFlag = this->m_PostureFlag.get() | Sonicteam::Player::PostureControl::PostureFlag_QuickRotate;

        //TODO : redo to not repeat  if (PostureContactID < PostureControl::ContactId_6)
        using Sonicteam::Player::PostureControl;
        switch (PostureContactID)
        {
        case PostureControl::ContactId_Default:
        {
            if ((this->m_PostureRequestFlag.get() & 0x20) != 0)
            {
                this->m_PadRotation = this->sub_82201D88(this->m_Rotation); //IPostureSupportInput
            }
            else
            {
                if ((this->m_ContextIFFlag.get() & 0x80000) != 0)
                {
                    this->sub_82201E00(this->m_Rotation);
                }
                else
                {
                    auto impulseForward = this->m_ImpulseForward.get();
                    if (this->m_RotationSpeedBorder > impulseForward)
                        this->m_Rotation = this->sub_82201D88(this->m_Rotation); //IPostureSupportInput
                    else
                        this->m_Rotation = this->sub_82201BC8(this->m_Rotation, impulseForward);
                }
            }
            this->sub_821F1858(this->m_spImpulse.get(), Sonicteam::SoX::Math::Vector::Zero(), &this->m_Rotation);
            auto v39 = this->m_GroundUp;
            auto v40 = this->m_spPathGuide.get();
            //LABEL_20:
            this->sub_821F1858(v40, v39, &this->m_Rotation);
            //LABEL_21:
            if (PostureContactID < PostureControl::ContactId_6)
            {
                r1_0x140 = this->CalculateVerticalImpulse();
                r1_0xC0 = r1_0x140;
                this->CalculateForwardImpulse(r1_0x120, 0, r1_0x80,deltaTime);
            }
        }
        break;
        //Other Cases
        case PostureControl::ContactId_Path:
        case PostureControl::ContactId_WaterSlider:
        case PostureControl::ContactId_LightDash:
        {
            //LABEL_21:
            if (PostureContactID < PostureControl::ContactId_6)
            {
                r1_0x140 = this->CalculateVerticalImpulse();
                r1_0xC0 = r1_0x140;
                this->CalculateForwardImpulse(r1_0x120, 0, r1_0x80, deltaTime);
            }
        }
        break;
        case PostureControl::ContactId_PathCollision:
        {
            r1_0x1A0 = this->sub_82201BC8(this->m_Rotation, this->m_ImpulseForward);;
            if (PostureContactID < PostureControl::ContactId_6)
            {
                r1_0x140 = this->CalculateVerticalImpulse();
                r1_0xC0 = r1_0x140;
                this->CalculateForwardImpulse(r1_0x120, 0, r1_0x80, deltaTime);
            }

        }
        break;
        case PostureControl::ContactId_Homing:
        {
            r1_0x1C0 = this->sub_82201F28(this->m_Rotation, 0.30000001);
            this->m_Rotation = r1_0x1C0;
            if (PostureContactID < PostureControl::ContactId_6)
            {
                r1_0x140 = this->CalculateVerticalImpulse();
                r1_0xC0 = r1_0x140;
                this->CalculateForwardImpulse(r1_0x120, 0, r1_0x80, deltaTime);
            }
        }
        break;
        case PostureControl::ContactId_6:
        {
            r1_0x130 = this->sub_82201D88(this->m_Rotation);
            Sonicteam::SoX::Math::Vector r1_0x130_forward = r1_0x130 * Sonicteam::SoX::Math::Vector::Forward();
            this->sub_821F1858(pPosturePath, r1_0x130_forward, &this->m_Rotation);
            if (PostureContactID < PostureControl::ContactId_6)
            {
                r1_0x140 = this->CalculateVerticalImpulse();
                r1_0xC0 = r1_0x140;
                this->CalculateForwardImpulse(r1_0x120, 0, r1_0x80, deltaTime);
            }
        }
        break;
        default:
            break;
        }

        if (!PostureContactID)
        {
            this->sub_821FC148(&r1_0x60, r1_0x120, r1_0xC0, deltaTime);
            //Make Sure it works properly
            //this->sub_821F1728(this->m_spMovableBody.get(), r1_0x60, r1_0x120, r1_0x70,deltaTime);
            this->sub_821F1728(this->m_spMovableBody.get(), r1_0x60, r1_0x120, r1_0xC0, deltaTime);
        }
        else if (PostureContactID < 6)
        {       
            this->sub_821F1728(pPosturePath, r1_0x60, r1_0x120, r1_0xC0, deltaTime);
        }

        //i think
        if (this->m_spPhysicsBodyList.get()) //  if ( v51 && sub_82615D60 ) sub_821FD5A0(v51 + 0x20);
            this->m_spPhysicsBodyList->m_vlrBody.clear();

        auto v52 = (this->m_PostureRequestFlag.get() & 0x2000) != 0;
        switch (PostureContactID)
        {
        case PostureControl::ContactId_Default:
        {

            auto r1_0x60_len = r1_0x60.Length3().X.get(); //v53
            auto r1_0x90 = r1_0x60.Normalize3();
            this->m_CurrentPostureContinueNum = 0;
            do
            {
                auto ContinueLen = this->m_PostureContinueLen.get();
                if (r1_0x60_len <= ContinueLen)
                {
                    r1_0xF0 = r1_0x90 * r1_0x60_len;
                    r1_0x60_len = 0.0;
                    r1_0x60 = r1_0xF0;
                }
                else
                {
                    r1_0x110 = r1_0x90 * ContinueLen;
                    r1_0x60_len -= this->m_PostureContinueLen.get();
                    r1_0x60 = r1_0x110;
                }

                //Not Sure Here
                r1_0xE0 = r1_0x50;

                this->m_PostureFlag = this->m_PostureFlag.get() & ~1;
                if ((this->m_PostureFlag.get() & 0x40) != 0)
                {
                    if (this->sub_821FE560(r1_0x50, r1_0x60, &this->m_PostureFlag))
                        v52 = false;
                }
                else
                {
                    if ((this->m_ContextIFFlag.get() & 0x1000) != 0)
                        this->sub_822024F0(r1_0x50, r1_0x60, &this->m_PostureFlag);
                    else
                        this->sub_82202300(r1_0x50, r1_0x60, &this->m_PostureFlag);

                    this->sub_821FF910(r1_0x50, &this->m_GroundNormal, r1_0x60);
                    if (this->sub_82201758(r1_0xE0, r1_0x50))
                        this->m_PostureFlag = this->m_PostureFlag.get() | PostureControl::PostureFlag_OTTOTTO;
                    r1_0x1B0 = r1_0x50 - r1_0x80;
                    this->sub_821FF0F0(r1_0x50, &this->m_GroundNormal, r1_0x1B0);
                    this->m_CurrentPostureContinueNum = this->m_CurrentPostureContinueNum + 1;
                    if (r1_0x60.Length3().X > 0.0)
                    {
                        r1_0x60_len += r1_0x60.Length3().X;
                        r1_0x140 = r1_0x60.Normalize3();
                        r1_0x90 = r1_0x140;
                    }
                }
            } while (((this->m_ContextIFFlag.get() & 0x40) == 0 || (this->m_PostureFlag.get() & 1) == 0)
                && r1_0x60_len > 0.0
                && this->m_CurrentPostureContinueNum < this->m_PostureContinueNum);

            r1_0x170 = r1_0x50 - r1_0x80;
            this->sub_821F18E0(this->m_spImpulse.get(), r1_0x170, &this->m_GroundNormal);
            this->sub_821F18E0(this->m_spPathGuide.get(), this->m_GroundUp, &this->m_GroundNormal);
            this->m_Position = r1_0x50;

            if (!this->m_DisableAccuratePosture)
            {
                be<uint32_t> Flag1;
                auto v69 = this->sub_82203840(&this->m_Position, this->m_Position, &Flag1);
                this->sub_821FEEF0(v69, Flag1);
            }

            if (this->EdgeGrabLeftOverFunction(r1_0x50))
            {
                this->m_PostureFlag = this->m_PostureFlag.get() | 0x80;
                auto pPostureEdge = static_cast<Sonicteam::Player::PostureEdge*>(this->m_spPostureEdge.get());
                pPostureEdge->m_Field54 = 0x80;
                pPostureEdge->m_Field70 = 0;
            }

            if (this->sub_821F17C8(this->m_spImpulse.get()) &&
                this->sub_821F17C8(this->m_spPathGuide.get()) &&
                v52)
            {
                r1_0x1C0 = Sonicteam::SoX::Math::Quaternion::Slerp(r1_0xB0, this->m_GroundNormal, 0.1);
                this->m_GroundNormal = r1_0x1C0;
            }
        }
        break;
        case PostureControl::ContactId_Path:
        case PostureControl::ContactId_WaterSlider:
        case PostureControl::ContactId_PathCollision:
        {
            this->sub_822024F0(r1_0x50, r1_0x60, &this->m_PostureFlag);
            this->sub_821F1858(pPosturePath, r1_0x60, &this->m_Rotation);
            this->sub_821F18E0(pPosturePath, r1_0x60, &this->m_GroundNormal);
            r1_0x50 = r1_0x50 + r1_0x60;
            this->OutPostureContactPosition(pPosturePath, &this->m_Position);
            this->m_GroundNormal = Sonicteam::SoX::Math::Quaternion::Slerp(r1_0xB0, this->m_GroundNormal, 0.1);
        }
        break;
        case PostureControl::ContactId_LightDash:
        {
            this->sub_822024F0(r1_0x50, r1_0x60, &this->m_PostureFlag);
            this->sub_821F1858(pPosturePath, r1_0x60, &this->m_Rotation);
            this->sub_821F18E0(pPosturePath, r1_0x60, &this->m_GroundNormal);
            auto gr1_0x50 = guest_heap_var<Sonicteam::SoX::Math::Vector>(r1_0x50);
            this->OutPostureContactPosition(pPosturePath, gr1_0x50.get());
            r1_0x50 = *gr1_0x50;
            r1_0x160 = r1_0x50 - r1_0x80;
            this->sub_821FF0F0(r1_0x50, &this->m_GroundNormal, r1_0x160);
            this->m_Position = r1_0x50;
            this->m_GroundNormal = Sonicteam::SoX::Math::Quaternion::Slerp(r1_0xB0, this->m_GroundNormal, 0.1);
        }
        break;
        case PostureControl::ContactId_Homing:
        {
            this->sub_821FFC08(r1_0x50, &this->m_GroundNormal, r1_0x60);
            this->sub_821F1858(pPosturePath, r1_0x60, &this->m_Rotation);
            this->sub_821F18E0(pPosturePath, r1_0x60, &this->m_GroundNormal);
            r1_0x50 = r1_0x50 + r1_0x60;
            r1_0x150 = r1_0x50 - r1_0x80;
            this->sub_821FF0F0(r1_0x50, &this->m_GroundNormal, r1_0x150);
            this->m_Position = r1_0x50;
            if (!this->m_DisableAccuratePosture)
            {
                be<uint32_t> Flag1;
                auto v69 = this->sub_82203840(&this->m_Position, this->m_Position, &Flag1);
                this->sub_821FEEF0(v69, Flag1);
            }
            this->m_PostureFlag = this->m_PostureFlag | this->OutPostureContactFlag(pPosturePath);
        }
        break;
        case PostureControl::ContactId_PostureVehicle:
        case PostureControl::ContactId_PostureRodeo:
        {
            this->sub_821F1858(pPosturePath, r1_0x60, &this->m_Rotation);
        }
        break;
        default:
            break;
        }
        r1_0x190 = this->m_Position - r1_0x80;
        guest_heap_var<Sonicteam::SoX::Math::Vector> gr1_0x190(r1_0x190);
        this->OutPostureContactPosition(this->m_spPathGuide.get(), gr1_0x190.get());
        this->sub_821FDDB0();
        return;
    }

    inline RotationGravityAxis* PostureControl::GetRotationGravityAxis()
    {
        return reinterpret_cast<RotationGravityAxis*>(this->m_spRotationGravityAxis.get());
    }

    inline MovableBody* PostureControl::GetMovableBody()
    {
        return reinterpret_cast<MovableBody*>(this->m_spMovableBody.get());
    }

    inline uint32_t PostureControl::GetPostureContact()
    {
        if (m_spPath.get() && m_spPath->IsInContact())
            return 1;

        if (m_spWaterSlider.get() && m_spWaterSlider->IsInContact())
            return 2;

        if (m_spPathLightDash.get() && m_spPathLightDash->IsInContact())
            return 3;

        if (m_spPathCollision.get() && m_spPathCollision->IsInContact())
            return 4;

        if (m_spHoming.get() && m_spHoming->IsInContact())
            return 5;

        if (m_spPostureVehicle.get() && m_spPostureVehicle->IsInContact())
            return 8;

        if (m_spPostureRodeo.get() && m_spPostureRodeo->IsInContact())
            return 9;
        else
            return (this->m_ContextIFFlag.get() & 0x40000) == 0 ? 0 : 6;
    }

    //ref to sub_821FDC28
    template <typename Type>
    inline Type* PostureControl::GetPosturePath(uint32_t path)
    {
        switch (path)
        {
        case 1:
            return static_cast<Type*>(m_spPath.get());
        case 2:
            return static_cast<Type*>(m_spWaterSlider.get());
        case 3:
            return static_cast<Type*>(m_spPathLightDash.get());
        case 4:
            return static_cast<Type*>(m_spPathCollision.get());
        case 5:
            return static_cast<Type*>(m_spHoming.get());
        case 6:
            return static_cast<Type*>(m_spPostureEdge.get());
        case 8:
            return static_cast<Type*>(m_spPostureVehicle.get());
        case 9:
            return static_cast<Type*>(m_spPostureRodeo.get());
        default:
            return static_cast<Type*>(m_spPosturePlugIn.get());
        }
    }

    //sub_821F1578
    inline uint32_t PostureControl::OutPostureContactFlag(IPosturePlugIn* plugin)
    {
        if (plugin && plugin->IsInContact())
            return plugin->GetContactFlag();

        return false;
    }

    //sub_821F1608
    inline bool PostureControl::OutPostureContactPosition(IPosturePlugIn* plugin, SoX::Math::Vector* position)
    {
        if (plugin && plugin->IsInContact())
        {
            plugin->OutContactPosition(position);
            return true;
        }

        return false;
    }

    //sub_821F1698
    inline bool PostureControl::OutPostureContactUP(IPosturePlugIn* plugin, SoX::Math::Vector* position)
    {
        if (plugin && plugin->IsInContact())
        {
           plugin->OutContactUP(position);
           return true;
        }

        return false;
    }

    inline bool PostureControl::sub_821F1858(IPosturePlugIn* plugin, SoX::Math::Vector vector, SoX::Math::Quaternion* quat)
    {
        if (plugin && plugin->IsInContact())
            return plugin->Func24(guest_heap_var<SoX::Math::Vector>(vector).get(), quat);

        return false;
    }

    //ref to sub_821FDCD0
    inline SoX::Math::Vector PostureControl::CalculateVerticalImpulse()
    {
        SoX::Math::Vector vector;
        auto velocityImpulse = this->m_ImpulseUP * this->m_ImpulseVertical.get();
        auto velocityGravity = this->m_GravityDirection * this->m_GravityForce.get();
        auto Velocity = velocityImpulse + velocityGravity;
        vector = Velocity;
        if (this->m_ImpulseVertical.get() == 0.0 && this->m_GravityForce.get() == 0.0)
            return vector;

        auto usableImpulse = this->m_ImpulseVertical.get() - this->m_GravityForce.get();

        if (-200.0 <= usableImpulse && usableImpulse <= 200.0)
        {
            this->m_PostureFlag = this->m_PostureFlag.get() | PostureFlag_FallIntermediate;
            return vector;
        }
        if (usableImpulse < 0.0)
        {
            this->m_PostureFlag = this->m_PostureFlag.get() | PostureFlag_Fall;
        }
        return vector;
    }

    //done, probabl it works fine
    inline bool PostureControl::sub_821F1410(uint32_t Flag1)
    {
        bool result = (Flag1 & 8) != 0;
        if ((Flag1 & 0x40) != 0 && (this->m_PostureRequestFlag.get() & 8) != 0)
            result = 1;
        if ((Flag1 & 0x100) != 0 && (this->m_PostureRequestFlag.get() & 0x200) != 0)
            result = 1;
        if ((Flag1 & 1) != 0 && (this->m_ContextIFFlag.get() & 0x200000) != 0)
            return 1;

        return result;
    }

    //sub_821FE3A0, --done--
    inline bool PostureControl::CalculateForwardImpulse(SoX::Math::Vector& outForwardImpulse, uint32_t lostArgument, SoX::Math::Vector point,double delta)
    {
        this->m_VectorA0 = SoX::Math::Quaternion::Identity();
        auto v12 = this->sub_822014B8(point);
        if (!v12)
        {
            outForwardImpulse = (this->m_Rotation * (SoX::Math::Vector::Forward() * this->m_ImpulseForward.get()));
        }
        else
        {
            auto Flag1 = v12->GetCollisionFlag();
            if (!this->sub_821F1410(Flag1))
            {
                outForwardImpulse = (this->m_Rotation * (SoX::Math::Vector::Forward() * this->m_ImpulseForward.get()));
            }
            else
            {
                auto normal = v12->GetContactNormal();
                outForwardImpulse = (this->m_LocalRotation * (SoX::Math::Vector::Forward() * this->m_ImpulseForward.get()));
                m_VectorA0 = SoX::Math::Quaternion::Identity();
            }
        }

        if (((this->m_PostureRequestFlag.get() & 0x20) != 0) && ((this->m_ContextIFFlag.get() & 0x100) == 0))
        {
            auto forwardIntertiaMove = (SoX::Math::Vector::Forward() * this->m_PostureIntertiaMove * this->m_InputForce);
            auto forwardRotatedIntertiaMove = (m_PadRotation * forwardIntertiaMove);
            outForwardImpulse = outForwardImpulse + forwardRotatedIntertiaMove;
        }

        return 1;
    }

    inline bool PostureControl::sub_821FC148(SoX::Math::Vector* lerpImpulse, SoX::Math::Vector forwardImpulse, SoX::Math::Vector VerticalImpulse, double delta)
    {
        if (!lerpImpulse)
            return 0;

        *lerpImpulse = (forwardImpulse + VerticalImpulse) * delta;
        return 1;
    }

    inline bool PostureControl::sub_821F1728(IPosturePlugIn* plugin, SoX::Math::Vector& impulse, SoX::Math::Vector& impulseForward, SoX::Math::Vector& impulseVertical, double delta)
    {
        if (plugin && plugin->IsInContact())
        {
            guest_heap_var< SoX::Math::Vector> gsv_v1(impulseForward);
            guest_heap_var< SoX::Math::Vector> gsv_v2(impulseVertical);
            guest_heap_var< SoX::Math::Vector> gsv_v3(impulse);
            plugin->Func20(gsv_v1.get(), gsv_v2.get(), gsv_v1.get(), gsv_v3.get(), delta);
            impulse = *gsv_v3;
            return 1;
        }
        return 0;
    }

    inline bool PostureControl::sub_821F18E0(IPosturePlugIn* plugin, SoX::Math::Vector contactNormal, SoX::Math::Quaternion* outGroundNormal)
    {
        if (plugin && plugin->IsInContact())
        {
            plugin->Func28(guest_heap_var<SoX::Math::Vector>(contactNormal).get(), outGroundNormal);
            return true;
        }
        return false;
    }


    //--done--
    inline void PostureControl::sub_821FDF90(SoX::Math::Vector invertNormal)
    {
        m_GroundUp = invertNormal;
        if ((this->m_PostureFlag.get() & PostureControl::PostureFlag_Tentative) != 0 || (this->m_PostureRequestFlag.get() & 0x2000) != 0)
        {
            m_ImpulseUP = invertNormal.InvertNormal();
        }
        else if (this->m_PostureFlag.get() & PostureControl::PostureFlag_Grounded)
        {
            m_ImpulseUP = SoX::Math::Vector::Up();
        }
        //TODO : Not Sure if It Really XMVector3Rotate, probably XMQuatMultiply, but for GroundNormal its not exactly Quaternion but :|, ot ST really did jackshit in code
        this->sub_82200EC0(invertNormal);
        this->sub_82201620(invertNormal, m_GroundNormal * m_Rotation);
        this->sub_82202B08(m_GroundNormal * m_Rotation);
    }

    //--done--  make sure it works properly
    inline bool PostureControl::sub_821FE560(SoX::Math::Vector& position, SoX::Math::Vector& impulse, be<uint32_t>* postureFlag)
    {
        auto listener = this->sub_82202180(position, position + impulse, 0);
        if (!listener)
            return 0;

        //TODO: make sure it works properly
        if ((this->m_PostureRequestFlag.get() & 0x40) != 0 && (listener->GetCollisionFlag() & 0x400) != 0)
        {
            auto nImpulse = impulse.NormalizeEX();
            impulse = nImpulse.first;
            auto normImpulseX = nImpulse.second;
            auto norm_14 = listener->GetLength(impulse.X);
            auto normal = listener->GetContactNormal();
            this->sub_821F18E0(m_spRotationGravityAxis.get(), norm_14, &m_GroundNormal);
            position = listener->GetContactPosition();
            impulse = SoX::Math::Vector::Zero();
            *postureFlag = PostureControl::PostureFlag_Stone | PostureControl::PostureFlag_Grounded | PostureControl::PostureFlag_WallSide;
            this->sub_821FDF90(listener->GetContactNormal().InvertNormal());
            return 1;
        }
        else if ((listener->GetCollisionFlag() & 0x1) != 0)
        {
            auto nImpulse = impulse.NormalizeEX();
            impulse = nImpulse.first;
            auto normImpulseX = nImpulse.second;
            auto norm_14 = listener->GetLength(impulse.X);
            auto normal = listener->GetContactNormal();

            if (impulse.Dot(normal.InvertNormal()).X > 0.89999998)
                *postureFlag = PostureControl::PostureFlag_WallFront;

            position = position + (impulse * norm_14);

            auto v33 = normal.InvertNormal().Cross(impulse);
            auto v34 = v33.Cross(normal.InvertNormal());
            auto v32 = v34 * (normImpulseX - norm_14);

            impulse = v32;
            *postureFlag = PostureControl::PostureFlag_WallSide;

        }
        return 0;
    }

    inline void PostureControl::sub_821F1400(uint32_t inputFlag)
    {
        uint32_t materialBits = (inputFlag >> 4) & 0xFF00000;
        this->m_PostureFlag = (this->m_PostureFlag.get() & ~(PostureControl::PostureFlag_Grass | PostureControl::PostureFlag_DirtClay | PostureControl::PostureFlag_Stone | PostureControl::PostureFlag_Material1 | PostureControl::PostureFlag_Sand | PostureControl::PostureFlag_Material2 | PostureControl::PostureFlag_Material3 | PostureControl::PostureFlag_Material4)) | materialBits; // this->m_PostureFlag = (this->m_PostureFlag & 0xF00FFFFF) | materialBits;
    }

    // TODO : Make Readable, if you try to do check water, bounce on it and if you didn't hear water and sonic just died then you did something wrong 
    // --done--
    inline void PostureControl::sub_821F1378(uint32_t inputFlag)
    {
        GuestToHostFunction<void>(::sub_821F1378, this, inputFlag);
        return;

        uint32_t flags = this->m_PostureFlag.get();
        uint32_t transformedBits = ((inputFlag << 2) & 0xFFFFFFFC) | inputFlag;
        uint32_t rotatedInput = (inputFlag << 2) | (inputFlag >> 30);
        transformedBits = (rotatedInput & 0x100) | (transformedBits & 0xFFFFFEFF);
        transformedBits = (transformedBits & 0x300) | (inputFlag & 0x100);
        transformedBits = ((transformedBits << 2) & 0xFFFFFFFC) |
            (((inputFlag << 29) | (inputFlag >> 3)) & 0x1) |
            (inputFlag & 0x2);

        flags |= transformedBits;
        this->m_PostureFlag = flags;

        if (inputFlag & 0x400) flags |= PostureControl::PostureFlag_Tentative;

        uint32_t rotatedBits = (((inputFlag << 23) | (inputFlag >> 9)) & 0x4) |
            (((inputFlag << 3) | (inputFlag >> 29)) & 0x8);

        flags |= rotatedBits;
        this->m_PostureFlag = flags;


        if (inputFlag & 0x1000) flags |= PostureControl::PostureFlag_Stone;

        this->m_PostureFlag = flags;
    }

    //--done--
    inline void PostureControl::sub_821FEEF0(ICollisionListener* listener, uint32_t cflag)
    {
        if ((cflag & 0x10) != 0 && listener)
        {
            auto lrEntity = listener->GetContactEntity();
            if (lrEntity->m_pElement.get() && lrEntity->m_pElement->IsInitializedToWorld())
            {
                auto position = listener->GetContactPosition();
                auto flag = listener->GetRayFlag();
                lrEntity = listener->GetContactEntity();
                struct _message_ : SoX::Message<0x1A001>
                {
                    be<uint32_t> field4 = 0;
                    be<uint32_t> field8 = 0; //ret-prm
                    SoX::Math::Vector point;
                    be<uint32_t> rflag;
                    SoX::RefSharedPointer<SoX::RefCountObject> robj;
                } msg;
                msg.point = position;
                msg.rflag = flag;
                if (m_spPath.get() && lrEntity->m_pElement->ProcessMessage(msg) && m_spPath->SetControllerMovement(msg.robj, m_Rotation))
                {
                    m_PostureFlag = m_PostureFlag.get() | PostureControl::PostureFlag_RailGrind;
                }
            }
        }
        if ((cflag & 0x20) != 0)
        {
            m_PostureFlag = m_PostureFlag.get() | 0x80;
            if (auto pPostureEdge = static_cast<PostureEdge*>(m_spPostureEdge.get()))
            {
                pPostureEdge->m_Field54 = 0x80;
                pPostureEdge->m_Field70 = 0;
            }
        }
        if ((m_PostureRequestFlag.get() & 0x40) != 0 && (cflag & 0x400) != 0)
            m_PostureFlag = m_PostureFlag.get() | PostureControl::PostureFlag_Tentative;
    }

    //--done--
    inline bool PostureControl::sub_821FF910(SoX::Math::Vector& position, SoX::Math::Quaternion* groundNormal, SoX::Math::Vector& impulse)
    {
        be<uint32_t> flag;
        auto listener = this->sub_82201058(position, position + impulse, &flag);
        this->sub_821F1378(flag);
        this->sub_821F1400(flag);
        this->sub_821FEEF0(listener, flag);
        if (!listener)
        {
            position = (position + impulse);
            impulse = SoX::Math::Vector::Zero();
            return true;
        }

        if (!this->sub_821F1410(flag))
        {
            return false;
        }

        auto nImpulse = impulse.NormalizeEX();
        impulse = nImpulse.first;
        float normX = nImpulse.second;
        float normX_14 = listener->GetLength(normX);
        auto normal = listener->GetContactNormal();
        this->sub_821F18E0(this->m_spRotationGravityAxis.get(), normal, groundNormal);
        position = listener->GetContactPosition();
        auto invertNormal = SoX::Math::Vector::Zero();
        if ((this->m_ContextIFFlag.get() & 0x40) != 0)
        {
            impulse = SoX::Math::Vector::Zero();
            invertNormal = listener->GetContactNormal().InvertNormal();
        }
        else
        {
            auto rot_1 = SoX::Math::Quaternion::CreateFromVectors(SoX::Math::Vector::Up(), normal, SoX::Math::Vector::Right());
            auto rot_2 = this->m_Rotation;

            auto forwardNorm = (SoX::Math::Vector)(this->m_Rotation * (SoX::Math::Vector::Forward() * (normX - normX_14)));
            auto upRightNormal = (rot_1 * forwardNorm);
            impulse = upRightNormal;
            invertNormal = listener->GetContactNormal().InvertNormal();
        }
        this->sub_821FDF90(invertNormal);
        if (!m_spPhysicsBodyList.get())
        {
            return true;
        }
        auto listenerEntity = listener->GetContactEntity();
        m_spPhysicsBodyList->sub_8223B148(listenerEntity.get());
        return true;
    }

    //--done--
    inline bool PostureControl::sub_821FF0F0(SoX::Math::Vector& p1, SoX::Math::Quaternion* outNormal, SoX::Math::Vector p2)
    {
        if ((this->m_PostureRequestFlag.get() & 0x10) != 0)
            return false;

        auto v28 = m_GroundUp * m_DownForce;
        auto v8 = true;

        if (p2.Length3().X > 0.0 && (this->m_ContextIFFlag.get() & 0x40) != 0)
        {
            auto v12 = v28.Normalize3();
            auto v13 = p2.Normalize3();
            v8 = SoX::Math::Vector::Dot(v13, v12).X > 0.0;
        }

        ICollisionListener* v14;
        be<uint32_t> Flag1;
        if (((this->m_PostureFlag.get() & PostureControl::PostureFlag_Grounded) == 0)
            && v8
            && (v14 = this->sub_822011E8(p1 - v28, p1 + v28, &Flag1)) != 0
            && this->sub_821F1410(Flag1))
        {
            this->sub_821F1378(Flag1);
            this->sub_821F1400(Flag1);
            this->sub_821FEEF0(v14, Flag1);
            auto v15 = v14->GetContactNormal();
            this->sub_821F18E0(m_spRotationGravityAxis.get(), v15, outNormal);
            auto v16 = v14->GetContactNormal();
            sub_821FDF90(v16.InvertNormal());
            auto v19 = v14->GetLength();
            auto v35 = (v28.Normalize3() * v19);
            auto v34 = p1 - v28;
            auto v30 = v34 + v35;
            p1 = v30;
            if (m_spPhysicsBodyList.get())
            {
                auto entity = v14->GetContactEntity();
                m_spPhysicsBodyList->sub_8223B148(entity.get());

            }
            return true;
        }

        return false;
    }

    inline bool PostureControl::sub_821F17C8(IPosturePlugIn* plugin)
    {
        if (plugin->IsInContact())
        {
            plugin->IsOffContact();
            return true;
        }
        return false;
    }

    //--done--
    inline bool PostureControl::sub_821FFC08(Sonicteam::SoX::Math::Vector& position, Sonicteam::SoX::Math::Quaternion* groundNormal, Sonicteam::SoX::Math::Vector& impulse)
    {
        auto v63 = position + impulse;
        be<uint32_t> Flag1;
        auto v8 = this->sub_82202180(position, v63, &Flag1);
        this->sub_821F1378(Flag1);
        this->sub_821F1400(Flag1);
        this->sub_821FEEF0(v8, Flag1);
        if (v8)
        {
            auto [nImpulse, length] = impulse.NormalizeEX();
            impulse = nImpulse;
            auto v11 = v8->GetLength();
            auto v51 = v8->GetContactNormal();
            if ((Flag1.get() & 0x400) != 0)
            {
                if (nImpulse.Dot(v51.InvertNormal()).X > 0.80000001)
                {
                    this->sub_821F18E0(m_spRotationGravityAxis.get(), v51, groundNormal);
                    auto v58 = nImpulse * v11;
                    position = position + v58;
                    auto v65 = nImpulse * 50.0;
                    auto v62 = position + v65;
                    if (auto v14 = this->sub_82201350(position, v62, 0))
                    {
                        auto v52 = v14->GetContactPosition();
                        position = v52;
                        impulse = SoX::Math::Vector::Zero();
                        m_DisableAccuratePosture = 1;

                        if (m_spPhysicsBodyList.get())
                        {
                            auto entity = v8->GetContactEntity();
                            m_spPhysicsBodyList->sub_8223B148(entity.get());
                        }

                        m_PostureFlag = m_PostureFlag.get() | PostureControl::PostureFlag_Grass | PostureControl::PostureFlag_Grounded;

                        auto v20 = v8->GetContactNormal();
                        this->sub_821FDF90(v20.InvertNormal());
                        return 1;
                    }
                }
            }

            if ((Flag1.get() & 1) != 0)
            {
                auto v62 = nImpulse * v11;
                if (nImpulse.Dot(v51.InvertNormal()).X >= 0.89999998)
                    m_PostureFlag = m_PostureFlag.get() | PostureControl::PostureFlag_WallFront;

                position = position + v62;
                impulse = SoX::Math::Vector::Zero();

                m_PostureFlag = m_PostureFlag.get() | PostureControl::PostureFlag_WallSide;
            }
        }


        auto v64 = position + impulse;
        auto v27 = this->sub_82201058(position, v64, &Flag1);
        this->sub_821F1378(Flag1);
        this->sub_821F1400(Flag1);
        this->sub_821FEEF0(v27, Flag1);
        if (!v27)
        {
            position = position + impulse;
            impulse = SoX::Math::Vector::Zero();
            return 1;
        }
        if (this->sub_821F1410(Flag1))
        {
            auto [nImpulse, length] = impulse.NormalizeEX();
            impulse = nImpulse;
            auto v30 = v27->GetLength();
            auto v51 = v27->GetContactNormal();
            this->sub_821F18E0(m_spRotationGravityAxis.get(), v51, groundNormal);
            auto v60 = impulse * v30;
            position = position + v60;
            if ((m_PostureRequestFlag.get() & 0x40) != 0)
            {
                impulse = SoX::Math::Vector::Zero();
                auto v44 = v27->GetContactNormal();
                this->sub_821FDF90(v44.InvertNormal());
            }
            else
            {
                auto v70 = SoX::Math::Quaternion::CreateFromVectors(SoX::Math::Vector::Up(), v51, SoX::Math::Vector::Right());
                auto v52 = (SoX::Math::Vector)(m_Rotation * (SoX::Math::Vector::Forward() * (length - v30)));
                impulse = v70 * v52;
                this->sub_821FDF90(v27->GetContactNormal());
            }

            if (m_spPhysicsBodyList.get())
            {
                auto entity = v27->GetContactEntity();
                m_spPhysicsBodyList->sub_8223B148(entity.get());
            }

        }
        return 0;
    }

    inline void PostureControl::sub_821F1968(SoX::Math::Vector v1, SoX::Math::Vector v2)
    {
        this->m_LocalRotation = v2;
        this->m_LocalPosition = v1;
        if (auto pRootFrame = m_spRootFrame.get())
        {
            auto m1 = SoX::Math::Matrix4x4::RotationQuaternion(m_LocalRotation);
            auto m2 = SoX::Math::Matrix4x4::TranslationFromVector(m_LocalPosition);
            auto f = m1 * m2; //XMMatrixMultiply_2___1 82168C48
            pRootFrame->UpdateTransform(f);
        }
    }

    inline void PostureControl::sub_821FDDB0()
    {
        //Spining
        if ((m_PostureFlag118.get() & 0x10) != 0)
        {
            m_PostureFlag118 = m_PostureFlag118.get() & ~0x10;
            this->sub_821F1968(m_LocalPosition, m_LocalRotation);
        }
        else
        {
            this->sub_821F1968(m_Position, m_Rotation * (SoX::Math::Quaternion)m_GroundNormal); // * = XMQuaternionMultiplys
        }

        if ((m_PostureFlag.get() & PostureControl::PostureControl::PostureFlag_Grounded) == 0)
        {
            m_GroundUp = SoX::Math::Vector::Up().InvertNormal();
            this->sub_82200EC0(m_GroundUp);
        }

        if ((m_PostureRequestFlag.get() & 0x1000) != 0)
        {
            m_Vector3D0 = m_Rotation;
            m_Vector3E0 = m_GroundNormal;
        }

        if ((m_PostureRequestFlag.get() & 0x2000) != 0)
        {
            auto matrix = SoX::Math::Matrix4x4::LookToRH(m_Position, m_GroundUp.InvertNormal(), m_Position + SoX::Math::Vector::Up());
            if (auto pRootFrame = m_spRootFrame.get())
            {
                auto m1 = SoX::Math::Matrix4x4::RotationQuaternion(matrix.V[0]);
                auto m2 = SoX::Math::Matrix4x4::TranslationFromVector(matrix.V[1]);
                auto f = m1 * m2; //XMMatrixMultiply_2___1 82168C48
                pRootFrame->UpdateTransform(f);
            }
        }
    }

}
