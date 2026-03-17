# Read the Json Data

Code to read a json file filled with shapes and lights.

# Running the code with command-line arguments

### Building 

To build this project, run the following commands:
```
cd buildVCPkg
cmake --build .
```

or to use Release to get a faster runtime:
```
cd buildVCPkg
cmake --build . --config Release
```

### Running

After building this project in `buildVCPkg`, run the excetutable with optional arguments to control rendering settings:
```
./src/Debug/main.exe [filepath]
[--width image_width] [--height image_height] [--rpp rays_per_pixel] [--recursionDepth max_depth] [--depth focallength]
```
where `[filepath]` can be e.g., `../src/sceneData/scenes_A/PhongExp.json`, which is the location of the file `PhongExp.json`.
