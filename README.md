# WBA SKSE Plugin

This plugin fixes an issue in Skyrim Special Edition where bound arrows add
weight in Survival Mode. Note that the presence of the configuration files means
that you can add any arrow to the "no weight" list, even though it's primarily
designed for bound arrows.

Configuration examples have been provided in the `examples` folder.

## Building Requirements

* [CMake](https://cmake.org/)
* [Vcpkg](https://github.com/microsoft/vcpkg)
  * Add the environment variable `VCPKG_ROOT` with the value as the path to
    the folder containing Vcpkg.
* [Visual Studio Community 2019/2022](https://visualstudio.microsoft.com/)
  * Desktop development with C++.
  * The project currently does not officially support any compilers other than
    MSVC. Everything is written with the assumption that MSVC is the compiler,
    especially because it is also the compiler used for Skyrim SE, hence the
    generated instructions should be more compatible with it.
* TOML Language Support: This is optional since one can live without
  highlighting and syntax checking since you probably won't be working with it
  much.

**Note #1:** Project is set up and catered for Visual Studio. Visual Studio
_Code_ should work, but seems to be occasionally buggy. Certain CMake variables
like `SKYRIM64_DATA_PATH` won't set when doing it from the interface so one
must do it manually, and sometimes it will refuse to find the correct library
paths.

CLion should work if you set it to target MSVC, but it's not tested.

**Note #2:** Any plugin that parses TOML must support at least v1.0 of the
specs. We use certain constructs that are only valid in v1.0+.

## Build Instructions

1. Install and set up dependencies.
   * The CMake script uses the `VCPKG_ROOT` environment variable to locate
     Vcpkg. Make sure this is set before starting your IDE. You may need to kill
     and restart `explorer.exe` for this to apply properly.
2. Clone the project _and_ its submodules using
   `git clone --recurse-submodules <repo_url>`.
3. Open the project in Visual Studio (preferred version: Visual Studio 2022).
4. Set CMake variables. See
   [Important CMake Variables](#important-cmake-variables) section for details.
5. Build the project.

### Using a Different Compiler Version

You can pick a different compiler version than the default in Visual Studio by
setting the following settings:

* Add `-Tv<msvc_version>` to CMake's command arguments.
* Use the Visual Studio generator instead of Ninja (otherwise it won't recognize
  the argument).
  * In Visual Studio 2022, the bundled CMake version does not currently support
    the VS2022 generator as of this writing. In order to use this generator,
    install an up-to-date version of CMake and change the CMake executable path
    to point to it (should be under advanced settings).

    If you also have VS2019 installed, you can use its generator instead.

_Note:_ Within Visual Studio's UI, the generator configuration may be hidden
under advanced settings.

![CMake Configuration](images/cmake-custom-compiler-0-marked.png)

### Important CMake Variables

These variables should be set in `CMakePresets.json` and
`CMakeUserPresets.json`. The environment variable `VCPKG_ROOT` should be set to
the path of Vcpkg.

#### Required

* `SKSE_SUPPORT_XBYAK` - This option comes from CommonLibSSE and is required
  to be set to `ON` for the project to build.
* `SKYRIM_VERSION` - Specifies the version of Skyrim to compile for.

  Valid values: `SE`, `AE`

#### Optional

* `COPY_BUILD` - Copies the built binaries to the appropriate folder after
  building.
* `SKYRIM64_DATA_PATH` - Set this to the path of Skyrim's Data folder. This
  should be filled if `COPY_BUILD` is `ON`.

  _Tip:_ You can also set this to the mod folder you're working on if you're
  using something like Mod Organizer 2.

### Example `CMakeUserPresets.json`

Copy and paste this into `CMakeUserPresets.json`, then modify values as
appropriate. Replace `base-vs2022` with `base-vs2019` if you're using VS2019.

Both configurations are set up to use MSVC 14.2 compiler (VS2019) instead of
14.3. Either install it or remove the `toolset` key from `CMakePresets.json`
if you don't have the compiler installed.

```json
{
    "version": 3,
    "cmakeMinimumRequired": {
        "major": 3,
        "minor": 21,
        "patch": 0
    },
    "configurePresets": [
        {
            "name": "x64-Debug-SE-MSVC142",
            "inherits": [ "base-vs2022", "se" ],
            "cacheVariables": {
                "COPY_BUILD": true,
                "SKYRIM64_DATA_PATH": "D:/Games/MO2 Data/SkyrimSE/mods/WBA - SKSE Plugin (Debug)"
            }
        },
        {
            "name": "x64-Release-SE-MSVC142",
            "inherits": [ "base-vs2022", "se" ],
            "cacheVariables": {
                "COPY_BUILD": true,
                "SKYRIM64_DATA_PATH": "D:/Games/MO2 Data/SkyrimSE/mods/WBA - SKSE Plugin"
            }
        },
        {
            "name": "x64-Debug-AE-MSVC142",
            "inherits": [ "base-vs2022", "ae" ],
            "cacheVariables": {
                "COPY_BUILD": true,
                "SKYRIM64_DATA_PATH": "D:/Games/MO2 Data/SkyrimAE/mods/WBA - SKSE Plugin (Debug)"
            }
        },
        {
            "name": "x64-Release-AE-MSVC142",
            "inherits": [ "base-vs2022", "ae" ],
            "cacheVariables": {
                "COPY_BUILD": true,
                "SKYRIM64_DATA_PATH": "D:/Games/MO2 Data/SkyrimAE/mods/WBA - SKSE Plugin"
            }
        }
    ],
    "buildPresets": [
        {
            "name": "se-debug",
            "displayName": "SE Debug",
            "configuration": "Debug",
            "configurePreset": "x64-Debug-SE-MSVC142"
        },
        {
            "name": "se-release",
            "displayName": "SE Release",
            "configuration": "RelWithDebInfo",
            "configurePreset": "x64-Release-SE-MSVC142"
        },
        {
            "name": "ae-debug",
            "displayName": "AE Debug",
            "configuration": "Debug",
            "configurePreset": "x64-Debug-AE-MSVC142"
        },
        {
            "name": "ae-release",
            "displayName": "AE Release",
            "configuration": "RelWithDebInfo",
            "configurePreset": "x64-Release-AE-MSVC142"
        }
    ]
}
```
