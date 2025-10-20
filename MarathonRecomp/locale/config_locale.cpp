#include <user/config.h>

/*
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!! LOCALISATION NOTES !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    - Ensure brand names are always presented on the same line.

      Correct:
      This is a string that contains a brand name like
      Xbox 360, which is one of the two consoles to have a port of
      SONIC THE HEDGEHOG.

      Incorrect:
      This is a string that contains a brand name like Xbox
      360, which is one of the two consoles to have a port of SONIC THE
      HEDGEHOG.

    - Ensure your locale is added in the correct order following the language enum.

      Correct:
      {
          { ELanguage::English,  "Example" },
          { ELanguage::Japanese, "Example" },
          { ELanguage::German,   "Example" },
          { ELanguage::French,   "Example" },
          { ELanguage::Spanish,  "Example" },
          { ELanguage::Italian,  "Example" }
      }

      Incorrect:
      {
          { ELanguage::English,  "Example" },
          { ELanguage::French,   "Example" },
          { ELanguage::Spanish,  "Example" },
          { ELanguage::German,   "Example" },
          { ELanguage::Italian,  "Example" },
          { ELanguage::Japanese, "Example" }
      }

    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
*/

#define CONFIG_DEFINE_LOCALE(name) \
    CONFIG_LOCALE g_##name##_locale =

#define CONFIG_DEFINE_ENUM_LOCALE(type) \
    CONFIG_ENUM_LOCALE(type) g_##type##_locale =

CONFIG_DEFINE_LOCALE(Language)
{
    { ELanguage::English,  { "Language", "Change the language used for text." } },
    { ELanguage::Japanese, { "言語", "DUMMY" } },
    { ELanguage::German,   { "Sprache", "Ändere die Sprache für Text." } },
    { ELanguage::French,   { "Langue", "Modifie la langue utilisée pour le texte." } },
    { ELanguage::Spanish,  { "Idioma", "Cambia el idioma utilizado para el texto." } },
    { ELanguage::Italian,  { "Lingua", "Cambia la lingua utilizzata per i testi." } }
};

// Notes: do not localise this.
CONFIG_DEFINE_ENUM_LOCALE(ELanguage)
{
    {
        ELanguage::English,
        {
            { ELanguage::English,  { "English", "" } },
            { ELanguage::Japanese, { "日本語", "" } },
            { ELanguage::German,   { "Deutsch", "" } },
            { ELanguage::French,   { "Français", "" } },
            { ELanguage::Spanish,  { "Español", "" } },
            { ELanguage::Italian,  { "Italiano", "" } }
        }
    }
};

CONFIG_DEFINE_LOCALE(VoiceLanguage)
{
    { ELanguage::English, { "Voice Language", "Change the language used for character voices." } },
    { ELanguage::Japanese, { "音声言語", "ゲーム内の音声言語を変更できます" } },
    { ELanguage::German,   { "Stimmeinstellung", "Ändere die Sprache, die für Charakterstimmen benutzt wird." } },
    { ELanguage::French,   { "Langue de doublage", "Modifie la langue utilisée pour la voix des personnages." } },
    { ELanguage::Spanish,  { "Idioma de voz", "Cambia el idioma utilizado para las voces de los personajes." } },
    { ELanguage::Italian,  { "Lingua delle voci", "Modifica la lingua utilizzata per le voci dei personaggi." } }
};

CONFIG_DEFINE_ENUM_LOCALE(EVoiceLanguage)
{
    {
        ELanguage::English,
        {
            { EVoiceLanguage::English,  { "English", "" } },
            { EVoiceLanguage::Japanese, { "Japanese", "" } }
        }
    },
    {
        ELanguage::Japanese,
        {
            { EVoiceLanguage::English,  { "英語", "" } },
            { EVoiceLanguage::Japanese, { "日本語", "" } }
        }
    },
    {
        ELanguage::German,
        {
            { EVoiceLanguage::English,  { "Englisch", "" } },
            { EVoiceLanguage::Japanese, { "Japanisch", "" } }
        }
    },
    {
        ELanguage::French,
        {
            { EVoiceLanguage::English,  { "Anglais", "" } },
            { EVoiceLanguage::Japanese, { "Japonais", "" } }
        }
    },
    {
        ELanguage::Spanish,
        {
            { EVoiceLanguage::English,  { "Inglés", "" } },
            { EVoiceLanguage::Japanese, { "Japonés", "" } }
        }
    },
    {
        ELanguage::Italian,
        {
            { EVoiceLanguage::English,  { "Inglese", "" } },
            { EVoiceLanguage::Japanese, { "Giapponese", "" } }
        }
    },
};

CONFIG_DEFINE_LOCALE(Subtitles)
{
    { ELanguage::English, { "Subtitles", "Show subtitles during dialogue." } },
    { ELanguage::Japanese, { "字幕", "字幕の表示を選択できます" } },
    { ELanguage::German,   { "Untertitel", "Zeige Untertitel bei Dialogen." } },
    { ELanguage::French,   { "Sous-titres", "Affiche les sous-titres pendant les dialogues." } },
    { ELanguage::Spanish,  { "Subtítulos", "Muestra subtítulos durante los diálogos." } },
    { ELanguage::Italian,  { "Sottotitoli", "Mostra i sottotitoli durante i dialoghi." } }
};

