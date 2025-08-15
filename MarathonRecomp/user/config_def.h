// This file gets included in both config.h and config.cpp, with their own macros changing
// the preprocessed output. The header is only going to have the declarations this way.

CONFIG_DEFINE_ENUM_LOCALISED("System", ELanguage, Language, ELanguage::English);
CONFIG_DEFINE_ENUM_LOCALISED("System", EVoiceLanguage, VoiceLanguage, EVoiceLanguage::English);
CONFIG_DEFINE_LOCALISED("System", bool, Subtitles, true);
CONFIG_DEFINE_LOCALISED("System", bool, Hints, true);
CONFIG_DEFINE_LOCALISED("System", bool, ControlTutorial, true);
CONFIG_DEFINE_LOCALISED("System", bool, AchievementNotifications, true);
CONFIG_DEFINE("System", bool, ShowConsole, false);

CONFIG_DEFINE_ENUM_LOCALISED("Input", ECameraRotationMode, HorizontalCamera, ECameraRotationMode::Reverse);
CONFIG_DEFINE_ENUM_LOCALISED("Input", ECameraRotationMode, VerticalCamera, ECameraRotationMode::Normal);
CONFIG_DEFINE_LOCALISED("Input", bool, Vibration, true);
CONFIG_DEFINE_LOCALISED("Input", bool, AllowBackgroundInput, false);
CONFIG_DEFINE_ENUM_LOCALISED("Input", EControllerIcons, ControllerIcons, EControllerIcons::Auto);

CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_A, SDL_SCANCODE_S);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_B, SDL_SCANCODE_D);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_X, SDL_SCANCODE_A);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_Y, SDL_SCANCODE_W);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_DPadUp, SDL_SCANCODE_UNKNOWN);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_DPadDown, SDL_SCANCODE_UNKNOWN);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_DPadLeft, SDL_SCANCODE_Q);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_DPadRight, SDL_SCANCODE_E);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_Start, SDL_SCANCODE_RETURN);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_Back, SDL_SCANCODE_BACKSPACE);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_LeftTrigger, SDL_SCANCODE_1);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_RightTrigger, SDL_SCANCODE_3);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_LeftBumper, SDL_SCANCODE_UNKNOWN);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_RightBumper, SDL_SCANCODE_UNKNOWN);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_LeftStickUp, SDL_SCANCODE_UP);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_LeftStickDown, SDL_SCANCODE_DOWN);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_LeftStickLeft, SDL_SCANCODE_LEFT);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_LeftStickRight, SDL_SCANCODE_RIGHT);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_RightStickUp, SDL_SCANCODE_UNKNOWN);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_RightStickDown, SDL_SCANCODE_UNKNOWN);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_RightStickLeft, SDL_SCANCODE_UNKNOWN);
CONFIG_DEFINE_ENUM("Bindings", SDL_Scancode, Key_RightStickRight, SDL_SCANCODE_UNKNOWN);

CONFIG_DEFINE_LOCALISED("Audio", float, MasterVolume, 1.0f);
CONFIG_DEFINE_LOCALISED("Audio", float, MusicVolume, 0.6f);
CONFIG_DEFINE_LOCALISED("Audio", float, EffectsVolume, 0.6f);
CONFIG_DEFINE_ENUM_LOCALISED("Audio", EChannelConfiguration, ChannelConfiguration, EChannelConfiguration::Stereo);
CONFIG_DEFINE_LOCALISED("Audio", bool, MusicAttenuation, false);

CONFIG_DEFINE("Video", std::string, GraphicsDevice, "");
CONFIG_DEFINE_ENUM("Video", EGraphicsAPI, GraphicsAPI, EGraphicsAPI::Auto);
CONFIG_DEFINE("Video", int32_t, WindowX, WINDOWPOS_CENTRED);
CONFIG_DEFINE("Video", int32_t, WindowY, WINDOWPOS_CENTRED);
CONFIG_DEFINE_LOCALISED("Video", int32_t, WindowSize, -1);
CONFIG_DEFINE("Video", int32_t, WindowWidth, 1280);
CONFIG_DEFINE("Video", int32_t, WindowHeight, 720);
CONFIG_DEFINE_ENUM("Video", EWindowState, WindowState, EWindowState::Normal);
CONFIG_DEFINE_LOCALISED("Video", int32_t, Monitor, 0);
// TODO: Localise this enum
CONFIG_DEFINE_ENUM("Video", ERadialBlur, RadialBlur, ERadialBlur::Original);
CONFIG_DEFINE_ENUM_LOCALISED("Video", EAspectRatio, AspectRatio, EAspectRatio::Auto);
CONFIG_DEFINE_LOCALISED("Video", float, ResolutionScale, 1.0f);
CONFIG_DEFINE_LOCALISED("Video", bool, Fullscreen, true);
CONFIG_DEFINE_LOCALISED("Video", bool, VSync, true);
CONFIG_DEFINE_ENUM("Video", ETripleBuffering, TripleBuffering, ETripleBuffering::Auto);
CONFIG_DEFINE_LOCALISED("Video", int32_t, FPS, 60);
CONFIG_DEFINE("Video", bool, ShowFPS, false);
CONFIG_DEFINE("Video", uint32_t, MaxFrameLatency, 2);
CONFIG_DEFINE_LOCALISED("Video", float, Brightness, 0.5f);
CONFIG_DEFINE_ENUM_LOCALISED("Video", EAntiAliasing, AntiAliasing, EAntiAliasing::MSAA4x);
CONFIG_DEFINE_LOCALISED("Video", bool, TransparencyAntiAliasing, true);
CONFIG_DEFINE("Video", uint32_t, AnisotropicFiltering, 16);
CONFIG_DEFINE_ENUM("Video", EShadowResolution, ShadowResolution, EShadowResolution::x4096);
// TODO: Localise this enum
CONFIG_DEFINE_ENUM("Video", EReflectionResolution, ReflectionResolution, EReflectionResolution::Half);
CONFIG_DEFINE_ENUM_LOCALISED("Video", ECutsceneAspectRatio, CutsceneAspectRatio, ECutsceneAspectRatio::Original);
CONFIG_DEFINE_ENUM_LOCALISED("Video", EUIAlignmentMode, UIAlignmentMode, EUIAlignmentMode::Edge);

CONFIG_DEFINE_HIDDEN("Codes", bool, DisableDWMRoundedCorners, false);
CONFIG_DEFINE_HIDDEN("Codes", bool, DisableTitleInputDelay, false);
CONFIG_DEFINE_HIDDEN("Codes", bool, HUDToggleKey, false);
CONFIG_DEFINE_HIDDEN("Codes", bool, SkipIntroLogos, false);
CONFIG_DEFINE_HIDDEN("Codes", bool, UseOfficialTitleOnTitleBar, false);
CONFIG_DEFINE_HIDDEN("Codes", bool, DisableLowResolutionFontOnCustomUI, false);
CONFIG_DEFINE_HIDDEN("Codes", bool, RestoreContextualHUDColours, false);
CONFIG_DEFINE_HIDDEN("Codes", bool, DisableEdgeGrabLeftover, false);
CONFIG_DEFINE_HIDDEN("Codes", bool, TailsGauge, false);
CONFIG_DEFINE_HIDDEN("Codes", bool, MidairControlForMachSpeed, false);
CONFIG_DEFINE_HIDDEN("Codes", bool, MidairControlForSnowboards, false);

CONFIG_DEFINE("Update", time_t, LastChecked, 0);
