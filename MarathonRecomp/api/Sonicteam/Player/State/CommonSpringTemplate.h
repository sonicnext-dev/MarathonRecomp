namespace Sonicteam::Player::State
{
    // T - Probably Angle, flag - ExternalFlag ( from IExportExternalFlag )
    template <uint32_t T, uint32_t flag>
    class CommonSpringTemplate : public CommonObject
    {
    public:
        be<float> m_AngleMoveData; // Default Value 1.0, +-
        bool m_IsRunBorder;
        be<float> m_ContinuousForceTime; // LastLockInputTime * 0.5, 
        be<uint32_t> m_CurrentState;
    };

    typedef CommonSpringTemplate<0, 0> CommonSpringTemplate_Default;

    // Default 06 Types
    typedef CommonSpringTemplate<25, 0x40> CommonSpringTemplate_2540; // DashRamp
    typedef CommonSpringTemplate<75, 0x10> CommonSpringTemplate_7510; // Spring, DashRamp, Pole
    typedef CommonSpringTemplate<75, 0x2000000> CommonSpringTemplate_752000000;
    typedef CommonSpringTemplate<75, 0x20000000> CommonSpringTemplate_7520000000;
    typedef CommonSpringTemplate<76, 0x80> CommonSpringTemplate_7680; // WideSpring usually

    MARATHON_ASSERT_OFFSETOF(CommonSpringTemplate_Default, m_AngleMoveData, 0xC);
    MARATHON_ASSERT_OFFSETOF(CommonSpringTemplate_Default, m_IsRunBorder, 0x10);
    MARATHON_ASSERT_OFFSETOF(CommonSpringTemplate_Default, m_ContinuousForceTime, 0x14);
    MARATHON_ASSERT_OFFSETOF(CommonSpringTemplate_Default, m_CurrentState, 0x18);
    MARATHON_ASSERT_SIZEOF(CommonSpringTemplate_Default, 0x1C);
}