CONFIG_DEFINE_LOCALE(Hints)
{
    { ELanguage::English,  { "Hints", "Show hints during gameplay." } },
    { ELanguage::Japanese, { "ヒントリング", "ゲーム内にヒントリングを表示するか選択できます" } },
    { ELanguage::German,   { "Hinweise", "Zeige Hinweise während des Spiels." } },
    { ELanguage::French,   { "Indices", "Affiche les indices pendant le jeu." } },
    { ELanguage::Spanish,  { "Pistas", "Muestra pistas durante el juego." } },
    { ELanguage::Italian,  { "Indizi", "Mostra degli indizzi durante il gioco." } }
};

CONFIG_DEFINE_LOCALE(ControlTutorial)
{
    { ELanguage::English,  { "Control Tutorial", "Show controller hints during gameplay." } },
    { ELanguage::Japanese, { "アクションナビ", "ゲーム内にアクションナビを表示するか選択できます" } },
    { ELanguage::German,   { "Steuerungsanleitung", "Zeige Steuerungshinweise während des Spiels." } },
    { ELanguage::French,   { "Indication des commandes", "Affiche les indications des commandes pendant le jeu." } },
    { ELanguage::Spanish,  { "Tutorial de controles", "Muestra pistas de controles durante el juego." } },
    { ELanguage::Italian,  { "Tutorial dei comandi", "Mostra i tutorial dei comandi durante il gioco." } }
};

CONFIG_DEFINE_LOCALE(Autosave)
{
    { ELanguage::English,  { "Autosave", "Save the game automatically at manual save points." } },
    { ELanguage::Japanese, { "DUMMY", "DUMMY" } },
    { ELanguage::German,   { "DUMMY", "DUMMY" } },
    { ELanguage::French,   { "DUMMY", "DUMMY" } },
    { ELanguage::Spanish,  { "DUMMY", "DUMMY" } },
    { ELanguage::Italian,  { "DUMMY", "DUMMY" } }
};

CONFIG_DEFINE_LOCALE(AchievementNotifications)
{
    { ELanguage::English,  { "Achievement Notifications", "Show notifications for unlocking achievements. Achievements will still be rewarded with notifications disabled." } },
    { ELanguage::Japanese, { "実績通知", "実績通知の有無を選択できます オフにしても実績は付与されます" } },
    { ELanguage::German,   { "Erfolgsbenachrichtigungen", "Zeige Benachrichtigungen für das Freischalten von Erfolgen. Erfolge werden weiterhin freigeschaltet, auch wenn die Benachrichtigungen ausgeschaltet sind." } },
    { ELanguage::French,   { "Notification des succès", "Affiche les notifications pour le déverrouillage des succès. Les succès seront toujours obtenus même si les notifications sont désactivées." } },
    { ELanguage::Spanish,  { "Notificaciones de logros", "Muestra notificaciones al desbloquear logros. Los logros se seguirán obteniendo aunque las notificaciones estén desactivadas." } },
    { ELanguage::Italian,  { "Notifiche obiettivi", "Mostra delle notifiche quando sblocchi degli obiettivi. Gli obiettivi verranno comunque assegnati anche con le notifiche disattivate." } }
};

CONFIG_DEFINE_LOCALE(HorizontalCamera)
{
    { ELanguage::English,  { "Horizontal Camera", "Change how the camera moves left and right." } },
    { ELanguage::Japanese, { "カメラの左右", "カメラ左右の動く方向を選択できます" } },
    { ELanguage::German,   { "Horizontale Kamera", "Ändere wie sich die Kamera nach links und rechts bewegt." } },
    { ELanguage::French,   { "Caméra horizontale", "Modifie la rotation horizontale de la caméra." } },
    { ELanguage::Spanish,  { "Cámara horizontal", "Cambia cómo se mueve la camara hacia la izquierda y la derecha." } },
    { ELanguage::Italian,  { "Telecamera orizzontale", "Modifica come la telecamera si muove da sinistra a destra." } }
};

CONFIG_DEFINE_LOCALE(VerticalCamera)
{
    { ELanguage::English,  { "Vertical Camera", "Change how the camera moves up and down." } },
    { ELanguage::Japanese, { "カメラの上下", "カメラ上下の動く方向を選択できます" } },
    { ELanguage::German,   { "Vertikale Kamera", "Ändere wie sich die Kamera nach oben und unten bewegt." } },
    { ELanguage::French,   { "Caméra verticale", "Modifie la rotation verticale de la caméra." } },
    { ELanguage::Spanish,  { "Cámara vertical", "Cambia cómo se mueve la camara hacia arriba y abajo." } },
    { ELanguage::Italian,  { "Telecamera verticale", "Modifica come la telecamera si muove su e giù." } }
};

CONFIG_DEFINE_ENUM_LOCALE(ECameraRotationMode)
{
    {
        ELanguage::English,
        {
            { ECameraRotationMode::Normal,  { "Normal", "" } },
            { ECameraRotationMode::Reverse, { "Reverse", "" } }
        }
    },
    {
        ELanguage::Japanese,
        {
            { ECameraRotationMode::Normal,  { "ノーマル", "" } },
            { ECameraRotationMode::Reverse, { "リバース", "" } }
        }
    },
    {
        ELanguage::German,
        {
            { ECameraRotationMode::Normal,  { "Normal", "" } },
            { ECameraRotationMode::Reverse, { "Invertiert", "" } }
        }
    },
    {
        ELanguage::French,
        {
            { ECameraRotationMode::Normal,  { "Normale", "" } },
            { ECameraRotationMode::Reverse, { "Inversée", "" } }
        }
    },
    {
        ELanguage::Spanish,
        {
            { ECameraRotationMode::Normal,  { "Normal", "" } },
            { ECameraRotationMode::Reverse, { "Invertido", "" } }
        }
    },
    {
        ELanguage::Italian,
        {
            { ECameraRotationMode::Normal,  { "Normale", "" } },
            { ECameraRotationMode::Reverse, { "Invertita", "" } }
        }
    }
};

