# [AnalogsEnhancerKai](https://github.com/t0mizwf26/AnalogsEnhancerKai/) for PS Vita

A fork of PS Vita kernel plugin [Rinnegatamante](https://github.com/Rinnegatamante)/[AnalogsEnhancer](https://github.com/Rinnegatamante/AnalogsEnhancer)  
By [t0mizwf26](https://github.com/t0mizwf26) & [yakit4k0](https://github.com/yakit4k0) (u/Yakitako-Kun)  

With new features like **Outer Dead Zone** and **Slow Mode**  

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

### 1) Introduction

If .txt config file is missing, the following default config will be used instead.  
``l=0,127,n,s=0,0;r=0,127,n,s=0,0;n``  
Which means every feature offered by this plugin is turned **OFF**.  

![l=0,127,n,s=0,0;r=0,127,n,s=0,0;n](https://github.com/user-attachments/assets/8b9586f9-aa5f-4e8d-8bf8-9fd243acee22)  

### 2) Inner Dead Zone

Classic feature from the original Rinnegatamante/AnalogsEnhancer.  
With 2 different modes to choose from: **Rescaling On** & **Rescaling Off**.  

#### 2.1) Inner Dead Zone - Rescaling On (Scaled Radial Dead Zone)

``l=64,127,y,s=0,0;r=64,127,y,s=0,0;n``  
This is probably the most common Dead Zone people would use to fix drifting analogue sticks.  
The full range is available, no input is ignored (skipped), allowing low-magnitude adjustments.  

![l=64,127,y,s=0,0;r=64,127,y,s=0,0;n](https://github.com/user-attachments/assets/948025db-147b-44b8-a299-be4e982d5ee9)  

#### 2.2) Inner Dead Zone - Rescaling Off (Radial Dead Zone)

``l=16,127,n,s=0,0;r=16,127,n,s=0,0;n``  
Fewer people would choose to use this. Less ideal, some input is ignored (skipped).  
In this example, low-magnitude adjustments with magnitude < 16 are impossible.  
There's one advantage, though. Notice the gradient here is still **1**?  
Unlike the previous **Rescaling On** example with gradient **2**.  
This means it's a little easier to make precise adjustments for the remaining range (16 to Max).  

![l=16,127,n,s=0,0;r=16,127,n,s=0,0;n](https://github.com/user-attachments/assets/0cfbe53d-af94-4abb-be30-54aeea9d497f)  

### 3) Outer Dead Zone

Outer Dead Zone is useful with situations like  
> "since putting little joystick covers on them makes it extremely difficult to hit the maximum values."  
> u/lizin5ths  

Outer Dead Zone with **Rescaling On** could help to solve the problem by reaching the max earlier.  
The code is based on u/lizin5ths's implementation (fork [Haasman0 / AnalogsEnhancer](https://github.com/Haasman0/AnalogsEnhancer)), with many improvements.  

Outer Dead Zone with **Rescaling Off** however, is something a little different. More on that below.  

#### 3.1) Outer Dead Zone - Rescaling On (360 Degree Movement)

``l=0,80,y,s=0,0;r=0,80,y,s=0,0;n``  
In this example, the stick reaches the max much earlier.  
Bring more _cute cat paw claw grip cap cover_!!.  

![l=0,80,y,s=0,0;r=0,80,y,s=0,0;n](https://github.com/user-attachments/assets/134a7a74-48e5-4358-a695-c4c5db7b43ae)  

#### 3.2) Outer Dead Zone - Rescaling Off (8-Way, "Axial Dead Zone")

``l=0,80,n,s=0,0;r=0,80,n,s=0,0;n``  
The problem is solved by using **Rescaling On**.  
Q: What should **Rescaling Off** do? A: **Go retro.**  
Now here's something niche.  

![l=0,80,n,s=0,0;r=0,80,n,s=0,0;n](https://github.com/user-attachments/assets/5120397e-85de-4eb6-a417-5c4ded417ec3)  

#### 3.3) Whoa, hang on a second!

Why should people use a normal 360-degree analogue stick with only the outer one-third as 8-Way?  
Right, but we could **use Inner & Outer Dead Zone together**.  

``l=80,80,n,s=0,0;r=80,80,n,s=0,0;n``  

Note the **8-Way always has an "Axial Dead Zone" of magnitude 49**.  
So by setting "Inner Dead Zone = Outer Dead Zone", we could have an **8-Way joystick with customisable actuation point (49 to Max-1)**  

![l=80,80,n,s=0,0;r=80,80,n,s=0,0;n](https://github.com/user-attachments/assets/c85f6bd1-60c4-43d3-bbcd-b180e1ffa961)  

``l=80,80,y,s=0,0;r=80,80,y,s=0,0;n``  

You probably guessed we could do the same with **Rescaling On**, 360-Degree Movement with Max output only.  
However, personally, I can't think of a reason to.  

![l=80,80,y,s=0,0;r=80,80,y,s=0,0;n](https://github.com/user-attachments/assets/b64aa177-72c8-4944-a29b-a2ba45fb4416)  

### 4) S for Sniper? Snail? Shooter? ...Well, it's "Slow Mode"

Very boring name I know. Just call it Sniper / Snail / Shooter / High-Precision Mode, or whatever you like.  
I'm just way too lazy to change all related names in the code, so I will keep calling it: "Slow Mode".  

Remember earlier example in **2.1) Inner Dead Zone - Rescaling On (Scaled Radial Dead Zone)**?  
``l=64,127,y,s=0,0;r=64,127,y,s=0,0;n``  
Inner 50% = Dead Zone, gradient is "2" instead of the normal "1". Everything's so fast, it's hard to make tiny adjustments!  
Let's try to fix this, but unfortunately with a price to pay.  

#### 4.1) Slow Mode - Rescaling On

``l=64,127,y,s=32,16;r=64,127,y,s=32,16;n``  
Change ``s=0,0`` to ``s=32,16``, here's the result:  

![l=64,127,y,s=32,16;r=64,127,y,s=32,16;n](https://github.com/user-attachments/assets/ab320a13-e733-45dc-9e68-dbb877dcaea8)  

Now it's easier to sneak / snipe.  
However, the outer half is sacrificed. Gradient is increased, making the outer half even harder to do tiny adjustments.  

#### 4.2) Slow Mode - Rescaling Off

``l=16,127,n,s=32,16;r=16,127,n,s=32,16;n``  
Also change ``s=0,0`` to ``s=32,16``, see what happens here when **Rescaling Off**:  

![l=16,127,n,s=32,16;r=16,127,n,s=32,16;n](https://github.com/user-attachments/assets/942dbd2c-86f8-42fd-8389-0b63ff16f261)  

In this particular example, within Slow Mode range (32), the output magnitude will always be "16".  

#### 4.3) Have some fun

``l=16,80,n,s=32,8;r=16,80,n,s=32,8;n``  
Who's going to do this?  

![l=16,80,n,s=32,8;r=16,80,n,s=32,8;n](https://github.com/user-attachments/assets/391ce4da-e428-46b5-be93-5e1bf828e5ab)

### 5) ANALOG_WIDE

That last **"n"** in ``l=0,127,n,s=0,0;r=0,127,n,s=0,0;n``  

Also a classic feature from the original Rinnegatamante/AnalogsEnhancer.  

Please allow me to quote from [Rinnegatamante](https://github.com/Rinnegatamante)
> ANALOG_WIDE is one of the two modalities to poll analogs from Vita OS (one being ANALOG that simulates PSP polling and ANALOG_WIDE).  
> In ANALOG mode (used even in several commercial games as well as PSP and PSX titles), max range of analogs is reduced thus making analogs feel less reactive.

It's a very nice feature but might ["causes issues in some games"](https://github.com/Rinnegatamante/AnalogsEnhancer/issues/2).  
So it's disabled by default in AnalogsEnhancerKai.  
If needed, please re-enable it manually (by changing that last **n** into **y**).  

## Understand & Customise Config File

``l=0,127,n,s=0,0;r=0,127,n,s=0,0;n``  

Break it into 3 sections:  

| Section     | Content          |
| ----------- | ---------------- |
| Left Stick  | l=0,127,n,s=0,0; |
| Right Stick | r=0,127,n,s=0,0; |
| ----------- | ---------------- |
| ANALOG_WIDE | n                |

Now, we could see **Left Stick** and **Right Stick** configs sharing the same structure:  
``l(/r)=0,127,n,s=0,0;``  

Each part means:  

| Config       | Meaning                               |
| ------------ | ------------------------------------- |
| l(/r)=0,127, | Inner Dead Zone, Outer Dead Zone      |
| n,           | Rescaling (y=ON ; n=OFF)              |
| s=0,0;       | Slow Mode Range, Slow Mode Max Output |
| ------------ | ------------------------------------- |
| n            | **5) ANALOG_WIDE**, (y=ON ; n=OFF)    |

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
|                      |                        | **Autocorrect to Legit Range**                                |
|                      |                        | If **both Inner DZ & Outer DZ** = **OFF**, **Slow Mode OFF**  |
|                      |                        | If **0.5 * (Outer DZ - Inner DZ)** < **5**, **Slow Mode OFF** |
| -------------------- | ---------------------- | ------------------------------------------------------------- |
| Slow Mode Max Output | 5 ~ "Slow Mode Range"  | **Autocorrect to Legit Range**                                |

Eagle-eyed readers might spot in **4.1) Slow Mode - Rescaling On**  
``l=64,127,y,s=32,16;r=64,127,y,s=32,16;n``  
**32 > (127-64)/2**, ``s=32,16`` will get autocorrected to ``s=31,16``  

## Version History

* 1.1.0
    * Add new **Slow Mode** feature for better low-magnitude stick movement
    * Can use either **ur0:tai** or **ux0:data/AnalogsEnhancerKai** as config file folder
    * Config structure changed and is no longer compatible with older versions
* 1.0.1
    * Initial Release
    * Add new **Outer Dead Zone** feature
    * Config file moved to **ur0:tai/**

## License

See the LICENSE.md file for details.  

## Acknowledgements

* [Rinnegatamante](https://github.com/Rinnegatamante), for the original plugin
* [yakit4k0](https://github.com/yakit4k0), for compiling, testing, and helping to implement Outer Dead Zone & Slow Mode
* u/lizin5ths, for the idea of Outer Dead Zone (fork [Haasman0/AnalogsEnhancer](https://github.com/Haasman0/AnalogsEnhancer))
* [HENkaku 変革](https://henkaku.xyz/) / [Vita SDK](https://vitasdk.org/) Community
