namespace Sonicteam::Player::State
{
    template <uint32_t T, uint32_t flag>
    class CommonSpringTemplate : public CommonObject
    {
    public:
        be<float> m_FieldC;
        bool m_Field10;
        be<float> m_Field14;
        be<uint32_t> m_Field18;
    };

    typedef CommonSpringTemplate<0, 0> CommonSpringTemplate_Default;
    // Default 06 Types
    typedef CommonSpringTemplate<25, 0x40> CommonSpringTemplate_2540; // DashRamp
    typedef CommonSpringTemplate<75, 0x10> CommonSpringTemplate_7510; //Spring, DashRamp, Pole
    typedef CommonSpringTemplate<75, 0x2000000> CommonSpringTemplate_752000000;
    typedef CommonSpringTemplate<75, 0x20000000> CommonSpringTemplate_7520000000;
    typedef CommonSpringTemplate<76, 0x80> CommonSpringTemplate_7680; // WideSpring, usually


    // Verify member offsets first
    MARATHON_ASSERT_OFFSETOF(CommonSpringTemplate_Default, m_FieldC, 0xC);
    MARATHON_ASSERT_OFFSETOF(CommonSpringTemplate_Default, m_Field10, 0x10);
    MARATHON_ASSERT_OFFSETOF(CommonSpringTemplate_Default, m_Field14, 0x14);
    MARATHON_ASSERT_OFFSETOF(CommonSpringTemplate_Default, m_Field18, 0x18);
    MARATHON_ASSERT_SIZEOF(CommonSpringTemplate_Default, 0x1C);
}
