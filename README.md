# SAC (Simple Auto Clicker)

This repo is meant to be a simple auto-clicker created in `C` using the `cimgui` lib wrapper for `imgui`. I'm developing this for `Windows` first, then eventually maybe for `Linux` targeting `X11`, and maybe `Wayland` if I'm feeling ambitious enough. The reason I need to target these items is so that I can initiate clicks as if it's coming from the OS. On Windows, I can do this with `windows.h`. I haven't delved too deeply in OS specific GUI development, so I don't know what that would look like on Linux - Maybe it's something different, or maybe it's as I mentioned before: `X11` and/or `Wayland` to help me initiate clicks without the program having focus.

There are some auto-clickers that claim to be as fast as 50k/second, and that quite frankly, is amazing if true. I'm having a hard enough of a time pushing 300/second, and 1180/second on a good day. My use-case for this small program will be nowhere near that. This program can initate more clicks per second than a lot of games' framerate anyway.

This is still in its infancy, and I hope to take this further. I have basic functionality to:

* Start clicks without having program focused
* Stop clicks without having program focused
* Close program without having program focused
* Display mouse position on Status Page
* Display if currently clicking on Status Page
* Optional programs built to measure clicks per second (this will likely be better than a browser if you have hundreds of tabs open like me)

Optional programs built in C++ because Win32 API likes C++ just a little bit better than C. I could've found a way to do it purely in C, but OOP made it fly by just a bit quicker, and since these metric programs aren't the main focus here, I didn't want to do it in C. And as much of a pain `windows.h` is, I didn't want to fight against it more than necessary. `FactoryBuildFactoryToBuildSomeStructThatFeedsIntoAnotherClassToConstructALittleBrush`.

## Clay to ImGui

As of the commit that this readme update is happening, I'll be moving from Clay to ImGui. There may be what I'm looking for in Clay, but I know where it is in ImGui without loafing through a lot of code. I want an input field that handles input, but I also do not want to have to build it myself. That could possibly be a latter project, but for now I want to focus on the auto-clicker part of this project.

## Usage

* Capslock + 5 - Sarts/Stops clicking
* Capslock + 4 - Closes Program

The program will initiate another capslock keypress to return the capslock to the state it was before you entered a command.

## Dependencies

* Win32 API
* (Everything in Vendor directory)
* X11 (Future)
* Wayland (Future)
* Vulkan
* Make
* CMake

### Required Installs

* [Vulkan](https://vulkan.lunarg.com/sdk/home)

### Optional Program Dependencies

* Win32 API
* Direct2D API
* DirectWrite API
* X11 (Future)
* Wayland (Future)

## Get / Build

There may be other dependencies, but I don't want to track that right now.

```sh
# Cloning
git clone git@github.com:Cragady/sac.git --recurse-submodules -j8
```

```sh
# building
make
make build
```

I used a Makefile to control `cmake` commands because I didn't want to keep typing whatever commands were in there. Why type a command with several arguments when you can just type a command with two?

## Compilers

* MinGW (Windows)
* GCC (Linux (Future))
* NASM (Windows/Linux)
* https://www.nasm.us/pub/nasm/releasebuilds/2.16.03/win64/
  * Install is on user or global (windows) - add install dir to path
  * set nev var of `ASM_NASM` to `nasm.exe` (windows, user or global)

### CMAKE Setup

I have an environment variable that sets the default target for cmake to be the `MinGW64` compiler. For Windows, Cmake usually defaults to Visual Studio.

```sh
CMAKE_GENERATOR=MinGW Makefiles
```

## Future Plans

In the future I plan to:

* Create a linux build, and modify cmake files to dynamically handle `Windows`, `X11`, and `Wayland`
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

### Click Queue Buildup

If the clicker is being ran for a bit, the click queue seems to build up and stopping the clicks is not immediate. The reason for this is trying to squeeze out more clicks by sending an input buffer with a length greater than 1 to `SendInput`.

### Questionable Clicks Measuring Methodology

I'm having to use GUI based programs to capture clicks. This in and of itself will introduce some inaccuracy in collecting the amount of clicks the program is actually initiating. I can use the program itself to give an *estimated* amount of clicks, but that's like asking a government agency to audit itself for any wrongdoing. Unreliable at best, and straight up fantasy land reporting at worst.

### Clicks Per Second not Hitting Target

The rendering of the GUI is preventing accurate time for sending clicks. This can be resolved by buffering multiple clicks in one command by multiplying wanted clicks by delta_time and flooring the value and keeping track of click loss and adding the lost clicks appropriately.

An alternative solution is to not multiply clicks per second, and just calculate click loss and add the loss of clicks as soon as it's greater than 1.

## Solved / Innacurate Issues

### ~Questionable Clicks Per Second Setting~

> The clicks per second aren't exactly where they should be.

This issue is not from a questionable setting.
