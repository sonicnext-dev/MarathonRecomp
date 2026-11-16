# Dumping

> [!NOTE]
> The following guide is for dumping the game using a USB storage device.
>
> If you wish to dump the game from your Xbox 360 hard drive, use [this guide](DUMPING-en.md) instead.

### Pre-requisites
- Xbox 360 (modifications not necessary)
- USB Storage Device (16 GB minimum)
- Sonic the Hedgehog (2006) for Xbox 360
    - Retail Disc or Digital Copy.
    - All available DLC are optional.
- [7-Zip](https://7-zip.org/download.html) (for extracting Velocity)
- [Velocity](https://github.com/Gualdimar/Velocity/releases/download/xex%2Biso-branch/Velocity-XEXISO.rar) (Gualdimar's fork)

> [!NOTE]
> Some Xbox 360 S|E consoles come with 4 GB of internal flash storage. These are **not compatible** with this method since you cannot set a USB storage device as a system drive on them.
>
> An exception is for versions with the Trinity motherboard, where the storage module is located on a separate daughterboard and can be disconnected (not recommended for inexperienced users). On other models, this module is soldered directly onto the motherboard and cannot be disconnected.

### Instructions

1. Turn off your Xbox 360 and remove the hard drive from your console.

> [!TIP]
> You may consult the following guides if you're unsure on how to do this:
> - [Xbox 360](https://www.ifixit.com/Guide/Xbox+360+Hard+Drive+Replacement/3326)
> - [Xbox 360 S](https://www.ifixit.com/Guide/Xbox+360+S+Hard+Drive+Replacement/3184)
> - [Xbox 360 E](https://www.ifixit.com/Guide/Xbox+360+E+Hard+Drive+Replacement/22179)

2. Connect a USB storage device to the console and turn it on.
3. At the Xbox Dashboard, navigate to the **settings** tab, select **System**, then **Storage**.
4. On the **Storage Devices** screen, select your USB storage device and press Y to open **Device Options**.
5. Select **Make System Drive**, then **Configure Now** and confirm by selecting **Yes**.

> [!CAUTION]
> This operation will erase all content on the USB storage device. Before continuing, make sure that you have backed up its contents. **This action is irreversible!**

6. After successfully configuring the USB storage device as a system drive, turn off your console and reconnect the hard drive.
7. Turn on your console, then navigate to the **settings** tab, select **System**, then **Storage**.
8. Select the original hard drive, then navigate to the **Profiles** directory and move an Xbox Live profile that owns the game and its DLC to the USB storage device.
9. Turn off the console and remove the hard drive.
10. Turn on the console, **install the system update if prompted**, then sign in to your Xbox Live profile.

> [!NOTE]
> If you have a digital copy of Sonic the Hedgehog, skip to step 14.

11. Insert your retail disc copy of Sonic the Hedgehog into the Xbox 360 disc tray.
12. At the Xbox Dashboard, go over to the disc tile under the **home** tab and press X to view **Game Details**.
13. Under the **overview** tab, select the **Install** tile and choose to install to the USB drive.
14. For downloadable content, return to the Xbox Dashboard and navigate to the **settings** tab, select **Account**, then **Download History**. Look for the game (if you own it digitally) and its DLC. Select them and choose **Download Again**. Repeat this step for all of the DLC that you own.

> [!TIP]
> If the game or its DLC do not appear in **Download History**, ensure that you have moved the account that owns them and are currently signed into it.

15. Turn off the console and connect the USB storage device to your PC.
16. Download [the latest release of Velocity](https://github.com/Gualdimar/Velocity/releases/download/xex%2Biso-branch/Velocity-XEXISO.rar) and open the `*.rar` file using [7-Zip](https://7-zip.org/download.html), then extract its contents anywhere that's convenient to you.
17. Create a new folder anywhere that's convenient to you for storing the game files.

> [!NOTE]
> If you're using Linux, skip to step 19.

18. Right-click `Velocity.exe` and click **Properties**, then under the **Compatibility** tab, tick **Run this program as an administrator** and click **OK**. This is required in order for the program to recognize the storage device.
19. Launch `Velocity.exe`. You should see a **Device Detected** message appear on launch asking if you would like to open the **Device Content Viewer**. Click **Yes**.
20. You should now see a tree view of your storage device's contents. Expand the tree nodes for `/Shared Items/Games/` (and optionally `/Shared Items/DLC/`, if you have the DLC installed).
21. Hold the CTRL key and click on **SONIC THE HEDGEHOG** under the `Games` node, as well as the **DLC** under the `DLC` node, if you have the DLC installed. Ensure all are selected before the next step.
22. Right-click any of the selected items and click **Copy Selected to Local Disk**, then navigate to the folder you created in step 18 and select it. Velocity will now begin copying the game files to your PC.
23. Once the transfer is complete, you should have all the necessary files for installation. You may now reconnect your hard drive to your console and restore your Xbox Live profile from the USB storage device. After that, [return to the readme and proceed to the next step](/README.md#how-to-install).