#pragma once

#include <locale/locale.h>

class IConfigDef
{
public:
    virtual ~IConfigDef() = default;
    virtual bool IsHidden() = 0;
    virtual void ReadValue(toml::v3::ex::parse_result& toml) = 0;
    virtual void MakeDefault() = 0;
    virtual std::string_view GetSection() const = 0;
    virtual std::string_view GetName() const = 0;
    virtual std::string GetNameLocalised(ELanguage language) const = 0;
    virtual std::string GetDescription(ELanguage language) const = 0;
    virtual bool IsDefaultValue() const = 0;
    virtual const void* GetValue() const = 0;
    virtual std::string GetValueLocalised(ELanguage language) const = 0;
    virtual std::string GetValueDescription(ELanguage language) const = 0;
    virtual std::string GetDefinition(bool withSection = false) const = 0;
    virtual std::string ToString(bool strWithQuotes = true) const = 0;
    virtual void GetLocaleStrings(std::vector<std::string_view>& localeStrings) const = 0;
    virtual void SnapToNearestAccessibleValue(bool searchUp) = 0;
};

#define CONFIG_LOCALE            std::unordered_map<ELanguage, std::tuple<std::string, std::string>>
#define CONFIG_ENUM_LOCALE(type) std::unordered_map<ELanguage, std::unordered_map<type, std::tuple<std::string, std::string>>>

#define CONFIG_CALLBACK(name)       if (name.Callback) name.Callback(&name)
#define CONFIG_LOCK_CALLBACK(name)  if (name.LockCallback) name.LockCallback(&name)
#define CONFIG_APPLY_CALLBACK(name) if (name.ApplyCallback) name.ApplyCallback(&name)

#define WINDOWPOS_CENTRED        0x2FFF0000

extern std::vector<IConfigDef*> g_configDefinitions;

enum class EVoiceLanguage : uint32_t
{
    English,
    Japanese
};

enum class ECameraRotationMode : uint32_t
{
    Normal,
    Reverse
};

enum class EControllerIcons : uint32_t
{
    Auto,
    Xbox,
    PlayStation
};

enum class EChannelConfiguration : uint32_t
{
    Stereo,
    Surround
};

enum class EGraphicsAPI : uint32_t
{
    Auto,
#ifdef MARATHON_RECOMP_D3D12
    D3D12,
#endif
#ifdef MARATHON_RECOMP_METAL
    Metal,
#endif
    Vulkan
};

enum class EWindowState : uint32_t
{
    Normal,
    Maximised
};

enum class EAspectRatio : uint32_t
{
    Auto,
    Wide,
    Narrow,
    OriginalNarrow
};

enum class ETripleBuffering : uint32_t
{
    Auto,
    On,
    Off
};

static constexpr int32_t FPS_MIN = 15;
static constexpr int32_t FPS_MAX = 241;

enum class EAntiAliasing : uint32_t
{
    None = 0,
    MSAA2x = 2,
    MSAA4x = 4,
    MSAA8x = 8
};

enum class EShadowResolution : int32_t
{
    x512 = 512,
    x1024 = 1024,
    x2048 = 2048,
    x4096 = 4096,
    x8192 = 8192
};

enum class EReflectionResolution : int32_t
{
    Eighth,
    Quarter,
    Half,
    Full
};

enum class ERadialBlur : uint32_t
{
    Off,
    Original
};

enum class ECutsceneAspectRatio : uint32_t
{
    Original,
    Unlocked
};

enum class EUIAlignmentMode : uint32_t
{
    Edge,
    Centre
};

enum class EPlayerCharacter : uint32_t
{
    Sonic,
    Shadow,
    Silver,
    Blaze,
    Amy,
    Tails,
    Rouge,
    Knuckles
};

template<typename T, bool isHidden = false>
class ConfigDef final : public IConfigDef
{
public:
    std::string Section{};
    std::string Name{};
    CONFIG_LOCALE* Locale{};
    T DefaultValue{};
    T Value{ DefaultValue };
    std::set<T> InaccessibleValues{};
    std::unordered_map<std::string, T>* EnumTemplate{};
    std::map<T, std::string> EnumTemplateReverse{};
    CONFIG_ENUM_LOCALE(T)* EnumLocale{};
    std::function<void(ConfigDef<T, isHidden>*)> Callback;
    std::function<void(ConfigDef<T, isHidden>*)> LockCallback;
    std::function<void(ConfigDef<T, isHidden>*)> ApplyCallback;
    bool IsLoadedFromConfig{};

    // CONFIG_DEFINE
    ConfigDef(std::string section, std::string name, T defaultValue);

    // CONFIG_DEFINE_LOCALISED
    ConfigDef(std::string section, std::string name, CONFIG_LOCALE* nameLocale, T defaultValue);

    // CONFIG_DEFINE_ENUM
    ConfigDef(std::string section, std::string name, T defaultValue, std::unordered_map<std::string, T>* enumTemplate);

    // CONFIG_DEFINE_ENUM_LOCALISED
    ConfigDef(std::string section, std::string name, CONFIG_LOCALE* nameLocale, T defaultValue, std::unordered_map<std::string, T>* enumTemplate, CONFIG_ENUM_LOCALE(T)* enumLocale);

    ConfigDef(const ConfigDef&) = delete;
    ConfigDef(ConfigDef&&) = delete;
    ~ConfigDef();

    bool IsHidden() override;
    void ReadValue(toml::v3::ex::parse_result& toml) override;
    void MakeDefault() override;
    std::string_view GetSection() const override;
    std::string_view GetName() const override;
    std::string GetNameLocalised(ELanguage language) const override;
    std::string GetDescription(ELanguage language) const override;
    bool IsDefaultValue() const override;
    const void* GetValue() const override;
    std::string GetValueLocalised(ELanguage language) const override;
    std::string GetValueDescription(ELanguage language) const override;
    std::string GetDefinition(bool withSection = false) const override;
    std::string ToString(bool strWithQuotes = true) const override;
    void GetLocaleStrings(std::vector<std::string_view>& localeStrings) const override;
    void SnapToNearestAccessibleValue(bool searchUp) override;

    operator T() const
    {
        return Value;
    }

    void operator=(const T& other)
    {
        Value = other;
    }
};

#define CONFIG_DECLARE(type, name)                                              static ConfigDef<type>       name;
#define CONFIG_DECLARE_HIDDEN(type, name)                                       static ConfigDef<type, true> name;

#define CONFIG_DEFINE(section, type, name, defaultValue)                        CONFIG_DECLARE(type, name)
#define CONFIG_DEFINE_HIDDEN(section, type, name, defaultValue)                 CONFIG_DECLARE_HIDDEN(type, name)
#define CONFIG_DEFINE_LOCALISED(section, type, name, defaultValue)              CONFIG_DECLARE(type, name)
#define CONFIG_DEFINE_ENUM(section, type, name, defaultValue)                   CONFIG_DECLARE(type, name)
#define CONFIG_DEFINE_ENUM_LOCALISED(section, type, name, defaultValue)         CONFIG_DECLARE(type, name)

class Config
{
public:
    #include "config_def.h"

    static inline bool s_isCallbacksCreated;

    static std::filesystem::path GetConfigPath();

    static void CreateCallbacks();
    static void Load();
    static void Save();
};
