# vpgo 2.0

## Build

### Linux

#### Prerequisite

```sh
apt install curl ca-certificates build-essential ninja-build git zip unzip pkg-config
```

#### Release Build

```sh
./vcpkg-cmake.sh --preset Release
./vcpkg-cmake.sh --build --preset Release
```

### Windows

#### Prerequisites

- git
- Visual Studio 2022

#### Release Build

```
"%ProgramFiles%\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
vcpkg-cmake.bat --preset Release
vcpkg-cmake.bat --build --preset Release
```
