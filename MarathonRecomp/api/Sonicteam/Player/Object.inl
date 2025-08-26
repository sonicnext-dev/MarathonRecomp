namespace Sonicteam::Player
{
    template <typename T>
    inline T* Object::GetGauge()
    {
        return (T*)m_spGauge.get();
    }

    template<typename T>
    inline T* Object::GetPlugin(const char* pluginName)
    {
        for (auto& spPlugin : m_vspPlayerPlugins)
        {
            if (spPlugin->m_Name == pluginName)
                return static_cast<T*>(spPlugin.get());
        }

        return nullptr;
    }
}