CONFIG_DEFINE_ENUM_LOCALE(EFaceButton)
{
    {
        ELanguage::English,
        {
            { EFaceButton::A, { "A", "" } },
            { EFaceButton::B, { "B", "" } },
            { EFaceButton::X, { "X", "" } },
            { EFaceButton::Y, { "Y", "" } }
        }
    }
};

CONFIG_DEFINE_LOCALE(Antigravity)
{
    { ELanguage::English,  { "Antigravity", "Change how Antigravity is activated." } },
    { ELanguage::Japanese, { "DUMMY", "DUMMY" } },
    { ELanguage::German,   { "DUMMY", "DUMMY" } },
    { ELanguage::French,   { "DUMMY", "DUMMY" } },
    { ELanguage::Spanish,  { "DUMMY", "DUMMY" } },
    { ELanguage::Italian,  { "DUMMY", "DUMMY" } }
};

CONFIG_DEFINE_LOCALE(LightDash)
{
    { ELanguage::English,  { "Light Dash", "Change how Light Dash is activated." } },
    { ELanguage::Japanese, { "DUMMY", "DUMMY" } },
    { ELanguage::German,   { "DUMMY", "DUMMY" } },
    { ELanguage::French,   { "DUMMY", "DUMMY" } },
    { ELanguage::Spanish,  { "DUMMY", "DUMMY" } },
    { ELanguage::Italian,  { "DUMMY", "DUMMY" } }
};

CONFIG_DEFINE_LOCALE(AllowBackgroundInput)
{
    { ELanguage::English,  { "Allow Background Input", "Allow controller input whilst the game window is unfocused." } },
    { ELanguage::Japanese, { "バックグラウンド入力", "フォーカスされていないゲームに入力できるか選択できます" } },
    { ELanguage::German,   { "Erlaube Hintergrundeingaben", "Erlaube Eingaben deines Controllers auch wenn das Spielfenster nicht fokussiert ist." } },
    { ELanguage::French,   { "Manette en arrière plan", "Permet d'utiliser la manette dans le jeu lorsque qu'il n'est pas au premier plan." } },
    { ELanguage::Spanish,  { "Control en segundo plano", "Permite controlar el juego con un mando mientras la ventana esté en segundo plano." } },
    { ELanguage::Italian,  { "Input con la finestra inattiva", "Attiva/disattiva i tasti del controller mentre la finestra è inattiva." } }
};

CONFIG_DEFINE_LOCALE(ControllerIcons)
{
    { ELanguage::English, { "Controller Icons", "Change the icons to match your controller." } },
    { ELanguage::Japanese, { "コントローラーアイコン", "ゲーム内のコントローラーアイコンを変更できます" } },
    { ELanguage::German,   { "Controllersymbole", "Ändere die Controllersymbole, um sie auf dein Modell anzupassen." } },
    { ELanguage::French,   { "Icône des boutons", "Modifie les icônes pour les faire correspondre à votre manette." } },
    { ELanguage::Spanish,  { "Iconos del mando", "Cambia los iconos para que coincidan con tu mando." } },
    { ELanguage::Italian,  { "Icone dei tasti", "Modifica le icone per farle corrispondere con il tuo controller." } }
};

CONFIG_DEFINE_ENUM_LOCALE(EControllerIcons)
{
    {
        ELanguage::English,
        {
            { EControllerIcons::Auto,        { "Auto", "Auto: the game will determine which icons to use based on the current input device." } },
            { EControllerIcons::Xbox,        { "Xbox", "" } },
            { EControllerIcons::PlayStation, { "PlayStation", "" } }
        }
    },
    {
        ELanguage::Japanese,
        {
            { EControllerIcons::Auto,        { "自動検出", "自動検出: コントローラーアイコンを使用している入力デバイスに応じて自動的に決定します" } },
            { EControllerIcons::Xbox,        { "Xbox", "" } },
            { EControllerIcons::PlayStation, { "PlayStation", "" } }
        }
    },
    {
        ELanguage::German,
        {
            { EControllerIcons::Auto,        { "Auto", "Auto: Das Spiel erkennt automatisch deinen Controller um die Symbole dementsprechend anzupassen." } },
            { EControllerIcons::Xbox,        { "Xbox", "" } },
            { EControllerIcons::PlayStation, { "PlayStation", "" } }
        }
    },
    {
        ELanguage::French,
        {
            { EControllerIcons::Auto,        { "Auto", "Auto : le jeu déterminera automatiquement quelles icônes utiliser en fonction du périphérique d'entrée." } },
            { EControllerIcons::Xbox,        { "Xbox", "" } },
            { EControllerIcons::PlayStation, { "PlayStation", "" } }
        }
    },
    {
        ELanguage::Spanish,
        {
            { EControllerIcons::Auto,        { "Auto", "Auto: el juego determinará de forma automática qué iconos utilizar dependiendo del dispositivo de entrada actual." } },
            { EControllerIcons::Xbox,        { "Xbox", "" } },
            { EControllerIcons::PlayStation, { "PlayStation", "" } }
        }
    },
    {
        ELanguage::Italian,
        {
            { EControllerIcons::Auto,        { "Auto", "Auto: il gioco determinerà quali icone da utilizzare in base al dispositivo di input attuale." } },
            { EControllerIcons::Xbox,        { "Xbox", "" } },
            { EControllerIcons::PlayStation, { "PlayStation", "" } }
        }
    }
};

