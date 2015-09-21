# 3DSRemote

A remote control homebrew application for 3DS. Designed to integrate nicely with the [Left4VR](https://github.com/wchill/Left4VR) and [MultiInput](https://github.com/wchill/MultiInput) remote input systems, but can realistically be used for any application if needed.

So far it does not do much other than report the state of the buttons (including ZL/ZR on N3DS), Circle Pad and C-stick (N3DS only), and touch screen.

### Building/Installation

Install devkitPro and devkitARM and set the environment variables for DEVKITPRO/DEVKITARM to point to their respective locations.

Build the project using `make`, clean using `make clean`.

To install, the 3DS Homebrew Channel is required. This means that (as of this writing) either Ninjhax, Ironhax, Tubehax, or Smashbroshax must be used. Theoretically all firmware versions up to 10.1.0-27 should be supported, regardless of 3DS model, but this has only been tested on a New 3DS XL running 9.0.0-20U with Ninjhax as the entry point. Copy the 3dsx and smdh files from the build into the appropriate directory on the SD card and launch from the Homebrew Channel.

### Usage

Because this first version does not have support for UDP multicast and network service discovery, the IP and port to connect to must be changed in `main.c` and then the project must be rebuilt, as described above. Run the server-side program (depending on what this application will be used for; I have included a sample server in `PC\server-echo.py`) and then launch 3DSRemote from the Homebrew Channel.
