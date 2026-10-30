#pragma once

#include <concepts>

namespace Sonicteam::System
{
    class ISingleton {};

    template <typename T, const uint32_t pSingleton, typename TCreator>
    class Singleton : public ISingleton
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

    template <typename T>
    concept DerivedSingleton = std::derived_from<T, ISingleton> && requires
    {
        { T::GetInstance() } -> std::same_as<T*>;
    };
}
