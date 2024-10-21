# huawei-hackathon

## Getting started

1. Compile

```bash
g++ ./main.cpp -o scheduler -Werror --std=c++11
```

2. Run

```bash
./scheduler.exe -i input_path -o output_path
```

Discard the options to use the standard i/o by default

## Getting started with CMake

CMake is a cross-platform compile method build system generator. Instead of using Visual Studio, CMake is an industry standard software--great with C++!

1. Install CMake

2. Go into the build directory (or make one if you don't have one)
```bash
mkdir build
cd build
```

3. Generate build files & Compile

```
cmake ..
make -j
```

Now you have compiled and generated the build files!

4. Call the executable (aka main.cpp)

Make sure you're in the build directory

```bash
./target -input ../input -output ../output
```
