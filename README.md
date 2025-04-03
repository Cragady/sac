git clone git@github.com:Cragady/sac.git --recurse-submodules --depth 2 -j8

## Compilers

* MinGW
* NASM
* https://www.nasm.us/pub/nasm/releasebuilds/2.16.03/win64/
  * Install is on user or global (windows) - add install dir to path
  * set nev var of `ASM_NASM` to `nasm.exe` (windows, user or global)

### CMAKE Setup

I have an environment variable that sets the default target for cmake to be the `MinGW64` compiler. For Windows, Cmake usually defaults to Visual Studio.

## Issues

### Lag

NOTE: The lag issue seems to be fixed for now? We'll have to keep a tentative eye on it. Click speeds were able to regularly get higher than 1100 clicks per second, but browsers hate this much clicking, so I had to create a small program to act as a click speed test.

For some reason, having the program click to fast will lag everything. There must be some atrocities I'm committing to cause this, as a simple c prog to click as fast as it wants to does not have an issue.

A cap of 100 clicks per second seems to be a safe max of what this program can do, which is down from 347 clicks per second. Higher numbers may be possible, but 100 is the number I landed on.
