This is a modified version just made for fun, by 2 noobs (t0mizwf26 & yakit4k0),<br>
based on the original AnalogsEnhancer v1.1 (Commit 0629c4b) by Rinnegatamante.<br>

-- Compare with the original AnalogsEnhancer v1.1<br>

(1) Added outer-deadzone function for both re-scaling and non re-scaling deadzone mode.<br>
(2) When using outer-deadzone with re-scaling, just like stick would stay centre/neutral within (inner-)deadzone, stick will become MAX when outer-deadzone is reached, and 360 degrees analog movement is still possible.<br>
(3) When using outer-deadzone with NO re-scaling, when outer-deadzone is reached, stick becomes 8-way digital.<br>
(When outer-deadzone is reached, x-axis or y-axis output will become MAX when actual x-axis or y-axis is also beyond "centre+/-49")<br>

(4) Default config is now "left=0,127,n;right=0,127,n;n", so everything is disabled. No inner or outer deadzone, no re-scaling, no ANALOG_WIDE mode.<br>
(5) Moved config file from ux0:/data/AnalogsEnhancer/config.txt to ur0:/tai/AnalogsEnhancerKai.txt (since it's now in ur0, edit with caution!)<br>

(6) Bug fixed, now when using re-scaling, x / y can properly reach MAX right / down, which is 255, instead of just 254.<br>
(7) Change CMakeLists.txt, using correct name "SceIofilemgrForDriver_stub" instead of "sceIofilemgrForDriver_stub", so the code can be compiled without error when using Linux (case-sensitive).<br>

-- HOW TO USE:<br>

(1) Place AnalogsEnhancerKai.skprx in ur0:/tai/<br>
(2) Install the plugin under *KERNEL in taiHen config file<br>
*KERNEL<br>
ux0:tai/AnalogsEnhancerKai.skprx<br>
(3) Place AnalogsEnhancerKai.txt file in ur0:/tai/<br>
(4) .txt config file contains config like:<br>
left=0,127,n;right=0,127,n;n<br>
(5) Above { 0 | 127 | n | 0 | 127 | n | n } in { left=0,127,n;right=0,127,n;n } means<br>
{<br>
| "left inner-deadzone (0-127)" | "left outer-deadzone (0-127)" | "left rescaling (y/n)" |<br>
| "right inner-deadzone (0-127)" | "right outer-deadzone (0-127)" | "right rescaling (y/n)" |<br>
| "use ANALOG_WIDE (y/n)" |<br>
}<br>

-- NOTE:<br>

(0) 0 means stick centre/neutral point, 127 means stick fully tilt (MAX circle)<br>
(1) inner-deadzone = 0 >> inner-deadzone disabled<br>
(2) outer-deadzone = 0 or 127 >> outer-deadzone disabled<br>
(3) inner and outer-deadzone will not overlap, if overlapped, it will fix itself, (outer = inner + 1)<br>
In other words, it will always like:<br>
{ MAX<--OuterDZ-->UsableRange<--InnerDZ-->Centre<--InnerDZ-->UsableRange<--OuterDZ-->MAX }<br>
(4) For normal use to just fix small drift, "left={YourDeadzone},127,y;right={YourDeadzone},127,y;n" is recommended.<br>
Personally I use "left=32,127,y;right=32,127,y;n" to fix my Vita Slim.<br>
Or just use the original AnalogsEnhancer, much simpler than this Kai mess.<br>

-- THANKS:<br>
Rinnegatamante, for the original deadzone plugin with re-scaling, Tokyo no longer drift tonight.<br>
yakit4k0 (u/Yakitako-Kun), for compiling & helping to implement outer-deadzone into both rescaleAnalogs() and deadzoneAnalogs().<br>
u/lizin5ths, for the idea of outer deadzone.<br>
HENkaku community on Discord, for helping yakit4k0 to fix errors during compiling.
