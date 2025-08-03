# [AnalogsEnhancerKai](https://github.com/t0mizwf26/AnalogsEnhancerKai/) for PS Vita

A fork of PS Vita kernel plugin [Rinnegatamante](https://github.com/Rinnegatamante)/[AnalogsEnhancer](https://github.com/Rinnegatamante/AnalogsEnhancer)  
By [t0mizwf26](https://github.com/t0mizwf26) & [yakit4k0](https://github.com/yakit4k0) (u/Yakitako-Kun)  

With new features like **Outer Dead Zone**, **Slow Mode**, etc.  

Based on the original AnalogsEnhancer v1.1 code (commit [0629c4b](https://github.com/Rinnegatamante/AnalogsEnhancer/tree/0629c4b8ca124bc08cf6a6dc3e759a0eb8bbc45a))  
With "Outer Dead Zone" idea from u/lizin5ths (fork [Haasman0 / AnalogsEnhancer](https://github.com/Haasman0/AnalogsEnhancer))  

## How to Use

1. Copy **AnalogsEnhancerKai.skprx** into **ur0:tai/**
2. Customise **AnalogsEnhancerKai's config.txt** config file
3. Copy **config.txt** into **ux0:data/AnalogsEnhancerKai**
> Alternatively, if you prefer to use **ur0:tai/** instead:  
>  
> Rename **config.txt** to **AnaEnKaiCfg.txt**  
> Copy **AnaEnKaiCfg.txt** directly into **ur0:tai/**  
>  
> **( !! Handle "ur0:tai/", or anything "ur0:" with Caution !! )**  
> ( If both **ur0:tai/AnaEnKaiCfg.txt** & **ux0:data/AnalogsEnhancerKai/config.txt** are present, will use **AnaEnKaiCfg.txt** )  
4. Edit **ur0:tai/config.txt**, add the following, and save
```
# AnalogsEnhancerKai
*KERNEL
ur0:tai/AnalogsEnhancerKai.skprx
```
5. Reboot PS Vita

## Features Explained

> **!! IMPORTANT NOTE: PLEASE READ !!**
>  
> All the **"Dead Zone"** and **"Slow Mode"** example graphs below were created for v1.1.0.  
> So you will find v1.2.0's new **"Diagonal Scaling (DS)"** config, or ``d=0``, missing.  
> However, **"DS"** config will not affect any of the examples. They are **NOT** outdated.  
> (Descriptions are updated)  
> Just remember, in real v1.2.0 config file, there's now ``d=0`` at the end of the config.  
> **"Diagonal Scaling"** is explained after **"5) ANALOG_WIDE"**.  
>  
> Thanks, please read on.  

### 1) Introduction

If .txt config file is missing, the following default config will be used instead.  
``l=0,127,n,s=0,0;r=0,127,n,s=0,0;n;d=0``  
Which means every feature offered by this plugin is turned **OFF**.  

![l=0,127,n,s=0,0;r=0,127,n,s=0,0;n;d=0](https://github.com/user-attachments/assets/8b9586f9-aa5f-4e8d-8bf8-9fd243acee22)  

### 2) Inner Dead Zone

Classic feature from the original Rinnegatamante/AnalogsEnhancer.  
With 2 different modes to choose from: **Rescaling On** & **Rescaling Off**.  

#### 2.1) Inner Dead Zone - Rescaling On (Scaled Radial Dead Zone)

``l=64,127,y,s=0,0;r=64,127,y,s=0,0;n;d=0``  
This is probably the most common Dead Zone people would use to fix drifting analogue sticks.  
The full range is available, no input is ignored (skipped), allowing low-magnitude adjustments.  

![l=64,127,y,s=0,0;r=64,127,y,s=0,0;n;d=0](https://github.com/user-attachments/assets/948025db-147b-44b8-a299-be4e982d5ee9)  

#### 2.2) Inner Dead Zone - Rescaling Off (Radial Dead Zone)

``l=16,127,n,s=0,0;r=16,127,n,s=0,0;n;d=0``  
Fewer people would choose to use this. Less ideal, some input is ignored (skipped).  

In this example, low-magnitude adjustments with magnitude < 16 are impossible.  
There's one advantage, though. Notice the gradient here is still **1**?  
Unlike the previous **Rescaling On** example with gradient **2**.  
This means it's a little easier to make precise adjustments for the remaining range (16 to Max).  

![l=16,127,n,s=0,0;r=16,127,n,s=0,0;n;d=0](https://github.com/user-attachments/assets/0cfbe53d-af94-4abb-be30-54aeea9d497f)  

### 3) Outer Dead Zone

Outer Dead Zone is useful with situations like  
> "since putting little joystick covers on them makes it extremely difficult to hit the maximum values."  
> u/lizin5ths  

Outer Dead Zone with **Rescaling On** could help to solve the problem by reaching the max earlier.  
The code is based on u/lizin5ths's implementation (fork [Haasman0 / AnalogsEnhancer](https://github.com/Haasman0/AnalogsEnhancer)), with many improvements.  

Outer Dead Zone with **Rescaling Off** however, is something a little different. More on that below.  

#### 3.1) Outer Dead Zone - Rescaling On (360-Degree Movement)

``l=0,80,y,s=0,0;r=0,80,y,s=0,0;n;d=0``  
In this example, the stick reaches the max much earlier.  
Bring more _cute cat paw claw grip cap cover_!!.  

![l=0,80,y,s=0,0;r=0,80,y,s=0,0;n;d=0](https://github.com/user-attachments/assets/134a7a74-48e5-4358-a695-c4c5db7b43ae)  

#### 3.2) Outer Dead Zone - Rescaling Off (8-Way, "Axial Dead Zone")

``l=0,80,n,s=0,0;r=0,80,n,s=0,0;n;d=0``  
The _little joystick covers_ problem is solved by using **Rescaling On**.  
Q: What should **Rescaling Off** do? A: **Go retro.**  
Now here's something niche.  

![l=0,80,n,s=0,0;r=0,80,n,s=0,0;n;d=0](https://github.com/user-attachments/assets/5120397e-85de-4eb6-a417-5c4ded417ec3)  

#### 3.3) Whoa, hang on a second!

Why should people use a normal 360-degree analogue stick with only the outer one-third as 8-Way?  
Right, but we could **use Inner & Outer Dead Zone together**.  

``l=80,80,n,s=0,0;r=80,80,n,s=0,0;n;d=0``  

Note the **8-Way always has an "Axial Dead Zone" of magnitude 49**.  
So by setting "Inner Dead Zone = Outer Dead Zone", we could have an **8-Way joystick with customisable actuation point (49 to 126)**  

![l=80,80,n,s=0,0;r=80,80,n,s=0,0;n;d=0](https://github.com/user-attachments/assets/c85f6bd1-60c4-43d3-bbcd-b180e1ffa961)  

``l=80,80,y,s=0,0;r=80,80,y,s=0,0;n;d=0``  

You probably guessed we could do the same with **Rescaling On**, 360-Degree Movement with Max output only.  
However, personally, I can't think of a reason to.  

![l=80,80,y,s=0,0;r=80,80,y,s=0,0;n;d=0](https://github.com/user-attachments/assets/b64aa177-72c8-4944-a29b-a2ba45fb4416)  

### 4) S for Sniper? Snail? Shooter? ...Well, it's "Slow Mode"

Very boring name I know. Just call it Sniper / Snail / Shooter / High-Precision Mode, or whatever you like.  
I'm just way too lazy to change all related names in the code, so I will keep calling it: "Slow Mode".  

Remember earlier example in **2.1) Inner Dead Zone - Rescaling On (Scaled Radial Dead Zone)**?  
``l=64,127,y,s=0,0;r=64,127,y,s=0,0;n;d=0``  
Inner 50% = Dead Zone, gradient is "2" instead of the normal "1". Everything's so fast, it's hard to make tiny adjustments!  
Let's try to fix this, but unfortunately with a price to pay.  

#### 4.1) Slow Mode - Rescaling On

``l=64,127,y,s=32,16;r=64,127,y,s=32,16;n;d=0``  
Change ``s=0,0`` to ``s=32,16``, here's the result:  

![l=64,127,y,s=32,16;r=64,127,y,s=32,16;n;d=0](https://github.com/user-attachments/assets/53e0a449-f811-4cd3-9749-222e0a274379)  

Now it's easier to sneak / snipe.  
However, the outer half is sacrificed. Gradient is increased, making the outer half even harder to do tiny adjustments.  

#### 4.2) Slow Mode - Rescaling Off

``l=16,127,n,s=32,16;r=16,127,n,s=32,16;n;d=0``  
Also change ``s=0,0`` to ``s=32,16``, see what happens here when **Rescaling Off**:  

![l=16,127,n,s=32,16;r=16,127,n,s=32,16;n;d=0](https://github.com/user-attachments/assets/942dbd2c-86f8-42fd-8389-0b63ff16f261)  

In this particular example, within Slow Mode range (32), the output magnitude will always be "16".  

#### 4.3) Have some fun

``l=16,80,n,s=32,8;r=16,80,n,s=32,8;n;d=0``  
Who's going to do this?  

![l=16,80,n,s=32,8;r=16,80,n,s=32,8;n;d=0](https://github.com/user-attachments/assets/391ce4da-e428-46b5-be93-5e1bf828e5ab)  

### 5) ANALOG_WIDE

That last **"n"**, before **"d=0"** in ``l=0,127,n,s=0,0;r=0,127,n,s=0,0;n;d=0``  

Also a classic feature from the original Rinnegatamante/AnalogsEnhancer.  

Please allow me to quote from [Rinnegatamante](https://github.com/Rinnegatamante)  
> "ANALOG_WIDE is one of the two modalities to poll analogs from Vita OS (one being ANALOG that simulates PSP polling and ANALOG_WIDE)."  
> "In ANALOG mode (used even in several commercial games as well as PSP and PSX titles), max range of analogs is reduced thus making analogs feel less reactive."  

It's a very nice feature but might ["causes issues in some games"](https://github.com/Rinnegatamante/AnalogsEnhancer/issues/2).  
So it's disabled by default in AnalogsEnhancerKai.  
If needed, please re-enable it manually (by changing that last **n** into **y**).  

### 6) Diagonal Scaling

The **"d=0"** from ``l=0,127,n,s=0,0;r=0,127,n,s=0,0;n;d=0``.  

In some software, the stick may have difficulty in reaching Max output in diagonal directions.  
This Diagonal Scaling feature might be useful when trying to fix that.  
(For example, there are people reporting issues with PSP games using **Adrenaline**.)  

#### 6.1) How to configure Diagonal Scaling

The **Minimum** is **0** (means Scale Factor 1.00x, Diagonal Scaling is Off).  
The **Maximum** is **42**.  
(Anything out of the min. ~ max. range will get **autocorrected to Minimum or Maximum**.)  

**Increasing** the value makes the stick reach its diagonal Max output earlier.  

#### 6.2) Note

> This feature **only works with "Rescaling On"**.  

> You might have to edit this value often.  
> For example, if you find **"d=42"** works best for **Adrenaline**, leaving it at 42 will cause other games to reach Max on diagonal directions too early.  
> Since those games might work best with **"d=0"**.  

> There's no plan to implement "per-game profile" for AnalogsEnhancerKai.  

> Using "**Outer Dead Zone - Rescaling On**" could achieve a very similar fix, but it will be harder to make tiny adjustments.  
> This "**Diagonal Scaling**" doesn't have that problem.  

#### 6.3) Some more details about Diagonal Scaling

The root cause of the problem is about "**stick circularity**".  
PC gamers might know that "[**Gamepad Tester - Test Circularity (spin joysticks slowly to test)**](https://hardwaretester.com/gamepad)".  

If trying to draw a circle using the stick's maximum tilt, software-wise speaking, it will not be a perfect circle.  
In most cases it's somewhere between a square and a circle, with rounded corners.  

An extreme example would be "**Outer Dead Zone - Rescaling Off (8-Way, "Axial Dead Zone")**", that's a square.  
Normally the stick moves within the circle and should not reach those 4 corners.  
(``0,0 ; 0,255 ; 255,255 ; 255,0`` for PS Vita)  

There will be problems if some games require values near those 4 corners in order to work properly.  
By using **Outer Dead Zone - Rescaling On** or **Diagonal Scaling**, we could bring those inaccessible corner areas into the circle.  

## Understand & Customise Config File

``l=0,127,n,s=0,0;r=0,127,n,s=0,0;n;d=0``  

Break it into 4 sections:  

| Section     | Content          |
| ----------- | ---------------- |
| Left Stick  | l=0,127,n,s=0,0; |
| Right Stick | r=0,127,n,s=0,0; |
| ----------- | ---------------- |
| ANALOG_WIDE | n;               |
| ----------- | ---------------- |
| DS          | d=0              |

Now, we could see **Left Stick** and **Right Stick** configs sharing the same structure:  
``l(/r)=0,127,n,s=0,0;``  

Each part means:  

| Config       | Meaning                               |
| ------------ | ------------------------------------- |
| l(/r)=0,127, | Inner Dead Zone, Outer Dead Zone      |
| n,           | Rescaling (y=ON ; n=OFF)              |
| s=0,0;       | Slow Mode Range, Slow Mode Max Output |
| ------------ | ------------------------------------- |
| n;           | **5) ANALOG_WIDE**, (y=ON ; n=OFF)    |
| ------------ | ------------------------------------- |
| d=0          | **6) Diagonal Scaling**               |

For each non-"y/n" config:  

| Config               | Legit Range if "ON"    | Special Cases (switch OFF, etc.)                              |
| -------------------- | ---------------------- | ------------------------------------------------------------- |
| Inner Dead Zone      | 1 ~ 126                | **0** : Inner DZ **OFF**                                      |
|                      |                        | **> 126** : **Stick Disabled** (Always Centre)                |
| -------------------- | ---------------------- | ------------------------------------------------------------- |
| Outer Dead Zone      | 1 ~ 126                | **0** or **127** : Outer DZ **OFF**                           |
|                      |                        | Never overlap, **Autocorrect if Outer DZ < Inner DZ**         |
| -------------------- | ---------------------- | ------------------------------------------------------------- |
| Slow Mode Range      | 5 ~ "50% Non-DZ range" | **0** : Slow Mode **OFF**                                     |
|                      |                        | "**50% Non-DZ range**" means **0.5 * (Outer DZ - Inner DZ)**  |
|                      |                        | **Autocorrect to Legit Min/Max**                              |
|                      |                        | if **both Inner DZ & Outer DZ** = **OFF**, **Slow Mode OFF**  |
|                      |                        | if **0.5 * (Outer DZ - Inner DZ)** < **5**, **Slow Mode OFF** |
| -------------------- | ---------------------- | ------------------------------------------------------------- |
| Slow Mode Max Output | 5 ~ "Slow Mode Range"  | **Autocorrect to Legit Min/Max**                              |
| -------------------- | ---------------------- | ------------------------------------------------------------- |
| Diagonal Scaling     | 1 ~ 42                 | **0** : Diagonal Scaling **OFF**                              |
|                      |                        | **Autocorrect to Legit Min/Max**                              |
|                      |                        | Only works with "**Rescaling On**"                            |
|                      |                        | if **both Inner DZ & Outer DZ** = **OFF**, **DS OFF**         |

Eagle-eyed readers might spot in **4.1) Slow Mode - Rescaling On**  
``l=64,127,y,s=32,16;r=64,127,y,s=32,16;n;d=0``  
**32 > (127-64)/2**, ``s=32,16`` will get autocorrected to ``s=31,16``  

## Version History

* 1.2.0 (2025-08-03 UTC)
    * Add new **Diagonal Scaling (DS)** feature for fixing inability to reach diagonal Max output in some games
    * Config structure changed again for **DS**, please update your config file to v1.2.0
* 1.1.0 (2025-08-01 UTC)
    * Add new **Slow Mode** feature for better low-magnitude stick movement
    * Can use either **ur0:tai** or **ux0:data/AnalogsEnhancerKai** as config file folder
    * Config structure changed and is no longer compatible with older versions
* 1.0.1 (2025-07-27 UTC)
    * Initial Release
    * Add new **Outer Dead Zone** feature
    * Config file moved to **ur0:tai/**

## License

See the LICENSE.md file for details.  

## Acknowledgements

* [Rinnegatamante](https://github.com/Rinnegatamante), for the original plugin
* [yakit4k0](https://github.com/yakit4k0), for compiling, testing, making README images, and co-developing new features
* u/lizin5ths, for the idea of Outer Dead Zone (fork [Haasman0/AnalogsEnhancer](https://github.com/Haasman0/AnalogsEnhancer))
* [HENkaku 変革](https://henkaku.xyz/) / [Vita SDK](https://vitasdk.org/) Community
