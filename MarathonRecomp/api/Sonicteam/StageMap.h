#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/Component.h>
#include <stdx/string.h>

namespace stdx
{
    template <typename Type>
    struct vector
    {
        be<uint32_t> _MyProxy;
        xpointer<Type> _MyFirst;
        xpointer<Type> _MyLast;
        xpointer<Type> _MyEnd;
        
        Type& operator[](uint32_t index)
        {
            return *(Type*)((uint64_t)_MyFirst.get()  + (index * sizeof(Type)));
        }
        size_t size() 
        {
            return (_MyLast.ptr.get() - _MyFirst.ptr.get()) / sizeof(Type);
        }
    };

}
namespace Sonicteam
{
    //Already in SonicGaugeCode (full)
   
    class StageMap : public SoX::Component
    {
    public:

        stdx::string m_Name;
        stdx::string m_Text;
        stdx::vector<xpointer<StageMap>> m_sStageMap;
    
    };
}
