#pragma once

namespace Sonicteam::System
{
    template <typename T, const uint32_t pSingleton, typename TCreator>
    class Singleton
    {
    private:
        inline static TCreator ms_Creator{};

    public:
        static T* GetInstance()
        {
            const auto pInstance = reinterpret_cast<xpointer<T>*>(g_memory.Translate(pSingleton));

            if (!pInstance->ptr.get())
                *pInstance = ms_Creator.Create();

            return *pInstance;
        }
    };
}
