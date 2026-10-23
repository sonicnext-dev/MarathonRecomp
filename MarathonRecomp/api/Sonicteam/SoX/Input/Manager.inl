namespace Sonicteam::SoX::Input
{
    inline bool PadState::IsDown(const PadButton buttons) const
    {
        return (Buttons & buttons) == buttons;
    }

    inline bool PadState::IsPressed(const PadButton buttons) const
    {
        return (PressedButtons & buttons) == buttons;
    }

    inline bool PadState::IsReleased(const PadButton buttons) const
    {
        return (ReleasedButtons & buttons) == buttons;
    }
}
