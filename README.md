## AnalogsEnhancerKai (アナログエンハンサー改) - Vanilla

> This is stripped down boring version of AnalogsEnhancerKai, hence Kai-Vanilla.
> 
> Made by [t0mizwf26](https://github.com/t0mizwf26) & [yakit4k0](https://github.com/yakit4k0) (u/Yakitako-Kun)
> 
> Based on the original [AnalogsEnhancer](https://github.com/Rinnegatamante/AnalogsEnhancer) v1.1 code (commit [0629c4b](https://github.com/Rinnegatamante/AnalogsEnhancer/tree/0629c4b8ca124bc08cf6a6dc3e759a0eb8bbc45a)) by [Rinnegatamante](https://github.com/Rinnegatamante)

### Compare with the AnalogsEnhancerKai
* Any outer-deadzone related code is removed.
* Bug fixes and config file location change is kept.

### Compare with the AnalogsEnhancer v1.1
* Default config when .txt config file missing is now "**left=0,n;right=0,n;n**", so everything is disabled.<br>
Which means: no deadzone, no re-scaling, no ANALOG_WIDE mode.
* Moved config file from **ux0:data/AnalogsEnhancer/config.txt** to **ur0:tai/AnalogsEnhancerKaiV.txt** (since it's now in ur0, edit with caution!)

### How to use
* Put both AnalogsEnhancerKaiV.skprx and AnalogsEnhancerKaiV.txt in ur0:tai/
* The AnalogsEnhancerKaiV.txt config included is "**left=32,y;right=32,y;n**", means 32 deadzone with re-scaling=ON for both L&R.
* Edit **ur0:tai/config.txt**, disable other existing AnalogsEnhancer plugins, then add following:
```
# AnalogsEnhancerKaiV
*KERNEL
ur0:tai/AnalogsEnhancerKaiV.skprx
```
* Reboot PS Vita
* But seriously, why not just use the original AnalogsEnhancer instead?
* This Kai-Vanilla is just the original AnalogsEnhancer with config file location moved to a much more dangerous place.

### More detail about config
* When editing the .txt config file<br>
-1. It is recommended to use number between 0 - 127<br>
-2. '0' means stick centre/neutral point, '127' means stick full tilt (gate)<br>

### Thanks
* Rinnegatamante, for the original plugin
* yakit4k0, for compiling & helping to test the code
* HENkaku community, for helping to fix errors during compiling
