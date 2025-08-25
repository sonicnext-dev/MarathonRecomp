namespace Sonicteam::Player
{
    template <typename T>
    inline T* Object::GetGauge()
    {
        return (T*)m_spGauge.get();
    }

    template<typename T>
    inline T* Sonicteam::Player::Object::GetPlugin(const char* pluginName)
    {
        for (stdx::vector<boost::shared_ptr<Sonicteam::Player::IPlugIn>>::iterator it = m_vspPlayerPlugins.begin(); it != m_vspPlayerPlugins.end(); it = it + 1)
        {
            if (it->get()->m_Name == pluginName)
            {
                return static_cast<T*>(it->get());
            }
        }
        return nullptr;
    }
}