CONFIG_DEFINE_LOCALE(MasterVolume)
{
    { ELanguage::English,  { "Master Volume", "Adjust the overall volume." } },
    { ELanguage::Japanese, { "マスターボリューム", "全体ボリュームの大きさを調整できます" } },
    { ELanguage::German,   { "Gesamtlautstärke", "Passe die Gesamtlautstärke an." } },
    { ELanguage::French,   { "Volume général", "Réglage du volume général." } },
    { ELanguage::Spanish,  { "Volumen maestro", "Ajusta el volumen general." } },
    { ELanguage::Italian,  { "Volume principale", "Regola il volume principale" } }
};

CONFIG_DEFINE_LOCALE(MusicVolume)
{
    { ELanguage::English,  { "Music Volume", "Adjust the volume for the music." } },
    { ELanguage::Japanese, { "BGMボリューム", "BGMボリュームの大きさを調整できます" } },
    { ELanguage::German,   { "Musiklautstärke", "Passe die Lautstärke der Musik an." } },
    { ELanguage::French,   { "Volume de la musique", "Réglage du volume de la musique." } },
    { ELanguage::Spanish,  { "Volumen de la música", "Ajusta el volumen de la música." } },
    { ELanguage::Italian,  { "Volume musica di sottofondo", "Regola il volume della musica di sottofondo." } }
};

CONFIG_DEFINE_LOCALE(EffectsVolume)
{
    { ELanguage::English,  { "Effects Volume", "Adjust the volume for sound effects." } },
    { ELanguage::Japanese, { "SEボリューム", "SEボリュームの大きさを調整できます" } },
    { ELanguage::German,   { "Soundeffektlautstärke", "Passe die Lautstärke der Soundeffekte an." } },
    { ELanguage::French,   { "Volume des effets sonores", "Réglage du volume des effets sonores." } },
    { ELanguage::Spanish,  { "Volumen de efectos", "Ajusta el volumen de los efectos de sonido." } },
    { ELanguage::Italian,  { "Volume effetti sonori", "Regola il volume degli effetti sonori." } }
};

CONFIG_DEFINE_LOCALE(ChannelConfiguration)
{
    { ELanguage::English,  { "Channel Configuration", "Change the output mode for your audio device." } },
    { ELanguage::Japanese, { "チャンネル設定", "オーディオデバイスの出力モードを変更できます" } },
    { ELanguage::German,   { "Kanalkonfiguration", "Ändere den Ausgabemodus für dein Audioausgabegerät." } },
    { ELanguage::French,   { "Configuration sortie audio", "Modifie le mode de sortie pour votre périphérique audio." } },
    { ELanguage::Spanish,  { "Configuración de canales", "Cambia el modo de salida para tu dispositivo de audio." } },
    { ELanguage::Italian,  { "Configurazione canali audio", "Modifica la modalità di output per il tuo dispositivo audio." } }
};

CONFIG_DEFINE_ENUM_LOCALE(EChannelConfiguration)
{
    {
        ELanguage::English,
        {
            { EChannelConfiguration::Stereo,   { "Stereo", "" } },
            { EChannelConfiguration::Surround, { "Surround", "" } }
        }
    },
    {
        ELanguage::Japanese,
        {
            { EChannelConfiguration::Stereo,   { "ステレオ", "" } },
            { EChannelConfiguration::Surround, { "サラウンド", "" } }
        }
    },
    {
        ELanguage::German,
        {
            { EChannelConfiguration::Stereo,   { "Stereo", "" } },
            { EChannelConfiguration::Surround, { "Surround", "" } }
        }
    },
    {
        ELanguage::French,
        {
            { EChannelConfiguration::Stereo,   { "Stéréo", "" } },
            { EChannelConfiguration::Surround, { "Surround", "" } }
        }
    },
    {
        ELanguage::Spanish,
        {
            { EChannelConfiguration::Stereo,   { "Estéreo", "" } },
            { EChannelConfiguration::Surround, { "Envolvente", "" } }
        }
    },
    {
        ELanguage::Italian,
        {
            { EChannelConfiguration::Stereo,   { "Stereo", "" } },
            { EChannelConfiguration::Surround, { "Surround", "" } }
        }
    }
};

CONFIG_DEFINE_LOCALE(MuteOnFocusLost)
{
    { ELanguage::English,  { "Mute on Focus Lost", "Mute the game's audio when the window is not in focus." } },
    { ELanguage::Japanese, { "DUMMY", "DUMMY" } },
    { ELanguage::German,   { "DUMMY", "DUMMY" } },
    { ELanguage::French,   { "DUMMY", "DUMMY" } },
    { ELanguage::Spanish,  { "DUMMY", "DUMMY" } },
    { ELanguage::Italian,  { "DUMMY", "DUMMY" } }
};

