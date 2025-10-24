#include <user/config.h>
#include <locale/locale.h>

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

std::unordered_map<std::string_view, std::unordered_map<ELanguage, std::string>> g_locale =
{
    {
        "Options_Header_Name",
        {
            { ELanguage::English,  "OPTIONS" },
            { ELanguage::Japanese, "OPTIONS" },
            { ELanguage::German,   "OPTIONEN" },
            { ELanguage::French,   "OPTIONS" },
            { ELanguage::Spanish,  "OPCIONES" },
            { ELanguage::Italian,  "OPZIONI" }
        }
    },
    {
        "Options_MainMenu_Description",
        {
            { ELanguage::English,  "Options: Adjust various game settings" },
            { ELanguage::Japanese, "オプション：各種ゲームの設定を行います" },
            { ELanguage::German,   "Optionen: Spieleinstellungen ändern" },
            { ELanguage::French,   "Options : effectuer divers paramétrages" },
            { ELanguage::Spanish,  "Opciones: cambiar opciones de juego. " },
            { ELanguage::Italian,  "Opzioni: regola impostazioni di gioco." }
        }
    },
    {
        "Options_Category_System",
        {
            { ELanguage::English,  "System settings" },
            { ELanguage::Japanese, "DUMMY" },
            { ELanguage::German,   "System-Einstellungen" },
            { ELanguage::French,   "DUMMY" },
            { ELanguage::Spanish,  "Configuración del sistema" },
            { ELanguage::Italian,  "Impostazioni sistema" }
        }
    },
    {
        "Options_Desc_Category_System",
        {
            { ELanguage::English,  "Adjust system settings." },
            { ELanguage::Japanese, "DUMMY" },
            { ELanguage::German,   "Verändere System-Einstellungen." },
            { ELanguage::French,   "DUMMY" },
            { ELanguage::Spanish,  "Ajustar la configuración del sistema." },
            { ELanguage::Italian,  "Modifica le impostazioni di sistema." }
        }
    },
    {
        "Options_Category_Input",
        {
            { ELanguage::English,  "Input settings" },
            { ELanguage::Japanese, "DUMMY" },
            { ELanguage::German,   "Eingabe-Einstellungen" },
            { ELanguage::French,   "DUMMY" },
            { ELanguage::Spanish,  "Configuración de entrada" },
            { ELanguage::Italian,  "Impostazioni input" }
        }
    },
    {
        "Options_Desc_Category_Input",
        {
            { ELanguage::English,  "Adjust input settings." },
            { ELanguage::Japanese, "DUMMY" },
            { ELanguage::German,   "Verändere Eingabe-Einstellungen." },
            { ELanguage::French,   "DUMMY" },
            { ELanguage::Spanish,  "Ajustar la configuración de entrada." },
            { ELanguage::Italian,  "Modifica le impostazioni input." }
        }
    },
    {
        "Options_Category_Audio",
        {
            { ELanguage::English,  "Audio settings" },
            { ELanguage::Japanese, "DUMMY" },
            { ELanguage::German,   "Audio-Einstellungen" },
            { ELanguage::French,   "DUMMY" },
            { ELanguage::Spanish,  "Configuración de audio" },
            { ELanguage::Italian,  "Impostazioni audio" }
        }
    },
    {
        "Options_Desc_Category_Audio",
        {
            { ELanguage::English,  "Adjust audio settings." },
            { ELanguage::Japanese, "DUMMY" },
            { ELanguage::German,   "Verändere Audio-Einstellungen." },
            { ELanguage::French,   "DUMMY" },
            { ELanguage::Spanish,  "Ajustar la configuración de audio." },
            { ELanguage::Italian,  "Modifica le impostazioni audio." }
        }
    },
    {
        "Options_Category_Video",
        {
            { ELanguage::English,  "Video settings" },
            { ELanguage::Japanese, "DUMMY" },
            { ELanguage::German,   "Video-Einstellungen" },
            { ELanguage::French,   "DUMMY" },
            { ELanguage::Spanish,  "Configuración de vídeo" },
            { ELanguage::Italian,  "Impostazioni video" }
        }
    },
    {
        "Options_Desc_Category_Video",
        {
            { ELanguage::English,  "Adjust video settings." },
            { ELanguage::Japanese, "DUMMY" },
            { ELanguage::German,   "Verändere Video-Einstellungen." },
            { ELanguage::French,   "DUMMY" },
            { ELanguage::Spanish,  "Ajustar la configuración de vídeo." },
            { ELanguage::Italian,  "Modifica le impostazioni video." }
        }
    },
    {
        "Options_Category_Debug",
        {
            { ELanguage::English,  "Debug settings" },
            { ELanguage::Japanese, "DUMMY" },
            { ELanguage::German,   "Debug-Einstellungen" },
            { ELanguage::French,   "DUMMY" },
            { ELanguage::Spanish,  "Configuración de depuración" },
            { ELanguage::Italian,  "Impostazioni debug" }
        }
    },
    {
        "Options_Desc_Category_Debug",
        {
            { ELanguage::English,  "Adjust debug settings." },
            { ELanguage::Japanese, "DUMMY" },
            { ELanguage::German,   "Verändere Debug-Einstellungen." },
            { ELanguage::French,   "DUMMY" },
            { ELanguage::Spanish,  "Ajustar la configuración de depuración." },
            { ELanguage::Italian,  "Modifica le impostazioni debug." }
        }
    },
    {
        // Notes: integer values in the options menu (e.g. FPS) when at their maximum value.
        "Options_Value_Max",
        {
            { ELanguage::English,  "MAX" },
            { ELanguage::Japanese, "MAX" },
            { ELanguage::German,   "MAX" },
            { ELanguage::French,   "MAX" },
            { ELanguage::Spanish,  "MÁX" },
            { ELanguage::Italian,  "MAX" }
        }
    },
    {
        "Options_Name_WindowSize",
        {
            { ELanguage::English,  "Window Size" },
            { ELanguage::Japanese, "ウィンドウサイズ" },
            { ELanguage::German,   "Fenstergröße" },
            { ELanguage::French,   "Taille de la fenêtre" },
            { ELanguage::Spanish,  "Tamaño de ventana" },
            { ELanguage::Italian,  "Dimensioni della finestra" }
        }
    },
    {
        "Options_Desc_WindowSize",
        {
            { ELanguage::English,  "Adjust the size of the game window in windowed mode." },
            { ELanguage::Japanese, "ゲームのウィンドウサイズを設定できます" },
            { ELanguage::German,   "Passe die Fenstergröße des Spiels im Fenstermodus an." },
            { ELanguage::French,   "Définir la résolution de jeu en mode fenêtré." },
            { ELanguage::Spanish,  "Ajusta el tamaño de la ventana de juego en modo ventana." },
            { ELanguage::Italian,  "Regola la dimensione della finestra del gioco in modalità finestra." }
        }
    },
    {
        // Notes: description for options that cannot be accessed during gameplay (e.g. Language).
        "Options_Desc_NotAvailable",
        {
            { ELanguage::English,  "This option is not available during gameplay." },
            { ELanguage::Japanese, "DUMMY" },
            { ELanguage::German,   "Diese Einstellung kann während des Spiels nicht verändert werden." },
            { ELanguage::French,   "DUMMY" },
            { ELanguage::Spanish,  "Esta opción no está disponible durante la partida." },
            { ELanguage::Italian,  "Questa opzione non è disponibile durante il gioco." }
        }
    },
    {
        // Notes: description for options that are not implemented yet in development builds.
        "Options_Desc_NotImplemented",
        {
            { ELanguage::English,  "This option is not implemented yet." },
            { ELanguage::Japanese, "DUMMY" },
            { ELanguage::German,   "Diese Einstellung wurde noch nicht implementiert." },
            { ELanguage::French,   "DUMMY" },
            { ELanguage::Spanish,  "Esta opción aún no está implementada." },
            { ELanguage::Italian,  "Questa opzione non è ancora implementata." }
        }
    },
    {
        // Notes: currently the description for Window Size when in fullscreen.
        "Options_Desc_NotAvailableFullscreen",
        {
            { ELanguage::English,  "This option is not available in fullscreen mode." },
            { ELanguage::Japanese, "このオプションはフルスクリーンモードでは変更できません" },
            { ELanguage::German,   "Diese Option ist im Vollbildmodus nicht verfügbar." },
            { ELanguage::French,   "Cette option n'est pas disponible en mode plein écran." },
            { ELanguage::Spanish,  "Esta opción no está disponible en modo pantalla completa." },
            { ELanguage::Italian,  "Questa opzione non è disponibile in modalità schermo pieno." }
        }
    },
    {
        // Notes: currently the description for Monitor when in fullscreen.
        "Options_Desc_NotAvailableWindowed",
        {
            { ELanguage::English,  "This option is not available in windowed mode." },
            { ELanguage::Japanese, "このオプションはウィンドウモードでは変更できません" },
            { ELanguage::German,   "Diese Option ist im Fenstermodus nicht verfügbar." },
            { ELanguage::French,   "Cette option n'est pas disponible en mode fenêtré." },
            { ELanguage::Spanish,  "Esta opción no está disponible en modo ventana." },
            { ELanguage::Italian,  "Questa opzione non è disponibile in modalità finestra." }
        }
    },
    {
        // Notes: currently the description for Monitor when the user only has one display connected.
        "Options_Desc_NotAvailableHardware",
        {
            { ELanguage::English,  "This option is not available with your current hardware configuration." },
            { ELanguage::Japanese, "このオプションは現在のハードウェア構成で変更できません" },
            { ELanguage::German,   "Diese Option ist mit der momentanen Hardwarekonfiguration nicht verfügbar." },
            { ELanguage::French,   "Cette option n'est pas disponible avec votre configuration matérielle actuelle." },
            { ELanguage::Spanish,  "Esta opción no está disponible con tu configuración actual de hardware." },
            { ELanguage::Italian,  "Questa opzione non è disponibile con l'hardware in tuo possesso." }
        }
    },
    {
        // Notes: description for Transparency Anti-Aliasing when MSAA is disabled.
        "Options_Desc_NotAvailableMSAA",
        {
            { ELanguage::English,  "This option is not available without MSAA." },
            { ELanguage::Japanese, "このオプションはMSAAなしで変更できません" },
            { ELanguage::German,   "Diese Option ist ohne MSAA nicht verfügbar." },
            { ELanguage::French,   "Cette option n'est pas disponible sans MSAA." },
            { ELanguage::Spanish,  "Esta opción no está disponible sin MSAA." },
            { ELanguage::Italian,  "Questa opzione non è disponibile senza MSAA." }
        }
    },
    {
        // Notes: description for Music Attenuation when the user is not running a supported OS.
        "Options_Desc_OSNotSupported",
        {
            { ELanguage::English,  "This option is not supported by your operating system." },
            { ELanguage::Japanese, "このオプションは現在のOSで変更できません" },
            { ELanguage::German,   "Diese Option wird von diesem Betriebssystem nicht unterstützt." },
            { ELanguage::French,   "Cette option n'est pas prise en charge par votre système d'exploitation." },
            { ELanguage::Spanish,  "Esta opción no es compatible con tu sistema operativo." },
            { ELanguage::Italian,  "Questa opzione non è disponibile con il tuo sistema operativo." }
        }
    },
    {
        "Options_Message_Restart",
        {
            { ELanguage::English,  "The game needs to restart to apply\nthe following changes. OK?\n" },
            { ELanguage::Japanese, "DUMMY\n" },
            { ELanguage::German,   "DUMMY\n" },
            { ELanguage::French,   "DUMMY\n" },
            { ELanguage::Spanish,  "Se necesita reiniciar el juego para\naplicar los siguientes cambios. ¿OK?\n" },
            { ELanguage::Italian,  "Il gioco deve essere riavviato per\napplicare le seguenti modifiche. OK?\n" }
        }
    },
    {
        // Notes: used for the button guide at the pause menu.
        "Achievements_Name",
        {
            { ELanguage::English,  "Achievements" },
            { ELanguage::Japanese, "実績" },
            { ELanguage::German,   "Erfolge" },
            { ELanguage::French,   "Succès" },
            { ELanguage::Spanish,  "Logros" },
            { ELanguage::Italian,  "Obiettivi" }
        }
    },
    {
        // Notes: used for the header in the achievements menu.
        "Achievements_Name_Uppercase",
        {
            { ELanguage::English,  "ACHIEVEMENTS" },
            { ELanguage::Japanese, "実績" },
            { ELanguage::German,   "ERFOLGE" },
            { ELanguage::French,   "SUCCÈS" },
            { ELanguage::Spanish,  "LOGROS" },
            { ELanguage::Italian,  "OBIETTIVI" }
        }
    },
    {
        "Achievements_Unlock",
        {
            { ELanguage::English,  "Achievement Unlocked!" },
            { ELanguage::Japanese, "実績のロックが解除されました" },
            { ELanguage::German,   "Erfolg Freigeschaltet!" },
            { ELanguage::French,   "Succès déverrouillé !" },
            { ELanguage::Spanish,  "¡Logro desbloqueado!" },
            { ELanguage::Italian,  "Obiettivo sbloccato!" }
        }
    },
    {
        "Installer_Header_Installer",
        {
            { ELanguage::English,  "INSTALLER" },
            { ELanguage::Japanese, "INSTALL" },
            { ELanguage::German,   "INSTALLATION" },
            { ELanguage::French,   "INSTALLATEUR" },
            { ELanguage::Spanish,  "INSTALADOR" },
            { ELanguage::Italian,  "INSTALLATORE" },
        },
    },
    {
        "Installer_Header_Installing",
        {
            { ELanguage::English,  "INSTALLING" },
            { ELanguage::Japanese, "INSTALL" },
            { ELanguage::German,   "INSTALLATION" },
            { ELanguage::French,   "INSTALLATION" },
            { ELanguage::Spanish,  "INSTALANDO" },
            { ELanguage::Italian,  "INSTALLANDO" },
        }
    },
    {
        "Installer_Page_SelectLanguage",
        {
            { ELanguage::English,  "Please select a language." },
            { ELanguage::Japanese, "言語を選択してください" },
            { ELanguage::German,   "Bitte eine Sprache auswählen." },
            { ELanguage::French,   "Choisissez une langue." },
            { ELanguage::Spanish,  "Selecciona un idioma." },
            { ELanguage::Italian,  "Seleziona una lingua." }
        }
    },
    {
        "Installer_Page_Introduction",
        {
            { ELanguage::English,  "Welcome to Marathon Recompiled!\n\nYou'll need an Xbox 360 copy of\nSONIC THE HEDGEHOG in order to proceed with the installation." },
            { ELanguage::Japanese, "Marathon Recompiledへようこそ！\nインストールにはXbox 360版の\n「ソニック・ザ・ヘッジホッグ」\nが必要です" },
            { ELanguage::German,   "Willkommen zu Marathon Recompiled!\n\nEs wird eine Xbox 360 Kopie von\nSONIC THE HEDGEHOG benötigt um mit der Installation fortfahren zu können." },
            { ELanguage::French,   "Bienvenue sur Marathon Recompiled !\n\nVous aurez besoin d'une copie de\nSONIC THE HEDGEHOG pour\nXbox 360 pour procéder à l'installation." },
            { ELanguage::Spanish,  "¡Bienvenido a Marathon Recompiled!\n\nNecesitas una copia de\nSONIC THE HEDGEHOG de\nXbox 360 para continuar con la instalación." },
            { ELanguage::Italian,  "Benvenuto a Marathon Recompiled!\n\nDovrai avere una copia di\nSONIC THE HEDGEHOG per la\nXbox 360 per proseguire con l'installazione." }
        }
    },
    {
        "Installer_Page_SelectGame",
        {
            { ELanguage::English,  "Add the sources for the game." },
            { ELanguage::Japanese, "ゲームのソースを追加" },
            { ELanguage::German,   "Füge die Quellen für das Spiel." },
            { ELanguage::French,   "Ajouter les fichiers du jeu." },
            { ELanguage::Spanish,  "Añade las fuentes para el juego." },
            { ELanguage::Italian,  "Aggiungi le fonti per il gioco." }
        }
    },
    {
        "Installer_Page_SelectDLC",
        {
            { ELanguage::English,  "Add the sources for the DLC." },
            { ELanguage::Japanese, "DLCのソースを追加" },
            { ELanguage::German,   "Füge die Quellen für die Erweiterungen des Spiels hinzu." },
            { ELanguage::French,   "Ajouter les fichiers pour les DLCs." },
            { ELanguage::Spanish,  "Añade las fuentes para el DLC." },
            { ELanguage::Italian,  "Aggiungi le fonti per i DLC." }
        }
    },
    {
        "Installer_Page_CheckSpace",
        {
            { ELanguage::English,  "The content will be installed to the program's folder.\n\n" },
            { ELanguage::Japanese, "コンテンツはプログラムのフォルダに\nインストールされます\n" },
            { ELanguage::German,   "Der Inhalt wird in dem Ordner des Programms installiert.\n" },
            { ELanguage::French,   "Le contenu sera installé dans le même dossier que le programme.\n" },
            { ELanguage::Spanish,  "El contenido será instalado a la carpeta del programa.\n\n" },
            { ELanguage::Italian,  "Il contenuto verrà installato nella cartella di questo programma.\n\n" }
        }
    },
    {
        "Installer_Page_Installing",
        {
            { ELanguage::English,  "Please wait while the content is being installed..." },
            { ELanguage::Japanese, "コンテンツのインストール中はお待ち\nください" },
            { ELanguage::German,   "Bitte warten. Der Inhalt wird installiert..." },
            { ELanguage::French,   "Veuillez patienter pendant l'installation du contenu..." },
            { ELanguage::Spanish,  "Por favor, espera mientras el contenido se instala... " },
            { ELanguage::Italian,  "Attendi mentre il contenuto viene installato... " }
        }
    },
    {
        "Installer_Page_InstallSucceeded",
        {
            { ELanguage::English,  "Installation complete!\nThis project is brought to you by:" },
            { ELanguage::Japanese, "インストール完了！\nプロジェクト制作：" },
            { ELanguage::German,   "Installation abgeschlossen!\nDieses Projekt wird präsentiert von:" },
            { ELanguage::French,   "Installation terminée !\nCe projet vous est présenté\npar :" },
            { ELanguage::Spanish,  "¡Instalación completada!\nEste proyecto ha sido posible gracias a:" },
            { ELanguage::Italian,  "Installazione completata!\nQuesto progetto è stato creato da:" }
        }
    },
    {
        "Installer_Page_InstallFailed",
        {
            { ELanguage::English,  "Installation failed.\n\nError: " },
            { ELanguage::Japanese, "インストールに失敗しました\n\nエラー： " },
            { ELanguage::German,   "Installation fehlgeschlagen.\n\nFehler: " },
            { ELanguage::French,   "L'installation a échoué.\n\nErreur : " },
            { ELanguage::Spanish,  "La instalación falló.\n\nError: " },
            { ELanguage::Italian,  "Installazione fallita.\n\nErrore: " }
        }
    },
    {
        "Installer_Step_Game",
        {
            { ELanguage::English,  "GAME" },
            { ELanguage::Japanese, "ゲーム" },
            { ELanguage::German,   "SPIEL" },
            { ELanguage::French,   "JEU" },
            { ELanguage::Spanish,  "JUEGO" },
            { ELanguage::Italian,  "GIOCO" }
        }
    },
    {
        "Installer_Step_RequiredSpace",
        {
            { ELanguage::English,  "Required space: %2.2f GiB" },
            { ELanguage::Japanese, "必要な容量: %2.2f GiB" },
            { ELanguage::German,   "Benötigter Speicherplatz:\n%2.2f GiB" },
            { ELanguage::French,   "Espace nécessaire :\n%2.2f Gio" },
            { ELanguage::Spanish,  "Espacio necesario: %2.2f GiB" },
            { ELanguage::Italian,  "Spazio necessario: %2.2f GiB" }
        }
    },
    {
        "Installer_Step_AvailableSpace",
        {
            { ELanguage::English,  "Available space: %2.2f GiB" },
            { ELanguage::Japanese, "使用可能な容量: %2.2f GiB" },
            { ELanguage::German,   "Verfügbarer Speicherplatz:\n%2.2f GiB" },
            { ELanguage::French,   "Espace disponible :\n%2.2f Gio" },
            { ELanguage::Spanish,  "Espacio disponible: %2.2f GiB" },
            { ELanguage::Italian,  "Spazio disponibile: %2.2f GiB" }
        }
    },
    {
        "Installer_Button_Next",
        {
            { ELanguage::English,  "NEXT" },
            { ELanguage::Japanese, "次へ" },
            { ELanguage::German,   "WEITER" },
            { ELanguage::French,   "SUIVANT" },
            { ELanguage::Spanish,  "SIGUIENTE" },
            { ELanguage::Italian,  "CONTINUA" }
        }
    },
    {
        "Installer_Button_Skip",
        {
            { ELanguage::English,  "SKIP" },
            { ELanguage::Japanese, "スキップ" },
            { ELanguage::German,   "ÜBERSPRINGEN" },
            { ELanguage::French,   "IGNORER" },
            { ELanguage::Spanish,  "SALTAR" },
            { ELanguage::Italian,  "SALTA" }
        }
    },
    {
        "Installer_Button_Retry",
        {
            { ELanguage::English,  "RETRY" },
            { ELanguage::Japanese, "リトライ" },
            { ELanguage::German,   "ERNEUT VERSUCHEN" },
            { ELanguage::French,   "RÉESSAYER" },
            { ELanguage::Spanish,  "REINTENTAR" },
            { ELanguage::Italian,  "RIPROVA" }
        }
    },
    {
        "Installer_Button_AddFiles",
        {
            { ELanguage::English,  "ADD FILES" },
            { ELanguage::Japanese, "ファイルを追加" },
            { ELanguage::German,   "DATEIEN HINZUFÜGEN" },
            { ELanguage::French,   "AJOUTER DES FICHIERS" },
            { ELanguage::Spanish,  "AÑADIR ARCHIVOS" },
            { ELanguage::Italian,  "AGGIUNGI FILE" }
        }
    },
    {
        "Installer_Button_AddFolder",
        {
            { ELanguage::English,  "ADD FOLDER" },
            { ELanguage::Japanese, "フォルダを追加" },
            { ELanguage::German,   "ORDNER HINZUFÜGEN" },
            { ELanguage::French,   "AJOUTER UN DOSSIER" },
            { ELanguage::Spanish,  "AÑADIR CARPETA" },
            { ELanguage::Italian,  "AGGIUNGI CARTELLA" }
        }
    },
    {
        // Notes: message appears when using the "Add Files" option and choosing any file that is not an Xbox 360 game dump.
        "Installer_Message_InvalidFilesList",
        {
            { ELanguage::English,  "The following selected files are invalid:" },
            { ELanguage::Japanese, "選択した次のファイルは無効です：" },
            { ELanguage::German,   "Die folgenden Dateien sind ungültig:" },
            { ELanguage::French,   "Les fichiers suivants ne sont pas valides :" },
            { ELanguage::Spanish,  "Los siguientes archivos no son válidos:" },
            { ELanguage::Italian,  "I seguenti file non sono validi:" }
        }
    },
    {
        // Notes: message appears in the event there are some invalid files after adding the DLC and moving onto the next step.
        "Installer_Message_InvalidFiles",
        {
            { ELanguage::English,  "Some of the files that have\nbeen provided are not valid.\n\nPlease make sure all the\nspecified files are correct\nand try again." },
            { ELanguage::Japanese, "提供されたファイルの一部が有効ではありません\n指定されたファイルがすべて正しいことを確認して\nもう一度お試しください" },
            { ELanguage::German,   "Einige Dateien, die bereitgestellt\nwurden sind ungültig.\n\nBitte stelle sicher, dass\ndie angegebenen Dateien korrekt\nsind und versuche es erneut." },
            { ELanguage::French,   "Certains fichiers fournis ne\nsont pas valides.\n\nVeuillez vous assurer que tous\nles fichiers spécifiés sont\ncorrects et réessayez." },
            { ELanguage::Spanish,  "Algunos de los archivos\nseleccionados no son válidos.\n\nPor favor, asegúrate de que\ntodos los archivos son correctos\ne inténtalo de nuevo.\n" },
            { ELanguage::Italian,  "Alcuni dei file che sono stati\nselezionati non sono validi.\n\nAssicurati che tutti\ni file sono quelli corretti\ne riprova." }
        }
    },
    {
        // Notes: message appears when clicking the "Add Files" option for the first time.
        "Installer_Message_FilePickerTutorial",
        {
            { ELanguage::English,  "Select a digital dump with\ncontent from the game.\n\nThese files can be obtained from\nyour Xbox 360 hard drive by\nfollowing the instructions on\nthe GitHub page.\n\nFor choosing a folder with extracted\nand unmodified game files, use\nthe \"Add Folder\" option instead." },
            { ELanguage::Japanese, "ゲームのコンテンツを含む デジタルダンプを選択してください\n\nこれらのファイルは GitHubページの指示に従って\nXbox 360ハードドライブから取得できます\n\n抽出された変更されていない\nゲームファイルを含むフォルダーを選択するには\n代わりに「フォルダの追加」オプションを使用してください" },
            { ELanguage::German,   "Wähle einen digitalen Dump von dem Spiel.\n\nDie Dateien können über die Festplatte deiner\nXbox 360 erlangt werden.\nFolge hierfür den Anweisungen auf der GitHub Seite.\n\nUm einen Ordner mit unmodifizierten Spieldateien auszuwählen, benutze die \"Ordner Hinzufügen\" Option stattdessen." },
            { ELanguage::French,   "Sélectionnez une copie\ndématérialisée avec le contenu du\njeu de base.\n\nCes fichiers peuvent être obtenus\nà partir du disque dur de votre\nXbox 360 en suivant les\ninstructions de la page GitHub.\n\nPour choisir un dossier contenant\nles fichiers de jeu extraits et\nnon modifiés, utilisez plutôt\nl'option \"Ajouter un dossier\"." },
            { ELanguage::Spanish,  "Selecciona una copia digital\ncon contenido del juego.\n\nPuedes obtener los archivos\nde tu disco duro de Xbox 360\nsiguiendo las instrucciones de\nla página de GitHub.\n\nPara elegir una carpeta con\narchivos extraídos sin modificar,\nutiliza la opción \"Añadir Carpeta\"." },
            { ELanguage::Italian,  "Seleziona una copia digitale\ncon i contenuti del gioco.\n\nQuesti file possono essere ottenuti\ndall'hard drive della tua Xbox 360\nseguendo le istruzioni\nsulla pagina GitHub.\n\nPer selezionare una cartella\ncon file estratti e non modificati\nusa l'opzione \"Aggiungi cartella\"." }
        }
    },
    {
        // Notes: message appears when clicking the "Add Folder" option for the first time.
        "Installer_Message_FolderPickerTutorial",
        {
            { ELanguage::English,  "Select a folder that contains the\nunmodified files that have been\nextracted from the game.\n\nThese files can be obtained from\nyour Xbox 360 hard drive by\nfollowing the instructions on\nthe GitHub page.\n\nFor choosing a digital dump,\nuse the \"Add Files\" option instead." },
            { ELanguage::Japanese, "ゲームから抽出された変更されていない\nファイルを含むフォルダを選択してください\n\nこれらのファイルは GitHubページの指示に従って\nXbox 360ハードドライブから取得できます\n\nデジタルダンプを選択するには\n代わりに「ファイルの追加」オプションを使用してください" },
            { ELanguage::German,   "Wähle einen Ordner, der unmodifizierte Dateien, die vom Spiel extrahiert wurden enthält.\n\nDie Dateien können über die Festplatte deiner\nXbox 360 erlangt werden.\nFolge hierfür den Anweisungen auf der GitHub Seite.\n\nUm einen digitalen Dump auszuwählen, benutze die \"Datei Hinzufügen\" Option stattdessen." },
            { ELanguage::French,   "Sélectionnez un dossier contenant\nles fichiers extraits du jeu de\nbase.\n\nCes fichiers peuvent être obtenus\nà partir du disque dur de votre\nXbox 360 en suivant les\ninstructions de la page GitHub.\n\nPour choisir une copie\ndématérialisée, utilisez plutôt\nl'option \"Ajouter des fichiers\"." },
            { ELanguage::Spanish,  "Selecciona una carpeta que\ncontenga los archivos sin\nmodificar extraídos del juego.\n\nPuedes obtener los archivos\nde tu disco duro de Xbox 360\nsiguiendo las instrucciones de\nla página de GitHub.\n\nPara elegir una copia digital,\nutiliza la opción \"Añadir Archivos\"." },
            { ELanguage::Italian,  "Seleziona una cartella che contiene\ni file non modificati che sono stati\nestratti dal gioco.\n\nQuesti file possono essere ottenuti\ndall'hard drive della tua Xbox 360\nseguendo le istruzioni\nsulla pagina GitHub.\n\nPer selezionare una copia digitale\nusa l'opzione \"Aggiungi file\"." }
        }
    },
    {
        // Notes: message appears when choosing the Install option at the title screen when the user is missing DLC content.
        "Installer_Message_TitleMissingDLC",
        {
            { ELanguage::English,  "This will restart the game to\nallow you to install any DLC\nthat you are missing.\n\nWould you like to install missing\ncontent?" },
            { ELanguage::Japanese, "これによりゲームが再起動し不足しているDLCを\nインストールできるようになります\n\n不足しているコンテンツを\nインストールしますか？" },
            { ELanguage::German,   "Das Spiel wird neu gestartet\num die Installation einer fehlenden\nErweiterung zu ermöglichen.\n\nMöchtest du den fehlenden\nInhalt installieren?" },
            { ELanguage::French,   "Cela redémarrera le jeu pour vous\npermettre d'installer les DLC\nmanquants.\n\nSouhaitez-vous installer le\ncontenu manquant ?" },
            { ELanguage::Spanish,  "Esta opción reiniciará el juego\npara permitirte instalar los DLC\nque falten.\n\n¿Quieres instalar el contenido\nque falta?" },
            { ELanguage::Italian,  "Questa opzione riavviera il gioco\nper farti installare qualsiasi DLC\nche non hai installato.\n\nVuoi installare i DLC mancanti?" }
        }
    },
    {
        // Notes: message appears when choosing the Install option at the title screen when the user is not missing any content.
        "Installer_Message_Title",
        {
            { ELanguage::English,  "This restarts the game to\nallow you to install any DLC\nthat you may be missing.\n\nYou are not currently\nmissing any DLC.\n\nWould you like to proceed\nanyway?" },
            { ELanguage::Japanese, "これによりゲームが再起動され\n不足しているDLCを\nインストールできるようになります\n\n現在 不足しているDLCはありません\n\nそれでも続行しますか？" },
            { ELanguage::German,   "Das Spiel wird neu gestartet\num die Installation einer fehlenden\nErweiterung zu ermöglichen.\n\nEs kann keine weitere Erweiterung\ninstalliert werden.\n\nMöchtest du trotzdem fortfahren?" },
            { ELanguage::French,   "Cela redémarrera le jeu pour vous\npermettre d'installer les DLC\nmanquants.\n\nIl ne vous manque aucun DLC.\n\nVoulez-vous quand même continuer ?" },
            { ELanguage::Spanish,  "Esto reiniciará el juego\npara permitirte instalar\nlos DLC que falten.\n\nActualmente, no falta ningún\nDLC por instalarse.\n\n¿Quieres continuar de todos\nmodos?" },
            { ELanguage::Italian,  "Questa opzione riavviera il gioco\nper farti installare qualsiasi DLC\nche non hai installato.\n\nHai già installato tutti i DLC.\n\nVuoi procedere comunque?" }
        }
    },
    {
        // Notes: message appears when user chooses "Quit" on the first available installation screen.
        "Installer_Message_Quit",
        {
            { ELanguage::English,  "Exit the installer.\nOK?" },
            { ELanguage::Japanese, "DUMMY" },
            { ELanguage::German,   "Die Installation verlassen.\nOK?" },
            { ELanguage::French,   "DUMMY" },
            { ELanguage::Spanish,  "¿Salir del instalador?" },
            { ELanguage::Italian,  "Esci dall'installatore.\nOK?" }
        }
    },
    {
        // Notes: message appears when user chooses "Cancel" during installation.
        "Installer_Message_Cancel",
        {
            { ELanguage::English,  "Cancel the installation.\nOK?" },
            { ELanguage::Japanese, "DUMMY" },
            { ELanguage::German,   "Die Installation abbrechen.\nOK?" },
            { ELanguage::French,   "DUMMY" },
            { ELanguage::Spanish,  "¿Cancelar la instalación?" },
            { ELanguage::Italian,  "Annulla l'installazione.\nOK?" }
        }
    },
    {
        // Notes: message appears when pressing B at the title screen.
        "Title_Message_Quit",
        {
            { ELanguage::English,  "Exit the game.\nOK?" },
            { ELanguage::Japanese, "DUMMY" },
            { ELanguage::German,   "Das Spiel verlassen.\nOK?" },
            { ELanguage::French,   "DUMMY" },
            { ELanguage::Spanish,  "¿Salir del juego?" },
            { ELanguage::Italian,  "Esci dal gioco.\nOK?" }
        }
    },
    {
        // Notes: message appears when ACH-DATA is corrupted (mismatching file size, bad signature, incorrect version or invalid checksum) upon pressing start at the title screen.
        // To make this occur, open the file in any editor and just remove a large chunk of data.
        "Title_Message_AchievementDataCorrupt",
        {
            { ELanguage::English,  "The achievement data appears to be\ncorrupted and cannot be loaded.\n\nProceeding from this point will\nclear your achievement data." },
            { ELanguage::Japanese, "実績データが破損しているため\n読み込むことができません\n\nこの先に進むと実績データが\n消去されます" },
            { ELanguage::German,   "Die Erfolgsdaten sind möglicherweise\nfehlerhaft und können nicht\ngeladen werden.\n\nDurch das Fortfahren werden\ndeine bisherigen Erfolgsdaten gelöscht." },
            { ELanguage::French,   "Les données des succès semblent être\nendommagées et ne peuvent être\nchargées.\n\nSi vous continuez, vos données\nseront écrasées." },
            { ELanguage::Spanish,  "Los datos de logros parecen estar\ncorruptos y no pueden cargarse.\n\nContinuar a partir de este punto\neliminará los datos de logros." },
            { ELanguage::Italian,  "I file degli obiettivi sembrano danneggiati\ne non possono essere caricati.\n\nSe prosegui da questo punto\ni tuoi obiettivi verranno cancellati." }
        }
    },
    {
        // Notes: message appears when ACH-DATA cannot be loaded upon pressing start at the title screen.
        // To make this occur, lock the ACH-DATA file using an external program so that it cannot be accessed by the game.
        "Title_Message_AchievementDataIOError",
        {
            { ELanguage::English,  "The achievement data could not be loaded.\nYour achievements will not be saved." },
            { ELanguage::Japanese, "実績データを読み込めませんでした\n実績は保存されません" },
            { ELanguage::German,   "Die Erfolgsdaten konnten nicht geladen werden.\nDeine Erfolge werden nicht gespeichert." },
            { ELanguage::French,   "Les données des succès ne\npeuvent être chargées.\nVos succès ne seront pas\nsauvegardés." },
            { ELanguage::Spanish,  "Los datos de logros no pueden cargarse.\nTus logros no serán guardados." },
            { ELanguage::Italian,  "I file degli obiettivi non possono essere caricati.\nI tuoi obiettivi non verranno salvati." }
        }
    },
    {
        "Title_Message_UpdateAvailable",
        {
            { ELanguage::English,  "An update is available!\n\nWould you like to visit the\nreleases page to download it?" },
            { ELanguage::Japanese, "アップデートが利用可能です\n\nリリースページにアクセスして\nダウンロードしますか？" },
            { ELanguage::German,   "Ein Update ist verfügbar!\n\nMöchtest du die Release-Seite\nbesuchen um es herunterzuladen?" },
            { ELanguage::French,   "Une mise à jour est disponible !\n\nVoulez-vous visiter la page\ndes mises à jour pour la\ntélécharger ?" },
            { ELanguage::Spanish,  "¡Hay una actualización disponible!\n\n¿Quieres ir a la página\npara descargarla?" },
            { ELanguage::Italian,  "È disponibile un aggiornamento!\n\nVuoi visitare la pagina releases\nper scaricarlo?" }
        }
    },
    {
        "Video_BackendError",
        {
            { ELanguage::English,  "Unable to create a D3D12 (Windows) or Vulkan backend.\n\nPlease make sure that:\n\n- Your system meets the minimum requirements.\n- Your GPU drivers are up to date.\n- Your operating system is on the latest version available." },
            { ELanguage::Japanese, "D3D12 (Windows)または\nVulkanバックエンドを作成できません\n\n次の点を確認してください：\n\n※システムが最小要件を満たしている\n※GPUドライバーが最新である\n※オペレーティングシステムが最新バージョンである" },
            { ELanguage::German,   "Es ist nicht möglich, ein D3D12 (Windows) oder Vulkan-Backend zu erstellen.\n\nBitte stelle sicher, dass:\n\n- Dein System die Mindestanforderungen erfüllt.\n- Deine GPU-Treiber auf dem neuesten Stand sind.\n- Dein Betriebssystem auf der neuesten verfügbaren Version ist." },
            { ELanguage::French,   "Impossible de créer un backend D3D12 (Windows) ou Vulkan.\n\nVeuillez vous assurer que :\n\n- Votre système répond aux critères minimums requis.\n- Les pilotes de votre processeur graphique sont à jour.\n- Votre système d'exploitation est à jour." },
            { ELanguage::Spanish,  "No se puede crear un entorno de D3D12 (Windows) o de Vulkan.\n\nPor favor, asegúrate de que:\n\n- Tu equipo cumple con los requisitos mínimos.\n- Los drivers de tu tarjeta gráfica están actualizados.\n- Tu sistema operativo está actualizado a la última versión.\n" },
            { ELanguage::Italian,  "Impossibile creare un backend D3D12 (Windows) o Vulkan.\n\nAssicurati che:\n\n- Il tuo sistema soddisfi i requisiti minimi.\n- I driver della scheda grafica siano aggiornati.\n- Il tuo sistema operativo sia aggiornato." }
        }
    },
    {
        "System_Win32_MissingDLLs",
        {
            { ELanguage::English,  "The module \"%s\" could not be found.\n\nPlease make sure that:\n\n- You extracted this copy of Marathon Recompiled fully and not just the *.exe file.\n- You are not running Marathon Recompiled from a *.zip file." },
            { ELanguage::Japanese, "モジュール\"%s\"が見つかりませんでした\n\n次の点を確認してください：\n\n※Marathon Recompiledの*.exeファイルだけを抽出していなく、 コピーを完全に抽出してること\n※Marathon Recompiledを*.zipファイルから実行していないこと" },
            { ELanguage::German,   "Das Modul \"%s\" konnte nicht gefunden werden.\n\nBitte stelle sicher, dass:\n\n- Diese Kopie von Marathon Recompiled vollständig entpackt wurde und nicht nur die *.exe-Datei.\n- Marathon Recompiled nicht direkt aus einer *.zip-Datei ausgeführt wird." },
            { ELanguage::French,   "Le module \"%s\" n'a pas pu être trouvé.\n\nVeuillez vous assurer que :\n\n- Vous avez extrait Marathon Recompiled dans son entièreté et pas seulement le fichier *.exe.\n- Vous n'exécutez pas Marathon Recompiled à partir d'un fichier *.zip." },
            { ELanguage::Spanish,  "No se pudo encontrar el módulo \"%s\".\n\nAsegúrese de que:\n\n- Ha extraido esta copia de Marathon Recompiled por completo y no solo el archivo *.exe.\n- No está ejecutando Marathon Recompiled desde un archivo *.zip." },
            { ELanguage::Italian,  "Impossibile trovare il modulo \"%s\".\n\nAssicurati che:\n\n- Hai estratto questa copia di Marathon Recompiled correttamente e non solo il file *.exe.\n- Non stai eseguendo Marathon Recompiled da un file *.zip." }
        }
    },
    {
        "System_MemoryAllocationFailed",
        {
            { ELanguage::English,  "Failed to allocate game memory.\n\nPlease make sure that:\n\n- You meet the minimum system requirements (8 GB).\n- Your page file is configured with at least 4-8 GB of virtual memory." },
            { ELanguage::Japanese, "ゲームメモリの割り当てに失敗しました\n\n次の点を確認してください：\n\n※最小システム要件（8 GB）を満たしていること\n※ページファイルに少なくとも4～8 GBの仮想メモリが設定されていること" },
            { ELanguage::German,   "Fehler beim Zuweisen des Spielspeichers.\n\nBitte stelle sicher, dass:\n\n- Die Mindestanforderungen für das System erfüllt sind (8 GB).\n- Die Auslagerungsdatei mit mindestens 4-8 GB virtuellem Speicher konfiguriert ist." },
            { ELanguage::French,   "Échec d'allocation de la mémoire du jeu.\n\nVeuillez vous assurer que :\n\n- Vous disposez de la configuration minimale requise (8 Go).\n- Votre fichier d’échange est configuré avec au moins 4 à 8 Go de mémoire virtuelle." },
            { ELanguage::Spanish,  "Fallo al asignar memoria del juego.\n\nPor favor, asegúrate de que:\n\n- Cumples los requisitos mínimos del sistema (8 GB).\n- Tu archivo de páginación está configurado con al menos 4 u 8 GB de memoria virtual." },
            { ELanguage::Italian,  "Impossibile allocare la memoria per il gioco.\n\nAssicurati che:\n\n- Soddisfi i requisiti minimi di sistema (8 GB).\n- Il tuo file di paging sia configurato con almeno 4 o 8 GB di memoria virtuale." }
        }
    },
    {
        "IntegrityCheck_Success",
        {
            { ELanguage::English,  "Installation check has finished.\n\nAll files seem to be correct.\n\nThe game will now close. Remove the launch argument to play the game." },
            { ELanguage::Japanese, "インストールチェックが終了しました\n\nすべてのファイルは正しいようです\n\nゲームは終了します、ゲームをプレイするには起動引数を削除してください" },
            { ELanguage::German,  "Die Installation wurde überprüft.\n\nAlle Dateien scheinen korrekt zu sein.\n\nDas Spiel wird nun geschlossen. Entferne die Startoption, um das Spiel zu spielen." },
            { ELanguage::French,  "La vérification de l'installation est terminée.\n\nTous les fichiers semblent corrects.\n\nL'application va maintenant se fermer. Retirez l'argument de lancement pour pouvoir lancer le jeu." },
            { ELanguage::Spanish,  "La verificación de la instalación ha terminado.\n\nTodos los archivos parecen correctos.\n\nEl juego se cerrará ahora. Elimina el argumento de lanzamiento para jugar al juego." },
            { ELanguage::Italian,  "La verifica dei file d'installazione è terminata.\n\nTutti i file sembrano corretti.\n\nIl gioco si chiuderà. Rimuovi l'argomento di avvio per poter giocare." }
        }
    },
    {
        "IntegrityCheck_Failed",
        {
            { ELanguage::English,  "Installation check has failed.\n\nError: %s\n\nThe game will now close. Try reinstalling the game by using the --install launch argument." },
            { ELanguage::Japanese, "インストールチェックに失敗しました\n\nエラー：%s\n\nゲームは終了します、--install 起動引数を使用してゲームを再インストールしてください" },
            { ELanguage::German,  "Die Installationsprüfung ist fehlgeschlagen.\n\nFehler: %s\n\nDas Spiel wird nun geschlossen. Versuche das Spiel durch Verwendung der Startoption --install neu zu installieren." },
            { ELanguage::French,  "La vérification de l'installation a échoué.\n\nErreur : %s\n\nL'application va maintenant se fermer. Essayez de réinstaller le jeu en utilisant l'argument de lancement --install." },
            { ELanguage::Spanish,  "La verificación de la instalación ha fallado.\n\nError: %s\n\nEl juego se cerrará ahora. Intenta reinstalar el juego utilizando el argumento de lanzamiento --install." },
            { ELanguage::Italian,  "La verifica dei file d'installazione non è andata a buon fine.\n\nErrore: %s\n\nIl gioco si chiuderà. Prova a reinstallare il gioco utilizzando l'argomento di avvio --install." }
        }
    },
    {
        "Common_OK",
        {
            { ELanguage::English,  "OK" },
            { ELanguage::Japanese, "OK" },
            { ELanguage::German,   "OK" },
            { ELanguage::French,   "OK" },
            { ELanguage::Spanish,  "OK" },
            { ELanguage::Italian,  "OK" }
        }
    },
    {
        "Common_On",
        {
            { ELanguage::English,  "On" },
            { ELanguage::Japanese, "オン" },
            { ELanguage::German,   "An" },
            { ELanguage::French,   "Oui" },
            { ELanguage::Spanish,  "Act." },
            { ELanguage::Italian,  "Sì" }
        }
    },
    {
        "Common_Off",
        {
            { ELanguage::English,  "Off" },
            { ELanguage::Japanese, "オフ" },
            { ELanguage::German,   "Aus" },
            { ELanguage::French,   "Non" },
            { ELanguage::Spanish,  "Desact." },
            { ELanguage::Italian,  "No" }
        }
    },
    {
        "Common_Yes",
        {
            { ELanguage::English,  "Yes" },
            { ELanguage::Japanese, "はい" },
            { ELanguage::German,   "Ja" },
            { ELanguage::French,   "Oui" },
            { ELanguage::Spanish,  "Sí" },
            { ELanguage::Italian,  "Sì" }
        }
    },
    {
        "Common_No",
        {
            { ELanguage::English,  "No" },
            { ELanguage::Japanese, "いいえ" },
            { ELanguage::German,   "Nein" },
            { ELanguage::French,   "Non" },
            { ELanguage::Spanish,  "No" },
            { ELanguage::Italian,  "No" }
        }
    },
    {
        "Common_Next",
        {
            { ELanguage::English,  "Next" },
            { ELanguage::Japanese, "次へ" },
            { ELanguage::German,   "Weiter" },
            { ELanguage::French,   "Suivant" },
            { ELanguage::Spanish,  "Siguiente" },
            { ELanguage::Italian,  "Avanti" }
        }
    },
    {
        "Common_Select",
        {
            { ELanguage::English,  "Select" },
            { ELanguage::Japanese, "決定" },
            { ELanguage::German,   "Auswählen" },
            { ELanguage::French,   "Sélectionner" },
            { ELanguage::Spanish,  "Seleccionar" },
            { ELanguage::Italian,  "Seleziona" }
        }
    },
    {
        "Common_Back",
        {
            { ELanguage::English,  "Back" },
            { ELanguage::Japanese, "戻る" },
            { ELanguage::German,   "Zurück" },
            { ELanguage::French,   "Retour" },
            { ELanguage::Spanish,  "Atrás" },
            { ELanguage::Italian,  "Indietro" }
        }
    },
    {
        "Common_Quit",
        {
            { ELanguage::English,  "Quit" },
            { ELanguage::Japanese, "やめる" },
            { ELanguage::German,   "Abbrechen" },
            { ELanguage::French,   "Annuler" },
            { ELanguage::Spanish,  "Cancelar" },
            { ELanguage::Italian,  "Annulla" }
        }
    },
    {
        "Common_Cancel",
        {
            { ELanguage::English,  "Cancel" },
            { ELanguage::Japanese, "キャンセル" },
            { ELanguage::German,   "Abbrechen" },
            { ELanguage::French,   "Annuler" },
            { ELanguage::Spanish,  "Cancelar" },
            { ELanguage::Italian,  "Annulla" }
        }
    },
    {
        "Common_Reset",
        {
            { ELanguage::English,  "Reset" },
            { ELanguage::Japanese, "リセット" },
            { ELanguage::German,   "Zurücksetzen" },
            { ELanguage::French,   "Par défaut" },
            { ELanguage::Spanish,  "Restablecer" },
            { ELanguage::Italian,  "Ripristina" }
        }
    },
    {
        "Common_Switch",
        {
            { ELanguage::English,  "Switch" },
            { ELanguage::Japanese, "きりかえ" },
            { ELanguage::German,   "Wechseln" },
            { ELanguage::French,   "Changer" },
            { ELanguage::Spanish,  "Cambiar" },
            { ELanguage::Italian,  "Cambia" }
        }
    },
    {
        "ButtonGuide_Cancel",
        {
            { ELanguage::English, "${picture(button_b)}${locale(Common_Cancel)}" },
            { ELanguage::Spanish, "${picture(button_b)} ${locale(Common_Cancel)}" }
        }
    },
    {
        "ButtonGuide_Back",
        {
            { ELanguage::English, "${picture(button_b)}${locale(Common_Back)}" },
            { ELanguage::Spanish, "${picture(button_b)} ${locale(Common_Back)}" }
        }
    },
    {
        "ButtonGuide_Select",
        {
            { ELanguage::English, "${picture(button_a)}${locale(Common_Select)}" },
            { ELanguage::Spanish, "${picture(button_a)} ${locale(Common_Select)}" },
        }
    },
    {
        "ButtonGuide_SelectQuit",
        {
            { ELanguage::English, "${picture(button_a)}${locale(Common_Select)}  ${picture(button_b)}${locale(Common_Quit)}" },
            { ELanguage::German,  "${picture(button_a)}${locale(Common_Select)}　　${picture(button_b)}${locale(Common_Quit)}" },
            { ELanguage::Spanish, "${picture(button_a)} ${locale(Common_Select)}  ${picture(button_b)} ${locale(Common_Quit)}" },
        }
    },
    {
        "ButtonGuide_SelectBack",
        {
            { ELanguage::English, "${picture(button_a)}${locale(Common_Select)}  ${picture(button_b)}${locale(Common_Back)}" },
            { ELanguage::German,  "${picture(button_a)}${locale(Common_Select)}　　${picture(button_b)}${locale(Common_Back)}" },
            { ELanguage::Spanish, "${picture(button_a)} ${locale(Common_Select)}  ${picture(button_b)} ${locale(Common_Back)}" },
        }
    },
    {
        "ButtonGuide_ResetSelectBack",
        {
            { ELanguage::English, "${picture(button_x)}${locale(Common_Reset)}  ${picture(button_a)}${locale(Common_Select)}  ${picture(button_b)}${locale(Common_Back)}" },
            { ELanguage::German,  "${picture(button_x)}${locale(Common_Reset)}　　${picture(button_a)}${locale(Common_Select)}　　${picture(button_b)}${locale(Common_Back)}" },
            { ELanguage::Spanish, "${picture(button_x)} ${locale(Common_Reset)}  ${picture(button_a)} ${locale(Common_Select)}  ${picture(button_b)} ${locale(Common_Back)}" },
        }
    },
};

std::string& Localise(const std::string_view& key)
{
    auto localeFindResult = g_locale.find(key);
    if (localeFindResult != g_locale.end())
    {
        auto languageFindResult = localeFindResult->second.find(Config::Language);
        if (languageFindResult == localeFindResult->second.end())
            languageFindResult = localeFindResult->second.find(ELanguage::English);

        if (languageFindResult != localeFindResult->second.end())
            return languageFindResult->second;
    }

    return g_localeMissing;
}
