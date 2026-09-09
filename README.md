# Descent 2 for Nintendo Switch (Descent2-NX-Modern)

[![Ko-fi](https://img.shields.io/badge/Ko--fi-Support%20My%20Work-ff5e5b?style=flat&logo=ko-fi&logoColor=white)](https://ko-fi.com/thorhax)
[![Build Status](https://img.shields.io/badge/devkitPro-devkitA64%20r29.2-32a852.svg)](https://devkitpro.org)
[![License](https://img.shields.io/badge/License-GPL%20v2-blue.svg)](COPYING.txt)

A Nintendo Switch port of **Descent II** (based on [D2X-Rebirth](https://www.dxx-rebirth.com/) v0.58.6 and Aaron Gallagher's [DXX-Switch](https://github.com/aagallag/DXX-Switch)), updated and modernized for current devkitPro toolchains, GCC 15, libnx 4.12+, and modern SDL2.

---

## Support My Work

If you enjoy playing retro and classic PC game ports on your Nintendo Switch, consider supporting my work on Ko-fi:

[![Support on Ko-fi](https://ko-fi.com/img/githubbutton_sm.svg)](https://ko-fi.com/thorhax)

Your support helps me maintain, update, and improve homebrew ports for the Nintendo Switch!

---

## Installation Instructions

1. Download the latest `d2x-switch.nro` and `descent.cfg` from the [Releases](https://github.com/Thorhax/Descent2-NX-Modern/releases) section.
2. Create a folder on your Switch SD card at `/switch/d2x-switch/`.
3. Copy `d2x-switch.nro` and `descent.cfg` into `/switch/d2x-switch/`.
4. Copy the Descent 2 game data files into the same `/switch/d2x-switch/` directory:
   - `descent2.ham`
   - `descent2.hog`
   - `descent2.s22`
   - All `*.pig` files
   *(These data files are included with the original game on Steam, GOG, or original CD).*

---

## Building from Source

### Prerequisites
- [devkitPro / devkitA64](https://devkitpro.org/wiki/Getting_Started) with `switch-dev`
- Portlibs:
  ```bash
  sudo dkp-pacman -Syu switch-dev switch-sdl2 switch-sdl2_mixer physfs-switch
  ```

### Compiling
Run `make` to compile the Switch executable:
```bash
make -j$(nproc)
```
To package a release zip (`d2x-switch_v0.58.6.zip`) containing the `.nro` and `descent.cfg`:
```bash
make dist-bin
```

### Docker Build
Alternatively, compile using the official devkitPro Docker image:
```bash
docker run --rm -v $(pwd):/work -w /work devkitpro/devkita64:latest bash -c 'source /opt/devkitpro/switchvars.sh && make dist-bin'
```

---

## Modernizations & Improvements
- **GCC 15 / C23 Compatibility:** Resolved `bool` keyword collisions and standardized function prototypes.
- **Modern devkitPro / libnx Support:** Updated link specifications, `-fcommon` alignment, and fixed system header collisions.
- **SDL2 Transition:** Modernized video and input pipeline to interface directly with devkitPro's `switch-sdl2` and `switch-sdl2_mixer`.
- **PhysicsFS RWops:** Updated `SDL_RWops` abstraction in `physfsrwops.c` to modern SDL2 64-bit size and position signatures.

---

## Credits & License
- Original Descent II by **Parallax Software Corporation**
- DXX-Rebirth team (**Christian Lackas** and contributors)
- Nintendo Switch initial port by **Aaron B. Gallagher** ([aagallag/DXX-Switch](https://github.com/aagallag/DXX-Switch))
- Switch modernization and maintenance by **Thorhax**
- Licensed under the [GNU General Public License v2](COPYING.txt)
