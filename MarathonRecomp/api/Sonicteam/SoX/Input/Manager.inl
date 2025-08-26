namespace Sonicteam::SoX::Input
{
    inline bool PadState::IsDown(const KeyState in_Keys) const
    {
        return (LastButtons & in_Keys) == in_Keys;
    }

    inline bool PadState::IsPressed(const KeyState in_Keys) const
    {
        return (PressedButtons & in_Keys) == in_Keys;
    }

    inline bool PadState::IsReleased(const KeyState in_Keys) const
    {
        return (ReleasedButtons & in_Keys) == in_Keys;
    }
}
