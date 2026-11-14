#pragma once

#include <Marathon.inl>
#include <Sonicteam/Prop/Class.h>
#include <stdx/compare/string_compare.h>
#include <stdx/map.h>
#include <stdx/string.h>

namespace Sonicteam::Prop
{
    class ClassRegistry
    {
    public:
        stdx::map<std::string, xpointer<Class>> m_Classes;
    };

    MARATHON_ASSERT_OFFSETOF(ClassRegistry, m_Classes, 0x00);
    MARATHON_ASSERT_SIZEOF(ClassRegistry, 0x0C);
}
