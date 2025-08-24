namespace Sonicteam::Player
{
    template <typename T>
    inline T* Object::GetGauge()
    {
        return (T*)m_spGauge.get();
    }

    template<typename T>
    inline T* Sonicteam::Player::Object::GetPlugin(const char* PluginName)
    {
        for (stdx::vector<boost::shared_ptr<Sonicteam::Player::IPlugIn>>::iterator it = m_PlayerPlugins.begin(); it != m_PlayerPlugins.end(); it = it + 1)
        {
            if (it->get()->m_Name == PluginName)
            {
                return static_cast<T*>(it->get());
            }
        }
        return nullptr;
    }
}
