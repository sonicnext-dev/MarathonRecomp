[33mcommit 4bdfb37745936c1925e9bffacf66cde9af48a48d[m[33m ([m[1;36mHEAD[m[33m -> [m[1;32mWindowsResize[m[33m, [m[1;31morigin/WindowsResize[m[33m)[m
Author: ReimousTH <reimousth@gmail.com>
Date:   Tue Oct 28 17:18:16 2025 +0300

    Graphics Api & Resize Base

[33mcommit 6198f0ea03de7928192bb2520775ea64bf1dddf8[m
Author: Hyper <34012267+hyperbx@users.noreply.github.com>
Date:   Thu Oct 30 18:51:04 2025 +0000

    Implement options menu (#183)
    
    * Initial options menu hooks and design
    
    Co-authored-by: squidbus <175574877+squidbus@users.noreply.github.com>
    
    * Implemented option controls
    
    * Update resources submodule
    
    * Remove app init checks from options menu text rendering
    
    * Load hidden category dynamically
    
    * Implemented cursor animation
    
    * Implemented restart prompt for necessary options
    
    * Implemented intro and outro animations for controls
    
    * Implemented button guide and reset button
    
    * Update resources submodule
    
    * Disable button guide animation at main menu
    
    * Keep game paused when options menu is opened
    
    * Implemented animations for selection arrows
    
    * Reopen pause menu pre-selecting the options button
    
    * Update FPS locale
    
    * Implement Effects Volume option
    
    * Fix slider gauges not fading out during outro animation
    
    * Clean-up
    
    * Implemented hold input for cursor and sliders
    
    * Don't redraw borders in options menu when accessed via the main menu
    
    * Unpause stage BGM upon entering options menu
    
    * Implemented Subtitles option
    
    * Implemented Hints and Control Tutorial options
    
    These may need further review in terms of blocking hints initiated by volumes that suggest things to the player and hints about bosses after death.
    
    * Fix camera options not affecting demo camera
    
    * Keep BGM paused until Master or Music Volume sliders are selected
    
    * Added hint patterns for disabling scripted hints
    
    * Clean-up API and player patches, added Antigravity and Light Dash remapping support
    
    * Disable unimplemented options temporarily
    
    * Disable stage title control tutorial
    
    * Revert "Disable stage title control tutorial"
    
    This reverts commit 649ef25fa9ff637c13b5f0f6004672d18bf7e2f2.
    
    * Implemented Mute on Focus Lost option
    
    * Don't play button guide intro animation if buttons are the same
    
    * Restored original transition animations between main menu and options menu
    
    * Implemented enum descriptions
    
    * Fix Window Size option not displaying the correct window size
    
    * Prevent inputs leaking from the options menu to the main menu
    
    * Improve chevron animation accuracy
    
    * Use linear interpolation for button guide animation
    
    * Fix aspect ratio scaling for custom UI
    
    * Fix version info not accounting for letterbox
    
    * UI accuracy pass
    
    * Implemented interpolated text renderer
    
    * Improve locale for remapping actions
    
    * Finalise options menu locale
    
    * German and Italian localisation
    
    Co-authored-by: Kitzuku <smashnicsg@gmail.com>
    Co-authored-by: NextinHKRY <38560522+NextinMono@users.noreply.github.com>
    
    * Spanish localisation by DaGuAr
    
    * Use correct linear interpolation strategy for all animations
    
    * Fix options menu descriptions fading when unnecessary
    
    * Improved restart prompt
    
    * Fixed main menu not drawing accurately at original aspect ratio
    
    * Fixed field offset in HUDMainMenu
    
    * Fade option names to avoid clipping into scroll arrows
    
    * French localisation
    
    Co-authored-by: LJSTAR <31629427+ljstarbird@users.noreply.github.com>
    
    * Fix button guide being too short at ludicrously wide aspect ratios
    
    * Fix chevron animation not looping properly
    
    * Hide control tutorial messages for remapped actions
    
    * Rename Antigravity option to Sliding Attack
    
    * Update locale for Light Dash and Sliding Attack
    
    Co-authored-by: LJSTAR <31629427+ljstarbird@users.noreply.github.com>
    Co-authored-by: DaGuAr
    Co-authored-by: NextinHKRY <38560522+NextinMono@users.noreply.github.com>
    
    * Fix options menu scaling at narrow aspect ratios
    
    * Fix "OPTIONS" title not being localised immediately upon changing language
    
    * Fix description text fading out when cancelling a restart
    
    * Fixed animation delay between options menu -> main menu transition
    
    * Fix seam in top metal plate at narrow aspect ratios
    
    * Don't animate installer title when changing language
    
    * Disable credits marquee in installer (will address in a later PR)
    
    * Fix red strip misaligning from top metal plate during intro and outro animations
    
    * Japanese localisation
    
    Co-authored-by: brianuuu <38166666+brianuuu@users.noreply.github.com>
    
    * Update README.md
    
    * Fix warnings
    
    * Update embedded resources
    
    * Fix alignment
    
    * Finalised German localisation
    
    Co-authored-by: Ray Vassos
    
    * Update embedded resources
    
    * Fix button guide displaying Reset button whilst options are locked on
    
    * Block options menu input during intro animation
    
    * Addressed review comments
    
    * Swap Xbox and PS3 voice lines when changing Controller Icons
    
    * Clean-up
    
    * Fix Linux build
    
    * Swap Xbox and PS3 button icons at loading screens
    
    This does not account for remapped buttons yet.
    
    * Fix Japanese main menu buttons not changing to PlayStation
    
    * ButtonGuide -> ButtonWindow
    
    For consistency sake.
    
    * Fix Tag text lingering at ultrawide in different menu states
    
    * Fix button window flickering when returning to main menu
    
    * Increase Rodin font size for Japanese
    
    * Fix seam between option title background edge and stretch
    
    ---------
    
    Co-authored-by: squidbus <175574877+squidbus@users.noreply.github.com>
    Co-authored-by: Kitzuku <smashnicsg@gmail.com>
    Co-authored-by: NextinHKRY <38560522+NextinMono@users.noreply.github.com>
    Co-authored-by: LJSTAR <31629427+ljstarbird@users.noreply.github.com>
    Co-authored-by: brianuuu <38166666+brianuuu@users.noreply.github.com>

[33mcommit 5da32318c65e9501f08c027a2136066eaa37a7c1[m[33m ([m[1;31msonicnext-dev/main[m[33m, [m[1;31msonicnext-dev/HEAD[m[33m, [m[1;31mnext-dev/main[m[33m, [m[1;31mnext-dev/HEAD[m[33m)[m
Author: Isaac Marovitz <42140194+IsaacMarovitz@users.noreply.github.com>
Date:   Sat Oct 25 17:55:24 2025 -0400

    Video Cleanup (#186)
    
    * Delete a bunch of Unleashed specific code
    
    Signed-off-by: Isaac Marovitz <isaacryu@icloud.com>
    
    * ScreenShaderInit
    
    Signed-off-by: Isaac Marovitz <isaacryu@icloud.com>
    
    ---------
    
    Signed-off-by: Isaac Marovitz <isaacryu@icloud.com>

[33mcommit b058d8067f8bdf89621f0fe7f8a2ac728a11a86f[m
Author: Isaac Marovitz <42140194+IsaacMarovitz@users.noreply.github.com>
Date:   Sun Oct 19 15:15:09 2025 -0400

    InfiniteLives (#176)
    
    Signed-off-by: Isaac Marovitz <isaacryu@icloud.com>

[33mcommit f8488ac4536c95c8f99858c54d5d0cb47f478cc5[m
Author: squidbus <175574877+squidbus@users.noreply.github.com>
Date:   Wed Oct 1 20:09:38 2025 -0700

    Remove old std::atomic_ref polyfill. (#180)

[33mcommit d8377861fbed7bdf4758ccc13ef3802993ca997a[m
Author: squidbus <175574877+squidbus@users.noreply.github.com>
Date:   Wed Oct 1 20:06:40 2025 -0700

    Update MoltenVK (#179)

[33mcommit 3cc06611efaa004ac0698cebfcbdee7f0b732ee8[m
Author: squidbus <175574877+squidbus@users.noreply.github.com>
Date:   Wed Oct 1 12:36:07 2025 -0700

    Fix conditional survey buffer flags. (#178)

[33mcommit 8604878a15b1c891cbbdd5264c9ad990f79d0728[m
Author: ReimousTH <51449880+ReimousTH@users.noreply.github.com>
Date:   Mon Sep 22 18:06:50 2025 +0300

    Code: Sonic Gauge Restoration (#155)
    
    * Gauge
    
    * Finish rebase 