CONFIG_DEFINE_LOCALE(MusicAttenuation)
{
    { ELanguage::English,  { "Music Attenuation", "Fade out the game's music when external media is playing." } },
    { ELanguage::Japanese, { "BGM減衰", "外部メディアを再生するとゲームの音楽をフェードアウトします" } },
    { ELanguage::German,   { "Musikdämpfung", "Stelle die Musik des Spiels stumm während externe Medien abgespielt werden." } },
    { ELanguage::French,   { "Atténuation audio", "Abaisse le volume des musiques du jeu lorsqu'un média externe est en cours de lecture." } },
    { ELanguage::Spanish,  { "Atenuación de música", "Atenúa la música del juego cuando un reproductor multimedia se encuentra activo." } },
    { ELanguage::Italian,  { "Attenuazione musica", "Abbassa il volume della musica di sottofondo quando un'altra applicazione riproduce dei suoni." } }
};

CONFIG_DEFINE_LOCALE(WindowSize)
{
    { ELanguage::English,  { "Window Size", "Adjust the size of the game window in windowed mode." } },
    { ELanguage::Japanese, { "ウィンドウサイズ", "ウィンドウモードでのゲームのウィンドウサイズを調整できます" } },
    { ELanguage::German,   { "Fenstergröße", "Ändere die Größe des Spielfensters im Fenstermodus." } },
    { ELanguage::French,   { "Taille de la fenêtre", "Modifie la taille de la fenêtre de jeu en mode fenêtré." } },
    { ELanguage::Spanish,  { "Tamaño de ventana", "Ajusta el tamaño de la ventana de juego." } },
    { ELanguage::Italian,  { "Dimensioni finestra", "Regola le dimensioni della finestra del gioco in modalità finestra." } }
};

CONFIG_DEFINE_LOCALE(Monitor)
{
    { ELanguage::English,  { "Monitor", "Change which monitor to display the game on." } },
    { ELanguage::Japanese, { "モニター選択", "ゲームを表示するモニターを変更できます" } },
    { ELanguage::German,   { "Monitor", "Ändere auf welchem Monitor das Spiel angezeigt wird." } },
    { ELanguage::French,   { "Moniteur", "Change le moniteur sur lequel le jeu sera affiché." } },
    { ELanguage::Spanish,  { "Pantalla", "Cambia la pantalla en la cuál se muestra el juego." } },
    { ELanguage::Italian,  { "Schermo", "Cambia lo schermo su cui visualizzare il gioco." } }
};

CONFIG_DEFINE_LOCALE(AspectRatio)
{
    { ELanguage::English,  { "Aspect Ratio", "Change the aspect ratio." } },
    { ELanguage::Japanese, { "アスペクト比", "アスペクト比を変更できます" } },
    { ELanguage::German,   { "Seitenverhältnis", "Verändere das Seitenverhältnis." } },
    { ELanguage::French,   { "Format d'image", "Modifie le format d'image." } },
    { ELanguage::Spanish,  { "Relación de aspecto", "Cambia la relación de aspecto." } },
    { ELanguage::Italian,  { "Rapporto d'aspetto", "Modifica il rapporto d'aspetto." } }
};

CONFIG_DEFINE_ENUM_LOCALE(EAspectRatio)
{
    {
        ELanguage::English,
        {
            { EAspectRatio::Auto,     { "Auto", "Auto: the aspect ratio will dynamically adjust to the window size." } },
            { EAspectRatio::Original, { "Original", "Original: locks the game to a widescreen aspect ratio." } }
        }
    },
    {
        ELanguage::Japanese,
        {
            { EAspectRatio::Auto,     { "自動", "自動: アスペクト比はウィンドウサイズに合わせて調整されます" } },
            { EAspectRatio::Original, { "オリジナル", "オリジナル: ワイドスクリーンのアスペクト比に固定されます" } }
        }
    },
    {
        ELanguage::German,
        {
            { EAspectRatio::Auto,     { "Auto", "Auto: Das Seitenverhältnis passt sich automatisch der Fenstergröße an." } },
            { EAspectRatio::Original, { "Original", "Original: Stellt das Spiel in einem Breitbildschirm-Format dar." } }
        }
    },
    {
        ELanguage::French,
        {
            { EAspectRatio::Auto,     { "Auto", "Auto : le format d'image s'adapte automatiquement à la taille de la fenêtre." } },
            { EAspectRatio::Original, { "Original", "Original : force le jeu sur un format d'image large." } }
        }
    },
    {
        ELanguage::Spanish,
        {
            { EAspectRatio::Auto,     { "Auto", "Auto: la relación de aspecto se ajusta de forma dinámica al tamaño de la ventana." } },
            { EAspectRatio::Original, { "Original", "Original: muestra el juego en relación de aspecto de pantalla ancha." } }
        }
    },
    {
        ELanguage::Italian,
        {
            { EAspectRatio::Auto,     { "Auto", "Auto: il rapporto d'aspetto verra cambiato automaticamente in base alle dimensioni della finestra." } },
            { EAspectRatio::Original, { "Originale", "Originale: blocca il gioco a un rapporto d'aspetto widescreen." } }
        }
    }
};

CONFIG_DEFINE_LOCALE(ResolutionScale)
{
    { ELanguage::English,  { "Resolution Scale", "Adjust the internal resolution of the game." } },
    { ELanguage::Japanese, { "解像度スケール", "ゲームの内部解像度を調整できます" } },
    { ELanguage::German,   { "Rendering-Auflösung", "Passe die Auflösung der internen Darstellung an." } },
    { ELanguage::French,   { "Échelle de rendu", "Modifie la résolution interne du jeu." } },
    { ELanguage::Spanish,  { "Escala de resolución", "Ajusta la resolución interna del juego." } },
    { ELanguage::Italian, { "Scala risoluzione", "Regola la risoluzione interna del gioco." } }
};

