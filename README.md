## AnalogsEnhancerKai (アナログエンハンサー改) 

> This is a modified version of [Rinnegatamante](https://github.com/Rinnegatamante)'s [AnalogsEnhancer](https://github.com/Rinnegatamante/AnalogsEnhancer) v1.1
> 
> Made by [t0mizwf26](https://github.com/t0mizwf26) & [yakit4k0](https://github.com/yakit4k0) (u/Yakitako-Kun)
> 
> Based on the original [AnalogsEnhancer](https://github.com/Rinnegatamante/AnalogsEnhancer) v1.1 code (commit [0629c4b](https://github.com/Rinnegatamante/AnalogsEnhancer/tree/0629c4b8ca124bc08cf6a6dc3e759a0eb8bbc45a)) by [Rinnegatamante](https://github.com/Rinnegatamante)
> 
> With the original "outer-deadzone" idea from u/lizin5ths (fork [Haasman0 / AnalogsEnhancer](https://github.com/Haasman0/AnalogsEnhancer))

### New Features
Compare with the original AnalogsEnhancer v1.1
* Added outer-deadzone function for both re-scaling and non re-scaling deadzone mode.
* When using outer-deadzone with re-scaling, just like stick would stay centre/neutral within (inner-)deadzone,<br>
stick will become MAX when outer-deadzone is reached, and 360 degrees analog movement is still possible.
* When using outer-deadzone with NO re-scaling, when outer-deadzone is reached, stick becomes 8-way digital.<br>
(When outer-deadzone is reached, x-axis or y-axis output will become MAX when actual x-axis or y-axis is also beyond "centre+/-49")

### Changes
* Default config when .txt config file missing is now "**left=0,127,n;right=0,127,n;n**", so everything is disabled.<br>
Which means: no inner or outer deadzone, no re-scaling, no ANALOG_WIDE mode.
* Moved config file from **ux0:data/AnalogsEnhancer/config.txt** to **ur0:tai/AnalogsEnhancerKai.txt** (since it's now in ur0, edit with caution!)

### Bug Fixes
* When re-scaling = ON, x / y can now properly reach MAX=255 (without some other fixes which skip 254), instead of just 254 no 255.
* Change CMakeLists.txt, using correct name "SceIofilemgrForDriver_stub" instead of "sceIofilemgrForDriver_stub", so the code can be compiled without error when using Linux (case-sensitive).

### How to use
* Put both AnalogsEnhancerKai.skprx and AnalogsEnhancerKai.txt in ur0:tai/
* The AnalogsEnhancerKai.txt config included is "**left=32,127,y;right=32,127,y;n**", means 32 inner-DZ with re-scaling=ON for both L&R.
* Edit **ur0:tai/config.txt**, disable other existing AnalogsEnhancer plugins, then add following:
```
# AnalogsEnhancerKai
*KERNEL
ur0:tai/AnalogsEnhancerKai.skprx
```
* Reboot PS Vita
* N-nani? (Hopefully) NO MOA Kansei Dorifto?! Omae (Analog Stick Drift) wa Mou Shindeiru.
* Edit the config file to fix even worse drift or explore other gimmicks.

### More detail about config, deadzone, and 8-way
* When editing the .txt config file<br>
-1. It is recommended to use number between 0 - 127<br>
-2. '0' means stick centre/neutral point, '127' means stick full tilt (gate)<br>
( 0->127 == centre->gate, or in Homebrew App VitaTester's term: 127 towards 0 OR 255 )<br>
-3. If inner-deadzone = 0, then inner-deadzone is OFF<br>
-4. If outer-deadzone = 0 or 127, then outer-deadzone is OFF<br>
-5. inner-DZ and outer-DZ will not overlap.<br>
If overlapped in config (e.g. inner-DZ=70, outer-DZ=40), it will fix itself by making (outer-DZ = inner-DZ + 1)

* About outer-DZ in re-scaling=OFF mode
I wish the following extremely poorly made & inaccurate picture would help to explain:<br>
Assume both stick using the same setting, then this pic is representing config: "**left=25,100,n;right=25,100,n;n**"<br>
Which means **inner-DZ=25, outer-DZ=100, re-scaling=OFF**<br>
_**(The image shown are for illustration purposes only and not an exact representation of actual situation)**_<br>
_(※画像はイメージです。(笑) image is image DESU)_<br>
![DeadZone(25-100-n)](https://github.com/user-attachments/assets/8e5c732f-b37e-419e-a673-d8a29a70073e)<br>
_(Errors in above pic: green area magnitude should be **25 to 99**, botom left SW direction should be "**0;255**")_<br>
Using this example, <br>
a) Want change green analog area to yellow/grey 8-way? "**left=24,25,n;right=24,25,n;n**"<br>
b) Want change 8-way area into analog? "**left=25,127,n;right=25,127,n;n**"<br>
c) Want no analog & 8-way fires at magnitude 50 instead of 100? "**left=49,50,n;right=49,50,n;n**"

### Thanks
* Rinnegatamante, for the original plugin
* yakit4k0, for compiling & helping to implement outer-deadzone into both rescaleAnalogs() and deadzoneAnalogs()
* u/lizin5ths, for the idea of outer deadzone
* HENkaku community, for helping to fix errors during compiling
