#pragma once

#include <Marathon.h>
#include <kernel/heap.h>
#include <string>
#include <vector>

namespace stdx
{
    class type_info
    {
    public:
        struct Vftable
        {
            be<uint32_t> fpdtor;
        };
        xpointer<Vftable> m_pVftable;
        be<uint32_t> m_InternalRuntimeReference;
        const char m_name[0];

        type_info(const type_info&) = delete;
        type_info& operator=(const type_info&) = delete;

        const char* raw_name() const
        {
            return &m_name[0];
        }

        std::string name() const
        {
            const char* mangled = &m_name[0];
            if (strncmp(mangled, ".?A", 3) != 0)
                return std::string(mangled);

            const char* type_char = mangled + 3;
            const char* type_start = type_char;

            if (*type_char == 'V' || *type_char == 'U' || *type_char == '_')
                type_start++;

            const char* end = strstr(type_start, "@@");
            if (!end)
                return std::string(mangled);

            std::string result;
            switch (mangled[3]) {
            case 'V': result = "class "; break;
            case 'U': result = "struct "; break;
            case '_': result = "union "; break;
            }

            // Parse all segments first
            std::vector<std::string> segments;
            const char* p = type_start;

            while (p < end) {
                if (*p == '@') {
                    p++;
                    continue;
                }

                const char* seg_start = p;
                while (p < end && *p != '@') p++;
                if (p > seg_start) {
                    segments.push_back(std::string(seg_start, p - seg_start));
                }
            }

            // Decode template parameter algorithm
            auto decode_param = [](const std::string& param) -> uint64_t {
                if (param.empty() || param[0] != '$') return 0;

                const char* str = param.c_str() + 2; // Skip "$0"
                size_t len = strlen(str);

                if (len == 2) {
                    // Two-character encoding: (c1 - 'A') * 16 + (c2 - 'A')
                    return (str[0] - 'A') * 16 + (str[1] - 'A');
                }
                else {
                    // Multi-character encoding for large powers of 2
                    // Count number of 'A's after the first character
                    int num_As = 0;
                    for (size_t i = 1; i < len && str[i] == 'A'; i++) {
                        num_As++;
                    }

                    // Calculate exponent based on pattern
                    int exponent = num_As * 4;
                    if (str[0] == 'C') {
                        exponent += 1;
                    }

                    return 1ULL << exponent;
                }
                };

            // Process segments to handle templates
            std::vector<std::string> parts;
            for (size_t i = 0; i < segments.size(); i++) {
                if (segments[i].find("?$") == 0) {
                    // Template class
                    std::string tmpl_name = segments[i].substr(2);

                    // Get template parameters
                    std::vector<std::string> params;
                    for (size_t j = i + 1; j < segments.size(); j++) {
                        if (segments[j].empty() || segments[j][0] != '$') break;

                        uint64_t val = decode_param(segments[j]);
                        params.push_back(std::to_string(val));
                        i = j;
                    }

                    // Build template string
                    std::string full_tmpl = tmpl_name + "<";
                    for (size_t k = 0; k < params.size(); k++) {
                        if (k > 0) full_tmpl += ",";
                        full_tmpl += params[k];
                    }
                    full_tmpl += ">";
                    parts.push_back(full_tmpl);
                }
                else if (segments[i].empty() || segments[i][0] == '$') {
                    // Skip parameter segments (already handled)
                    continue;
                }
                else {
                    // Regular namespace/class
                    parts.push_back(segments[i]);
                }
            }

            // Reverse for correct namespace order (MSVC stores backwards)
            for (int i = parts.size() - 1; i >= 0; i--) {
                if (i < parts.size() - 1) result += "::";
                result += parts[i];
            }

            return result;
        }
    };
}
