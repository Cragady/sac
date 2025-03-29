git clone git@github.com:Cragady/sac.git --recurse-submodules --depth 2 -j8

## Compilers

* MinGW
* NASM
* https://www.nasm.us/pub/nasm/releasebuilds/2.16.03/win64/
  * Install is on user or global (windows) - add install dir to path
  * set nev var of `ASM_NASM` to `nasm.exe` (windows, user or global)

### CMAKE Setup

I have an environment variable that sets the default target for cmake to be the `MinGW64` compiler. For Windows, Cmake usually defaults to Visual Studio.
