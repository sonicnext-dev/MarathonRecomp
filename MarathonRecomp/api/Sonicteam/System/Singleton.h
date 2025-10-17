#pragma once

namespace Sonicteam::System
{
    template <typename T, const uint32_t pSingleton, typename TCreator>
    class Singleton
    {
        inline static TCreator ms_Creator{};

    public:
        static T* GetInstance()
        {
            auto pInstance = (xpointer<T>*)g_memory.Translate(pSingleton);

            if (!pInstance->ptr.get())
                *pInstance = ms_Creator.Create();

            return *pInstance;
        }
    };
}
