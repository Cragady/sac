# SAC (Simple Auto Clicker)

This repo is meant to be a simple auto-clicker created in `c` using the `clay` lib. I'm developing this for `Windows` first, then eventually maybe for `Linux` targeting `X11`, and maybe `Wayland` if I'm feeling ambitious enough. The reason I need to target these items is so that I can initiate clicks as if it's coming from the OS. On Windows, I can do this with `windows.h`. I haven't delved too deeply in OS specific GUI development, so I don't know what that would look like on Linux - Maybe it's something different, or maybe it's as I mentioned before: `X11` and/or `Wayland` to help me initiate clicks without the program having focus.

There are some auto-clickers that claim to be as fast as 50k/second, and that quite frankly, is amazing if true. I'm having a hard enough of a time pushing 300/second, and 1180/second on a good day. My use-case for this small program will be nowhere near that. This program can initate more clicks per second than a lot of games' framerate anyway.

This is still in its infancy, and I hope to take this further. I have basic functionality to:

* Start clicks without having program focused
* Stop clicks without having program focused
* Close program without having program focused

## Usage

* Capslock + 5 - Sarts/Stops clicking
* Capslock + 4 - Closes Program

The program will initiate another capslock keypress to return the capslock to the state it was before you entered a command.

## Get / Build

There may be other dependencies, but I don't want to track that right now.

```sh
# Cloning
git clone git@github.com:Cragady/sac.git --recurse-submodules --depth 2 -j8
```

```sh
# building
make
make build
```

I used a Makefile to control `cmake` commands because I didn't want to keep typing whatever commands were in there. Why type a command with several arguments when you can just type a command with two?

## Compilers

* MinGW
* NASM
* https://www.nasm.us/pub/nasm/releasebuilds/2.16.03/win64/
  * Install is on user or global (windows) - add install dir to path
  * set nev var of `ASM_NASM` to `nasm.exe` (windows, user or global)

### CMAKE Setup

I have an environment variable that sets the default target for cmake to be the `MinGW64` compiler. For Windows, Cmake usually defaults to Visual Studio.

## Future Plans

In the future I plan to:

* Create a simple scripting language to move mouse and click mouse according to what's scripted, and to do it over time to ideally get close to human mouse movements and clicks
* Create a text editor to modify an in-memory script
* Create save functionality to save in-memory script to disk
* Create load functionality to load script into memory for modification and running
* Make gui look better
* Provide more in-depth controls

This program is the bare-bones of a bare-bones project. There's not a lot here, but it has been fun so far. Hopefully I won't abandon this, but for what it does, it works for my purposes and I don't have much need outside of that. It would be cool to get a few more of these goals fleshed out though.

## Issues

### Lag

NOTE: The lag issue seems to be fixed for now? We'll have to keep a tentative eye on it. Click speeds were able to regularly get higher than 1100 clicks per second, but browsers hate this much clicking, so I had to create a small program to act as a click speed test.

For some reason, having the program click to fast will lag everything. There must be some atrocities I'm committing to cause this, as a simple c prog to click as fast as it wants to does not have an issue.

A cap of 100 clicks per second seems to be a safe max of what this program can do, which is down from 347 clicks per second. Higher numbers may be possible, but 100 is the number I landed on.

### Garbage in Strings

Pretty much this. I may be committing more atrocities, which is evidenced by occasional garbage in the Status page. I believe I addressed this with a couple of `memset`s, but a few eyes may be needed to make sure it's truly gone. Can't reproduce the garbage consistently.
