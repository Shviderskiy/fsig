# fsig

TODO: description

## Command line syntax
```
Usage: fsig input output [options]
options:
  -h [ --help ]                Help
  -v [ --version ]             Version
  -i [ --input ] arg
  -o [ --output ] arg
  --block-size arg (=1MB)
  --io-block-size arg (=128KB)
  --threads-count arg (=4)
  --hash-algo arg (=MD5)
```

## Clone repository and update submodules
```bash
git clone https://github.com/Shviderskiy/fsig.git
cd fsig
```

## Install Conan
https://conan.io/downloads.html

## Add Conan remove
```bash
conan remote add bincrafters https://api.bintray.com/conan/bincrafters/public-conan
```

## Create directory for build
```bash
mkdir build
cd build
```

## Install dependencies, configure and build project

### On Windows with MSVC (TODO: finish it)

- Install conan dependencies:
  - Debug:
    ```bash
    conan install .. -s arch=x86 -s arch_build=x86 -s build_type=Debug -s compiler.runtime="MTd" --build missing
    ```
  - Release:
    ```bash
    conan install .. -s arch=x86 -s arch_build=x86 -s build_type=Release -s compiler.runtime="MT" --build missing
    ```

### On Linux with GCC

- Debug x64:
  ```bash
  conan install .. --profile ../conan_profiles/gcc6.2_debug_x64.txt --build missing

  cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-m64"
  cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-m64"

  cmake --build . --config Debug
  ```
- Release x64:
  ```bash
  conan install .. --profile ../conan_profiles/gcc6.2_release_x64.txt --build missing

  cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-m64"
  cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-m64"

  cmake --build . --config Release
  ```

### On Windows with MinGW

- Set path to gcc (for example Qt toolset):
  - cmd:
    ```bash
    set PATH=C:\Qt\Tools\mingw730_64\bin;%PATH%
    ```
  - PowerShell:
    ```bash
    $Env:path += ";C:\Qt\Tools\mingw730_64\bin"
    ```
  - git bash:
    ```bash
    set PATH="$PATH:/c/Qt/Tools/mingw730_64/bin"
    ```

- Debug x64:
  ```bash
  conan install .. --profile ../conan_profiles/mingw7.3_debug_x64.txt --build missing

  cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-m64"
  cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="-m64"

  cmake --build . --config Debug
  ```
- Release x64:
  ```bash
  conan install .. --profile ../conan_profiles/mingw7.3_release_x64.txt --build missing

  cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-m64"
  cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS="-m64"

  cmake --build . --config Release
  ```
