This is a stripped down boring version of AnalogsEnhancerKai, hence Kai-Vanilla.<br>

-- Compare with AnalogsEnhancerKai,<br>

(1) new functions (like outer deadzone) related code is removed.<br>

-- Compare with the original AnalogsEnhancer v1.1 by Rinnegatamante (Commit 0629c4b)<br>

(1) Moved config file from ux0:/data/AnalogsEnhancer/config.txt to ur0:/tai/AnalogsEnhancerKaiV.txt (since it's now in ur0, edit with caution!)<br>
(2) Default config is now "left=0,n;right=0,n;n", so everything is disabled. No deadzone, no rescaling, no ANALOG_WIDE mode.<br>
(3) Bugfixed, now when using rescaling, x / y can properly reach MAX right / down, which is 255, instead of just 254.<br>
(4) Change CMakeLists.txt, using correct name "SceIofilemgrForDriver_stub" instead of "sceIofilemgrForDriver_stub", so the code can be compiled without error when using Linux (case-sensitive).<br>

That's it.<br>

-- HOW TO USE:<br>

(1) Place AnalogsEnhancerKaiV.skprx in ur0:/tai/<br>
(2) Install the plugin under *KERNEL in taiHen config file<br>
*KERNEL<br>
ux0:tai/AnalogsEnhancerKaiV.skprx<br>
(3) Place AnalogsEnhancerKaiV.txt file in ur0:/tai/<br>
(4) .txt config file contains config like:<br>
left=0,n;right=0,n;n<br>
(5) Above { 0 | n | 0 | n | n } in { left=0,n;right=0,n;n } means<br>
{ "left deadzone (0-127)" | "left rescaling (y/n)" | "right deadzone (0-127)" | "right rescaling (y/n)" | "use ANALOG_WIDE (y/n)" }
