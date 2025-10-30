#pragma once

#include <xxHashMap.h>

class TextPatches
{
public:
    static inline xxHashMap<const char*> s_redirectedMessages
    {
        { HashStr("msg_deviceselect"), "msg_retry" },           // Replace "Select storage device." text with "Retry" for alert windows.
        { HashStr("msg_gamequitconfirm4"), "msg_backtotitle1" } // Replace "Exit the game." text with "Go back to the Title Screen."
    };

    static inline std::vector<const char*> s_hintPatterns =
    {
        "hint_all01_a0*",
        "hint_all01_e06_*",
        "hint_all03_a00_*",
        "hint_all03_h3*",
        "hint_all04_*",
        "hint_all05_a0*",
        "hint_all05_a1*",
        "hint_all05_a2*",
        "hint_all05_a30_*",
        "hint_all05_a31_*",
        "hint_all05_a32_*",
        "hint_all06_*",
        "hint_kdv01_a00_*",
        "hint_kdv01_a02_sv", // Silver: Man, the staircase is damaged. Now I'll have to use my power.
        "hint_kdv01_a04_rg", // Rouge: You can use this vehicle to get across the lake.
        "hint_kdv01_a05_sv", // Silver: I can manage this cage with my power.
        "hint_kdv01_a06_*",
        "hint_kdv01_a07_sv", // Silver: That container might help me move forward.
        "hint_kdv01_a08_sv", // Silver: I might be able to levitate to that spring.
        "hint_kdv01_h*",
        "hint_wvo01_a*",
        "hint_tpj01_a01_*",
        "hint_tpj01_a03_pr", // Elise: Over there!
        "hint_tpj01_a04_*",
        "hint_tpj01_a05_*",
        "hint_tpj01_a06_*",
        "hint_tpj01_a07_sv", // Silver: This hanging ruin might be useful.
        "hint_tpj01_a08_sv", // Silver: Looks like I can use this to go up.
        "hint_tpj01_a09_sv", // Silver: If I can hit that ball with this...
        "hint_tpj01_a10_sv", // Silver: Is this for breaking walls?
        "hint_tpj01_a11_*",
        "hint_tpj01_a12_*",
        "hint_tpj01_a13_*",
        "hint_tpj01_e03_pr", // Elise: Sonic! To your left!
        "hint_tpj01_e05_pr", // Elise: Sonic! To your right!
        "hint_tpj01_e07_rg", // Rouge: It looks like you need to take them down.
        "hint_tpj01_e09_pr", // Elise: Sonic! Above you!
        "hint_tpj01_e10_sn", // Sonic: There's a route here.
        "hint_tpj01_e13_pr", // Elise: Sonic! Look down!
        "hint_tpj01_e18_sn", // Sonic: Oh? Is there a back road?
        "hint_tpj01_e19_sn", // Sonic: Thank you, Elise!
        "hint_tpj01_h*",
        "hint_dtd01_a00_*",
        "hint_dtd01_a02_sd", // Shadow: It looks like these pillars will lead me to him.
        "hint_dtd01_a03_*",
        "hint_dtd01_a04_sv", // Silver: I might be able to make a path leading up if I use my power over there.
        "hint_dtd01_a05_sd", // Shadow: These big statues look pretty fragile.
        "hint_dtd01_h*",
        "hint_wap01_a*",
        "hint_wap01_h*",
        "hint_csc01_a03_sn", // Sonic: This lamp post... Maybe I can swing off of it?
        "hint_csc01_a06_sv", // Silver: Blaze! Watch out for things flying out of the tornado.
        "hint_csc01_a07_sv", // Silver: This steel bar has a green mark on it. If I use my Psychokinesis here...
        "hint_csc01_a08_sv", // Silver: That road doesn't look too solid.
        "hint_csc01_a09_sv", // Silver: I can wipe them out if I hit that pipe right.
        "hint_csc01_a10_bz", // Blaze: Use your Psychokinesis! You should have no problem with the enemy's bullets!
        "hint_csc01_a11_bz", // Blaze: It looks like you can take advantage of this concrete ramp.
        "hint_flc01_a06_sv", // Silver: I'll have to get closer and use my power directly!
        "hint_flc01_h*",
        "hint_rct01_a03_sd", // Shadow: I'm going to have to destroy the train so it doesn't get away!
        "hint_rct01_a04_sv", // Silver: There's no other way ahead. I guess I've got to break down the door.
        "hint_rct01_a05_sv", // Silver: If I put weights on this scale, I'll be able to move forward.
        "hint_rct01_a06_*",
        "hint_rct01_a07_*",
        "hint_rct01_a08_sn", // Sonic: I'll be able to move faster if I go on top of the train instead of being on the rails.
        "hint_rct01_a09_sv", // Silver: I'll blow away these containers with my power!
        "hint_rct01_a10_*",
        "hint_rct01_a12_sd", // Shadow: I'm going to have to use Homing Missiles so he doesn't get away.
        "hint_rct01_h*",
        "hint_aqa01_a00_*",
        "hint_aqa01_a01_tl", // Tails: If you use the power of this ball, you can get over there.
        "hint_aqa01_a02_*",
        "hint_aqa01_a03_tl", // Tails: If you attack the switch, the magnet will activate.
        "hint_aqa01_a04_tl", // Tails: There are some magnets that repel if you push them twice...
        "hint_aqa01_a05_tl", // Tails: Look! There's the magnet switch. Push it, and the path should open!
        "hint_aqa01_a07_sn", // Sonic: If I slide, I can squeeze through.
        "hint_aqa00_e*",
        "hint_end01_e03_*",
        "hint_end01_h*",
        "hint_bos01_a*",
        "hint_bos02_a*",
        "hint_bos03_a*",
        "hint_bos04_a*",
        "hint_bos05_a*",
        "hint_bos06_a*",
        "hint_bos07_a*",
        "hint_bos08_a*",
        "hint_bos09_a*",
        "hint_bos10_a*",
        "hint_bos11_e*",
        "hint_bos11_a*",
        "hint_twn01_a*",
        "hint_twn01_e0?_tl",
        "hint_twn01_e05_kn", // Knuckles: Heh, there's a secret room here! Let's hurry to the White Acropolis!
        "hint_twn01_e0?_pr",
        "hint_twn01_e18_kn", // Knuckles: Ring three bells at once... Sonic, you should be able to do it!
        "hint_twn01_e30_bz", // Blaze: You need to short out the electricity in order to stop this laser. We'll need to hit the switches at the same time.
        "hint_twn01_e43_rg", // Rouge: The bridge is broken! Shadow, can't you do something?
        "hint_twn01_h*",
        "hint_tpj01_e20_pr"  // Elise: If you jump when the bud is glowing, you may be able to jump higher.
    };
};
