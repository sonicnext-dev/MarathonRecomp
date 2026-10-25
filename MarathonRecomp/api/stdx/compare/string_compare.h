#pragma once

namespace stdx
{
    struct string_compare
    {
        using is_transparent = void;

        bool operator()(xpointer<const char> lhs, xpointer<const char> rhs) const
        {
            return std::strcmp(lhs.get(), rhs.get()) < 0;
        }

        bool operator()(const stdx::string& lhs, const stdx::string& rhs) const
        {
            return lhs < rhs;
        }

        bool operator()(const char* lhs, const stdx::string& rhs) const
        {
            return std::strcmp(lhs, rhs.c_str()) < 0;
        }

        bool operator()(const stdx::string& lhs, const char* rhs) const
        {
            return std::strcmp(lhs.c_str(), rhs) < 0;
        }
    };
}