CONFIG_DEFINE_LOCALE(Fullscreen)
{
    { ELanguage::English,  { "Fullscreen", "Toggle between borderless fullscreen or windowed mode." } },
    { ELanguage::Japanese, { "フルスクリーン", "ボーダーレスフルスクリーンかウィンドウモードを選択できます" } },
    { ELanguage::German,   { "Vollbild", "Wechsle zwischen dem randlosen Vollbildmodus und dem Fenstermodus." } },
    { ELanguage::French,   { "Plein écran", "Alterne entre le mode plein écran sans bordures et le mode fenêtré." } },
    { ELanguage::Spanish,  { "Pantalla completa", "Cambia entre modo de pantalla completa o ventana." } },
    { ELanguage::Italian,  { "Schermo pieno", "Attiva/disattiva tra modalità finestra senza cornice e modalità finestra." } }
};

CONFIG_DEFINE_LOCALE(VSync)
{
    { ELanguage::English,  { "V-Sync", "Synchronize the game to the refresh rate of the display to prevent screen tearing." } },
    { ELanguage::Japanese, { "垂直同期", "垂直同期の設定を変更できます" } },
    { ELanguage::German,   { "V-Sync", "Synchronisiere das Spiel mit der Bildwiederholrate deines Bildschirms um Bildverzerrungen zu vermeiden." } },
    { ELanguage::French,   { "V-Sync", "Synchronise le jeu avec la fréquence de rafraîchissement de l'écran pour éviter le screen tearing." } },
    { ELanguage::Spanish,  { "V-Sync", "Sincroniza el juego a la tasa de refresco de la pantalla para prevenir el rasgado de la imagen." } },
    { ELanguage::Italian,  { "V-Sync", "Sincronizza il gioco con la frequenza d'aggiornamento del display per evitare lo screen tearing." } }
};

CONFIG_DEFINE_LOCALE(FPS)
{
    { ELanguage::English,  { "FPS", "Set the max frame rate the game can run at. WARNING: this may introduce glitches at frame rates other than 60 FPS." } },
    { ELanguage::Japanese, { "フレームレート上限", "DUMMY" } },
    { ELanguage::German,   { "FPS", "DUMMY" } },
    { ELanguage::French,   { "IPS", "DUMMY" } },
    { ELanguage::Spanish,  { "FPS", "DUMMY" } },
    { ELanguage::Italian,  { "FPS", "DUMMY" } }
};

CONFIG_DEFINE_LOCALE(Brightness)
{
    { ELanguage::English,  { "Brightness", "Adjust the brightness level." } },
    { ELanguage::Japanese, { "明るさの設定", "DUMMY" } },
    { ELanguage::German,   { "Helligkeit", "DUMMY" } },
    { ELanguage::French,   { "Luminosité", "DUMMY" } },
    { ELanguage::Spanish,  { "Brillo", "DUMMY" } },
    { ELanguage::Italian,  { "Luminosità", "DUMMY" } }
};

CONFIG_DEFINE_LOCALE(AntiAliasing)
{
    { ELanguage::English,  { "Anti-Aliasing", "Adjust the amount of smoothing applied to jagged edges." } },
    { ELanguage::Japanese, { "アンチエイリアス", "アンチエイリアスの種類を選択できます" } },
    { ELanguage::German,   { "Kantenglättung", "Passe die Menge an Kantenglättung an." } },
    { ELanguage::French,   { "Anticrénelage", "Ajuste le niveau d'anticrénelage appliqué aux bords des objets." } },
    { ELanguage::Spanish,  { "Anti-Aliasing", "Ajusta el nivel de suavizado aplicado a los dientes de sierra." } },
    { ELanguage::Italian,  { "Anti-Aliasing", "Regola la quantità di smussamento applicata ai bordi." } }
};

CONFIG_DEFINE_ENUM_LOCALE(EAntiAliasing)
{
    {
        ELanguage::English,
        {
            { EAntiAliasing::Off, { "Off", "" } }
        }
    },
    {
        ELanguage::Japanese,
        {
            { EAntiAliasing::Off, { "オフ", "" } },
        }
    },
    {
        ELanguage::German,
        {
            { EAntiAliasing::Off, { "Aus", "" } }
        }
    },
    {
        ELanguage::French,
        {
            { EAntiAliasing::Off, { "Non", "" } }
        }
    },
    {
        ELanguage::Spanish,
        {
            { EAntiAliasing::Off, { "No", "" } },
        }
    },
    {
        ELanguage::Italian,
        {
            { EAntiAliasing::Off, { "No", "" } }
        }
    }
};

CONFIG_DEFINE_LOCALE(TransparencyAntiAliasing)
{
    { ELanguage::English,  { "Transparency Anti-Aliasing", "Apply anti-aliasing to alpha transparent textures." } },
    { ELanguage::Japanese, { "透明度のアンチエイリアス", "透過テクスチャにアンチエイリアスを適用します" } },
    { ELanguage::German,   { "Transparenz-Kantenglättung", "Wende Kantenglättung auf Alpha-Transparenz-Texturen an." } },
    { ELanguage::French,   { "Anticrénelage de transparence", "Applique l'anticrénelage sur les textures transparentes." } },
    { ELanguage::Spanish,  { "Anti-Aliasing de transparencias", "Aplica antialiasing a las texturas transparentes." } },
    { ELanguage::Italian,  { "Anti-Aliasing su texture trasparenti", "Applica l'anti-aliasing alle texture trasparenti." } }
};

