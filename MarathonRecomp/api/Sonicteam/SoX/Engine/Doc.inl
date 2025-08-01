namespace Sonicteam::SoX::Engine
{
    template <typename T>
    inline T* Doc::GetDocMode()
    {
        return (T*)m_pDocMode.get();
    }
}
