This repo contains source code for GAMES101 homeworks. It can only be built with latest VS2022 under release mode.

HW8 requires freetype.dll as an external dependency. This dll will be automatically installed after installing [Anaconda](https://www.anaconda.com/) Python package. If it's not
available, download it from [here](https://github.com/ubawurinna/freetype-windows-binaries) and place the dll to the final build directory or C:\Windows\System32.

It's recommended to follow the [official VS instructions](https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio) to open the top level CMake project in VS2022.
Alternatively, run the following commands to create the traditional VS project:
```sh
mkdir build
cd build
cmake -S .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
```