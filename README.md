This repo contains source code for GAMES101 homeworks. It can only be built with latest VS2022 under release mode.

It's recommended to follow the [official VS instructions](https://docs.microsoft.com/en-us/cpp/build/cmake-projects-in-visual-studio) to open the top level CMake project in VS2022.
Alternatively, run the following commands to create the traditional VS project:
```sh
mkdir build
cd build
cmake -S .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
```