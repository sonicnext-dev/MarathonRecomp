#pragma once

namespace Sonicteam::SoX::Graphics
{
    inline void Frame::SetTransform(SoX::Math::Matrix4x4* matrix)
    {
        auto vft = (Vftable*)m_pVftable.get();
        GuestToHostFunction<void>(vft->fpSetTransform, this, matrix);
    }

    inline void Frame::SetTransform(SoX::Math::Matrix4x4& matrix)
    {
        SetTransform(guest_heap_var<SoX::Math::Matrix4x4>(matrix).get());
    }

    // Recreated from sub_82594698
    inline void Frame::UpdateTransform(SoX::Math::Matrix4x4& matrix)
    {
        SetTransform(matrix);
        auto pOwner = m_pOwner;
        if (pOwner->m_lnFrame2.m_pNext == 0 && pOwner->m_plnFrame.get())
        {
            pOwner->m_plnFrame->InsertFast(&pOwner->m_lnFrame2);
        }
        pOwner->m_Flag = pOwner->m_Flag.get() | 1;
        m_Flag = m_Flag.get() | 2;
    }
}