CONFIG_DEFINE_LOCALE(ShadowResolution)
{
    { ELanguage::English,  { "Shadow Resolution", "Set the resolution of real-time shadows." } },
    { ELanguage::Japanese, { "影の解像度", "影の解像度を設定できます" } },
    { ELanguage::German,   { "Schattenauflösung", "Stelle die Auflösung der Echtzeit-Schatten ein." } },
    { ELanguage::French,   { "Résolution des ombres", "Définit la résolution des ombres en temps réel." } },
    { ELanguage::Spanish,  { "Resolución de sombras", "Establece la resolución de las sombras de tiempo real." } },
    { ELanguage::Italian,  { "Risoluzione ombre", "Imposta la risoluzioni delle ombre in tempo reale." } }
};

CONFIG_DEFINE_ENUM_LOCALE(EShadowResolution) {};

CONFIG_DEFINE_LOCALE(ReflectionResolution)
{
    { ELanguage::English,  { "Reflection Resolution", "Set the resolution of real-time reflections." } },
    { ELanguage::Japanese, { "DUMMY", "DUMMY" } },
    { ELanguage::German,   { "DUMMY", "DUMMY" } },
    { ELanguage::French,   { "DUMMY", "DUMMY" } },
    { ELanguage::Spanish,  { "DUMMY", "DUMMY" } },
    { ELanguage::Italian,  { "DUMMY", "DUMMY" } }
};

CONFIG_DEFINE_ENUM_LOCALE(EReflectionResolution)
{
    {
        ELanguage::English,
        {
            { EReflectionResolution::Eighth, { "12.5%", "" } },
            { EReflectionResolution::Quarter, { "25%", "" } },
            { EReflectionResolution::Half, { "50%", "" } },
            { EReflectionResolution::Full, { "100%", "" } }
        }
    }
};

CONFIG_DEFINE_LOCALE(RadialBlur)
{
    { ELanguage::English,  { "Radial Blur", "Change the quality of the radial blur." } },
    { ELanguage::Japanese, { "DUMMY", "DUMMY" } },
    { ELanguage::German,   { "DUMMY", "DUMMY" } },
    { ELanguage::French,   { "DUMMY", "DUMMY" } },
    { ELanguage::Spanish,  { "DUMMY", "DUMMY" } },
    { ELanguage::Italian,  { "DUMMY", "DUMMY" } }
};

CONFIG_DEFINE_ENUM_LOCALE(ERadialBlur)
{
    {
        ELanguage::English,
        {
            { ERadialBlur::Off,      { "Off", "" } },
            { ERadialBlur::Original, { "Original", "" } },
            { ERadialBlur::Enhanced, { "Enhanced", "Enhanced: uses more samples for smoother radial blur." } }
        }
    },
    {
        ELanguage::Japanese,
        {
            { ERadialBlur::Off,      { "オフ", "" } },
            { ERadialBlur::Original, { "オリジナル", "" } },
            { ERadialBlur::Enhanced, { "エンハンスド", "DUMMY" } }
        }
    },
    {
        ELanguage::German,
        {
            { ERadialBlur::Off,      { "Aus", "" } },
            { ERadialBlur::Original, { "Original", "" } },
            { ERadialBlur::Enhanced, { "Verbessert", "DUMMY" } }
        }
    },
    {
        ELanguage::French,
        {
            { ERadialBlur::Off,      { "Non", "" } },
            { ERadialBlur::Original, { "Original", "" } },
            { ERadialBlur::Enhanced, { "Amélioré", "DUMMY" } }
        }
    },
    {
        ELanguage::Spanish,
        {
            { ERadialBlur::Off,      { "No", "" } },
            { ERadialBlur::Original, { "Original", "" } },
            { ERadialBlur::Enhanced, { "Mejorado", "DUMMY" } }
        }
    },
    {
        ELanguage::Italian,
        {
            { ERadialBlur::Off,      { "No", "" } },
            { ERadialBlur::Original, { "Originale", "" } },
            { ERadialBlur::Enhanced, { "Aumentato", "DUMMY" } }
        }
    }
};

CONFIG_DEFINE_LOCALE(CutsceneAspectRatio)
{
    { ELanguage::English,  { "Cutscene Aspect Ratio", "Change the aspect ratio of the real-time cutscenes." } },
    { ELanguage::Japanese, { "アスペクト比のカットシーン", "リアルタイムカットシーンのアスペクト比を変更できます" } },
    { ELanguage::German,   { "Zwischensequenz-Seitenverhältnis", "Verändere das Seitenverhältnis der Echtzeit-Zwischensequenzen." } },
    { ELanguage::French,   { "Format des cinématiques", "Modifie le format d'image des cinématiques en temps réel." } },
    { ELanguage::Spanish,  { "Relación de aspecto de cinemáticas", "Cambia la relación de aspecto de las cinemáticas de tiempo real." } },
    { ELanguage::Italian,  { "Rapporto d'aspetto dei filmati", "Cambia il rapporto d'aspetto dei filmati in tempo reale." } }
};

