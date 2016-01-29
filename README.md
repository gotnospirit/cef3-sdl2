# cef3-sdl2
Cef 3 off-screen rendering example for SDL based on [CEF OSR](https://github.com/qwertzui11/cef_osr)

# Building
Assuming '$project_dir' is the directory where this project lives

## Get SDL 2
Download [SDL 2](https://www.libsdl.org/download-2.0.php) development libraries and unzip to '$project_dir\sdl'

## Get CEF 3
1. download [Chromium Embedded Framework 3](https://cefbuilds.com/) and unzip somewhere
2. cd $somewhere
3. build samples & libcef_dll :
    - mkdir build
    - cd build
    - cmake .. && cmake --build .
4. copy :
    - 'build\libcef_dll\Debug\libcef_dll_wrapper.lib' to '$project_dir\vendor\cef\lib'
    - 'Debug\\*' & 'Resources' to '$project_dir\vendor\cef'

## Build this project
1. cd $project_dir
2. mkdir build
3. cd build
4. cmake .. && cmake --build .
