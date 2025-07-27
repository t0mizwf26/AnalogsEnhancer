This is stripped down boring version of AnalogsEnhancerKai, hence Kai-Vanilla.<br>

-- Compare with AnalogsEnhancerKai,<br>

(1) New gimmicks (like outer-deadzone) related code are removed.<br>

-- Compare with the original AnalogsEnhancer v1.1 (Commit 0629c4b) by Rinnegatamante<br>

(1) Default config is now "left=0,n;right=0,n;n", so everything is disabled. No deadzone, no re-scaling, no ANALOG_WIDE mode.<br>
(2) Moved config file from ux0:/data/AnalogsEnhancer/config.txt to ur0:/tai/AnalogsEnhancerKaiV.txt (since it's now in ur0, edit with caution!)<br>

(3) Bug fixed, now when using re-scaling, x / y can properly reach MAX right / down, which is 255, instead of just 254.<br>
(4) Change CMakeLists.txt, using correct name "SceIofilemgrForDriver_stub" instead of "sceIofilemgrForDriver_stub", so the code can be compiled without error when using Linux (case-sensitive).<br>

-- HOW TO USE:<br>

(1) Place AnalogsEnhancerKaiV.skprx in ur0:/tai/<br>
(2) Install the plugin under *KERNEL in taiHen config file<br>
*KERNEL<br>
ur0:tai/AnalogsEnhancerKaiV.skprx<br>
(3) Place AnalogsEnhancerKai.txt file in ur0:/tai/<br>
(4) .txt config file contains config like:<br>
left=0,n;right=0,n;n<br>
(5) Above { 0 | n | 0 | n | n } in { left=0,n;right=0,n;n } means<br>
{<br>
| "left deadzone (0-127)" | "left rescaling (y/n)" |<br>
| "right deadzone (0-127)" | "right rescaling (y/n)" |<br>
| "use ANALOG_WIDE (y/n)"<br>
}<br>

-- NOTE:<br>

(0) 0 means stick centre/neutral point, 127 means stick fully tilt (MAX circle)<br>
(1) deadzone = 0 >> deadzone disabled<br>
(3) For normal use to just fix small drift, "left={YourDeadzone},y;right={YourDeadzone},y;n" is recommended.<br>
Personally I use "left=32,y;right=32,y;n" to fix my Vita Slim.<br>
Or why not just use the original AnalogsEnhancer?<br>
This Kai-Vanilla is just the original AnalogsEnhancer with config file location moved to a much more dangerous place.<br>

-- THANKS:<br>
Rinnegatamante, for the original deadzone plugin with re-scaling, Tokyo no longer drift tonight.<br>
yakit4k0 (u/Yakitako-Kun), for compiling & being boring doing this "Vanilla" non-sense :-) & (in the not "Vanilla" Kai) helping to implement outer-deadzone into both rescaleAnalogs() and deadzoneAnalogs().<br>
u/lizin5ths, for the idea of outer deadzone.<br>
HENkaku community on Discord, for helping yakit4k0 to fix errors during compiling.