CONFIG_DEFINE_ENUM_LOCALE(ECutsceneAspectRatio)
{
    {
        ELanguage::English,
        {
            { ECutsceneAspectRatio::Original, { "Original", "Original: locks cutscenes to their original 16:9 aspect ratio." } },
            { ECutsceneAspectRatio::Unlocked, { "Unlocked", "Unlocked: allows cutscenes to adjust their aspect ratio to the window size. WARNING: this will introduce visual oddities past the original 16:9 aspect ratio." } }
        }
    },
    {
        ELanguage::Japanese,
        {
            { ECutsceneAspectRatio::Original, { "オリジナル", "オリジナル: カットシーンを元の16:9のアスペクト比に固定します" } },
            { ECutsceneAspectRatio::Unlocked, { "解除", "解除: カットシーンのアスペクト比をウィンドウサイズに合わせて調整します 警告: 元の16:9のアスペクト比を超えると視覚的な異常が発生します" } },
        }
    },
    {
        ELanguage::German,
        {
            { ECutsceneAspectRatio::Original, { "Original", "Original: Behält die Zwischensequenzen in ihrem originalen 16:9 Seitenverhältnis." } },
            { ECutsceneAspectRatio::Unlocked, { "Entsperrt", "Entsperrt: Erlaubt Zwischensequenzen ihr Seitenverhältnis der Fenstergröße anzupassen.\n\nWARNUNG: Diese Option kann zu visuellen Fehlern außerhalb der Grenzen des ursprünglichen Seitenverhältnisses führen." } }
        }
    },
    {
        ELanguage::French,
        {
            { ECutsceneAspectRatio::Original, { "Original", "Original : force les cinématiques dans leur format 16:9 d'origine." } },
            { ECutsceneAspectRatio::Unlocked, { "Libre", "Libre : permet aux cinématiques d'adapter leur format d'image à la taille de la fenêtre.\n\nATTENTION : au delà du format 16:9 d'origine, des bugs visuels apparaitront." } },
        }
    },
    {
        ELanguage::Spanish,
        {
            { ECutsceneAspectRatio::Original, { "Original", "Original: muestra las cinemáticas en su relación de aspecto original de 16:9." } },
            { ECutsceneAspectRatio::Unlocked, { "Desbloqueado", "Desbloqueado: permite que las cinemáticas ajusten su relación de aspecto al tamaño de la ventana.\n\nADVERTENCIA: esto introducirá rarezas visuales más allá de la relación de aspecto original de 16:9." } },
        }
    },
    {
        ELanguage::Italian,
        {
            { ECutsceneAspectRatio::Original, { "Originale", "Originale: blocca il rapporto d'aspetto dei filmati a 16:9." } },
            { ECutsceneAspectRatio::Unlocked, { "Sbloccato", "Sbloccato: il rapporto d'aspetto verrà regolato in base alle dimensioni della finestra.\n\nATTENZIONE: questa opzione potrebbe causare dei problemi visivi se il rapporto d'aspetto è oltre 16:9." } }
        }
    }
};

CONFIG_DEFINE_LOCALE(UIAlignmentMode)
{
    { ELanguage::English,  { "UI Alignment Mode", "Change how the UI aligns with the display." } },
    { ELanguage::Japanese, { "UIアライメントモード", "UIとディスプレイの配置を変更できます" } },
    { ELanguage::German,   { "Benutzeroberflächenausrichtung", "Verändere wie die Benutzeroberfläche sich mit dem Bildschirm ausrichtet." } },
    { ELanguage::French,   { "Alignement de l'IU", "Modifie l'alignement de l'interface utilisateur sur l'écran." } },
    { ELanguage::Spanish,  { "Modo de alineamiento de UI", "Cambia la alineación de la interfaz de usuario con la pantalla." } },
    { ELanguage::Italian,  { "Modalità allineamento interfaccia", "Modifica come l'interfaccia si allinea con lo schermo." } }
};

CONFIG_DEFINE_ENUM_LOCALE(EUIAlignmentMode)
{
    {
        ELanguage::English,
        {
            { EUIAlignmentMode::Edge,    { "Edge", "Edge: the UI will align with the edges of the display." } },
            { EUIAlignmentMode::Centre,  { "Center", "Center: the UI will align with the center of the display." } }
        }
    },
    {
        ELanguage::Japanese,
        {
            { EUIAlignmentMode::Edge,    { "エッジ", "エッジ: UIがディスプレイの端に揃います" } },
            { EUIAlignmentMode::Centre,  { "センター", "センター: UIがディスプレイの中央に揃います" } },
        }
    },
    {
        ELanguage::German,
        {
            { EUIAlignmentMode::Edge,    { "Rand", "Rand: Die Benutzeroberfläche richtet sich zum Rand des Bildschirms aus." } },
            { EUIAlignmentMode::Centre,  { "Mitte", "Mitte: Die Benutzeroberfläche richtet sich zur Mitte des Bildschirms aus." } }
        }
    },
    {
        ELanguage::French,
        {
            { EUIAlignmentMode::Edge,    { "Bord", "Bord : l'interface utilisateur sera alignée sur les bords de l'écran." } },
            { EUIAlignmentMode::Centre,  { "Centrée", "Centrée : l'interface utilisateur sera alignée sur le centre de l'écran." } },
        }
    },
    {
        ELanguage::Spanish,
        {
            { EUIAlignmentMode::Edge,    { "Borde", "Borde: la interfaz de usuario se alineará con los bordes de la pantalla." } },
            { EUIAlignmentMode::Centre,  { "Centro", "Centro: la interfaz de usuario se alineará con el centro de la pantalla." } },
        }
    },
    {
        ELanguage::Italian,
        {
            { EUIAlignmentMode::Edge,    { "Bordi", "Bordi: l'interfaccia si allineerà con i bordi dello schermo." } },
            { EUIAlignmentMode::Centre,  { "Centro", "Centro: l'interfaccia si allineerà con il centro dello schermo." } }
        }
    }
};
