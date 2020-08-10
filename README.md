# Texture Compression

This is an example how to compress textures via OpenGL. No software compression, everything is done on the hardware by the GPU. No special OpenGL version, all you need is OpenGL 3.3 core profile (or later) and `GL_EXT_texture_compression_rgtc` + `GL_EXT_texture_compression_s3tc` extensions.

The compression is done in `src/Compressor.cpp` file. This works by rendering the source image into a framebuffer, then copying the pixels into a destination texture while specifying the target format (for example DXT5). You can also use the `glGetCompressedTexImage` function to get the compressed pixels (maybe save them to a file?). See the `compress` function for more details. 

![sample.png](sample.png)

## Usage

Compile the application via CMake and vcpkg (steps below). Run the `TextureCompression.exe` executable. **Press spacebar on your keyboard to switch between compression types.**

## Building

* Make sure you have vcpkg installed and integrated.
* Install `glfw3 glm glad` vcpkg packages.
* Build the project with vcpkg toolchain.

```
# Vcpkg dependencies
vcpkg install --triplet x64-windows glfw3 glm glad

# Clone
git clone https://github.com/matusnovak/texture-compression.git
cd texture-compression

# Create build dir and configure
mkdir build
cd build
cmake \
  -G "Visual Studio 16 2019" \
  -DCMAKE_TOOLCHAIN_FILE=C:/tools/vcpkg/scripts/buildsystems/vcpkg.cmake \
  ..

# Build it
cmake --build .

# Run it
./Debug/TextureCompression.exe
```