# Basalt Engine
## Developing on Windows 10
### Prerequisites
* A recent version of Windows 10 or 11 (x86-64)
* Git
* CMake 3.18 or newer
* MSVC v143 v14.36 or newer
* Windows 8.1 SDK or newer
* [DirectX SDK June 2010](https://www.microsoft.com/en-us/download/details.aspx?id=6812)
* [Latest data folder](https://github.com/juli27/basaltcpp/releases/tag/data-v1)

### Generating the Visual Studio Solution
    git clone https://github.com/juli27/basaltcpp.git
    cd basaltcpp
    cmake -S . -B ./_vs

## OS Support
### Windows
* Windows 8.1
* Windows 10 21H2 November 2021 Update (Build 19044)
* Windows 10 22H2 2022 Update (Build 19045)
* Windows 11 21H2 (Build 22000)
* Windows 11 22H2 2022 Update (Build 22621)

> The following Windows versions probably still work as features not available
in these versions haven't been made mandatory yet:
> * Windows Vista
> * Windows Vista SP1
> * Windows Vista SP2
> * Windows Vista SP2 with Platform Update
> * Windows 7
> * Windows 7 SP1
> * Windows 7 SP1 with Platform Update
> * Windows 8
> * Windows 10 1507 TH1 (Build 10240)
> * Windows 10 1511 TH2 November Update (Build 10586)
> * Windows 10 1607 RS1 Anniversary Update (Build 14393)
> * Windows 10 1703 RS2 Creators Update (Build 15063)
> * Windows 10 1709 RS3 Fall Creators Update (Build 16299)
> * Windows 10 1803 RS4 April 2018 Update (Build 17134)
> * Windows 10 1809 RS5 October 2018 Update (Build 17763)
> * Windows 10 1903 19H1 May 2019 Update (Build 18362)
> * Windows 10 1909 19H2 November 2019 Update (Build 18363)
> * Windows 10 2004 20H1 May 2020 Update (Build 19041)
> * Windows 10 20H2 October 2020 Update (Build 19042)
> * Windows 10 21H1 May 2021 Update (Build 19043)

## Hardware Support
* CPU: x86-64
* GPU: Direct3D 9.0c, Shader Model 2.0
