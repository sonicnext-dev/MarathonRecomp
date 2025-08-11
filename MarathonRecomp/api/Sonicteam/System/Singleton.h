#pragma once

namespace Sonicteam::System
{
    template <typename T, const uint32_t Ptr, typename TCreator>
    class Singleton
    {
        inline static TCreator m_Creator{};

    public:
        static T* GetInstance()
        {
            auto pInstance = (xpointer<T>*)g_memory.Translate(Ptr);

            if (!pInstance->ptr.get())
                *pInstance = m_Creator.Create();

            return *pInstance;
        }
    };
}
