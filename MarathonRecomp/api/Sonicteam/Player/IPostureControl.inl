#pragma once

namespace Sonicteam::Player
{
    // Recreate from sub_821F1E30, Full 1:1 recreated for debugging purposes
    inline void IPostureControl::RecreatedUpdate(double deltaTime)
    {
        if (auto contextIF = this->m_ContextIF.get())
        {
            this->m_ImpulseForward = contextIF->GetTotalForwardImpulse();
            this->m_ImpulseVertical = contextIF->GetTotalVerticalImpulse();
        }

        if ((this->m_PostureRequestFlag.get() & 0x8000) != 0)
            this->ImportPad(this->CalculateDefaultTaskPad(), 1.0);
        else
        {
            auto pAmigoAIListener = this->m_spAmigoListener.get();
            auto pInputListener = this->m_spInputListener.get();

            if (pAmigoAIListener && pAmigoAIListener->IsEnabled())
                this->ImportPad(pAmigoAIListener->GetPadQuat(), pAmigoAIListener->GetPadMagnitude());
            else if (pInputListener && pInputListener->IsEnabled())
                this->ImportPad(pInputListener->GetPadQuat(), pInputListener->GetPadMagnitude());
        }

        if (auto pGravity = this->GetGravity())
        {
            this->m_GravityDirection = pGravity->GetDirection();
            this->m_GravityForce = pGravity->GetForce();
            this->m_GroundNormal = Sonicteam::SoX::Math::CreateRotationFromUpToDirection(this->m_GravityDirection);
        }
    }

    template <typename T>
    inline T* IPostureControl::GetGravity()
    {
        return static_cast<T*>(this->m_spGravityForce.get());
    }

    template <typename T>
    inline T* IPostureControl::GetWorld()
    {
        return static_cast<T*>(this->m_spWorld.get());
    }

    //ref to sub_821F1B38 
    inline SoX::Math::Quaternion IPostureControl::CalculateDefaultTaskPad()
    {
        if (!this->m_pPlayerTask.get())
            return SoX::Math::Quaternion::Identity();

        struct MessageGetActorID :SoX::Message<0x1300C>
        {
            be<uint32_t> ActorID;
        };
        guest_stack_var<MessageGetActorID> m_gsvMessageGetActorID;
        this->m_pPlayerTask->ProcessMessage(m_gsvMessageGetActorID.get());
        auto pActor = this->m_spActorManager->FindActor(m_gsvMessageGetActorID->ActorID);
        if (!pActor)
            return SoX::Math::Quaternion::Identity();

        struct MessageGetActorTransform :SoX::Message<0x10009>
        {
            SoX::Math::Matrix4x4 Matrix4x4;
        };
        guest_stack_var<MessageGetActorTransform> m_gsvMessageGetActorTransform;
        pActor->ProcessMessage(m_gsvMessageGetActorTransform.get());
        auto transformCoord = SoX::Math::TransformCoord(SoX::Math::Vector::Forward(), m_gsvMessageGetActorTransform->Matrix4x4);
        return SoX::Math::Quaternion::CreateFromVectors(transformCoord, SoX::Math::Vector::Forward(), SoX::Math::Vector::Up());
    }

    inline void IPostureControl::ImportPad(SoX::Math::Quaternion pad, float step)
    {
        auto vft = reinterpret_cast<Vftable*>(static_cast<IVariable*>(this)->m_pVftable.get());
        GuestToHostFunction<void>(vft->fpImportPad, this, guest_heap_var< SoX::Math::Quaternion>(pad).get(), step);
    }
}

