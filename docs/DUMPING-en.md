# Dumping GTA IV for Xbox 360

This guide explains how to extract Grand Theft Auto IV game files from your Xbox 360 for use with Liberty Recompiled.

### Pre-requisites
- Xbox 360 (modifications not necessary)
- Xbox 360 Hard Drive (20 GB minimum)
- Xbox 360 Hard Drive Transfer Cable (or a compatible SATA to USB adapter)
- Grand Theft Auto IV for Xbox 360
    - Retail Disc (2 discs) or Digital Copy
    - Episodes from Liberty City DLC is optional
- [7-Zip](https://7-zip.org/download.html) (for extracting Velocity)
- [Velocity](https://github.com/Gualdimar/Velocity/releases/download/xex%2Biso-branch/Velocity-XEXISO.rar) (Gualdimar's fork)

> [!TIP]
> If you do not have the Xbox 360 Hard Drive Transfer Cable, please ensure that you purchase the correct revision of it for your console.
>
> The latest revision works with both original Xbox 360 and Xbox 360 S|E hard drives, but the first revision only works with original Xbox 360 hard drives.
>
> To know which is which, the first revision cable is gray, whereas the latest revision (which supports any Xbox 360 hard drive) is black.

### Instructions

> [!NOTE]
> If you have a digital copy of GTA IV, skip to step 4.

1. Insert Disc 1 of your retail copy of Grand Theft Auto IV into the Xbox 360 disc tray.
2. At the Xbox Dashboard, go over to the disc tile under the **home** tab and press X to view **Game Details**.
3. Under the **overview** tab, select the **Install** tile and choose to install to the primary hard drive. Repeat for Disc 2 if prompted.
4. Once installed, turn off your Xbox 360 and remove the hard drive from your console.

> [!TIP]
> You may consult the following guides if you're unsure on how to do this:
> - [Xbox 360](https://www.ifixit.com/Guide/Xbox+360+Hard+Drive+Replacement/3326)
> - [Xbox 360 S](https://www.ifixit.com/Guide/Xbox+360+S+Hard+Drive+Replacement/3184)
> - [Xbox 360 E](https://www.ifixit.com/Guide/Xbox+360+E+Hard+Drive+Replacement/22179)

5. Using the Xbox 360 Hard Drive Transfer Cable (or compatible SATA to USB adapter), connect your Xbox 360 hard drive to your PC.

> [!CAUTION]
> If you're using an unofficial SATA to USB adapter, you may need to remove the hard drive from its enclosure in order to connect it.
>
> For original Xbox 360 hard drives, this process is as simple as [removing some screws and cracking open the enclosure](https://www.ifixit.com/Guide/Xbox+360+HDD+Replacement/3430).
>
> For Xbox 360 S|E hard drives, this enclosure is glued shut and removing the hard drive may be an irreversible process!
>
> **It is highly recommended** that you obtain the official Xbox 360 Hard Drive Transfer Cable in order to proceed.

6. Download [the latest release of Velocity](https://github.com/Gualdimar/Velocity/releases/download/xex%2Biso-branch/Velocity-XEXISO.rar) and open the `*.rar` file using [7-Zip](https://7-zip.org/download.html), then extract its contents anywhere that's convenient to you.
7. Create a new folder anywhere that's convenient to you for storing the game files.

> [!NOTE]
> If you're using Linux, skip to step 9.

8. Right-click `Velocity.exe` and click **Properties**, then under the **Compatibility** tab, tick **Run this program as an administrator** and click **OK**. This is required in order for the program to recognize the hard drive. You can now launch `Velocity.exe`.
9. You should see a **Device Detected** message appear on launch asking if you would like to open the **Device Content Viewer**. Click **Yes**.
10. You should now see a tree view of your hard drive's contents. Expand the tree nodes for `/Shared Items/Games/`.
11. Hold the CTRL key and click on **Grand Theft Auto IV** under the `Games` node. If you have Episodes from Liberty City installed, select that as well.
12. Right-click any of the selected items and click **Copy Selected to Local Disk**, then navigate to the folder you created in step 7 and select it. Velocity will now begin copying the game files to your PC.
13. Once the transfer is complete, close the **Device Content Viewer** window.
14. You should now have all of the necessary files for installation. [Return to the readme and proceed to the next step](/README.md#how-to-install).

### Alternative: Using an ISO

If you have a GTA IV Xbox 360 ISO file, you can use it directly with Liberty Recompiled's installer wizard. The installer supports:
- `.iso` files (Xbox 360 disc images)
- Extracted game folders
- XContent packages

Simply point the installer to your ISO file or extracted game folder when prompted.
