#pragma once

#include <Marathon.inl>
#include <Sonicteam/SoX/IResource2.h>


namespace Sonicteam::SoX::Graphics
{
    class ShaderMgr;

    class Shader : public IResource2<Shader, ShaderMgr>
    {
    public:
        MARATHON_INSERT_PADDING(0xC);   
    };

}
