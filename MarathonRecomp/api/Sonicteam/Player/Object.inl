namespace Sonicteam::Player
{
    template <typename T>
    inline T* Object::GetGauge()
    {
        return (T*)m_spGauge.get();
    }
}
