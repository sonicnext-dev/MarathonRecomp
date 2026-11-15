# Dumping

> [!NOTE]
> The following guide is for dumping the game on a PC using OmniDrive firmware with a compatible disc drive.
>
> This method can only be used to dump the game off of a disc. Digital copies and DLC can only be dumped from an Xbox 360.
> 
> If you wish to dump the game from your Xbox 360 hard drive, use [this guide](DUMPING-en.md) instead.

### Pre-requisites
- A disc drive compatible with [OmniDrive](https://github.com/RibShark/OmniDrive)
- [7-Zip](https://7-zip.org/download.html) (for extracting Redumper)
- [Redumper](https://github.com/superg/redumper)

### Instructions

1. Plug your disc drive into your PC.
2. Download [the latest release of Redumper](https://github.com/superg/redumper/releases/latest) for your OS and open the `*.zip` file using [7-Zip](https://7-zip.org/download.html), then extract its contents anywhere that's convenient to you.
3. Open Redumper. If you have not already done so, you may use the "DRIVE FLASHER" option to flash Omnidrive to your disc drive. Please see [here](https://wiki.redump.info/OmniDrive) for more information about compatible drives and how to flash them.

> [!CAUTION]
> This operation will flash the firmware of your disc drive. THIS HAS THE RISK OF BRICKING YOUR DISC DRIVE IF YOU ARE NOT CAREFUL. Please make sure to follow the directions carefully, and do not interrupt the process.

4. Insert your Sonic the Hedgehog disc.
5. Select the drive in Redumper, and select "BROWSE" to choose where to output the dump. Enter a name for the dump into the "NAME" field.
6. Click the "DUMP" button. Redumper will begin to dump your game to an ISO file.
7. Once the process is complete, you should have all the necessary files for installation. You may now remove the game disc and disconnect your disc drive. After that, [return to the readme and proceed to the next step](/README.md#how-to-install).