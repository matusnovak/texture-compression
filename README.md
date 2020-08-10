# Texture Compression

This is an example how to compress textures via OpenGL. No software compression, everything is done on the hardware by the GPU. No special OpenGL version, all you need is OpenGL 3.3 core profile (or later) and `GL_EXT_texture_compression_rgtc` + `GL_EXT_texture_compression_s3tc` extensions.

The compression is done in `src/Compressor.cpp` file. This works by rendering the source image into a framebuffer, then copying the pixels into a destination texture while specifying the target format (for example DXT5). You can also use the `glGetCompressedTexImage` function to get the compressed pixels (maybe save them to a file?). See the `compress` function for more details. 

It works like this:

* Load the source image file into pixel array.
* Create a "source" texture from the source image pixels, in any format.
* Create an empty "fboColor" texture.
* Create a framebuffer object.
* Optionally attach a depth buffer to the framebuffer object (might be needed on some hardware).
* For each mimap level:
  * Set the "fboColor" texture storage via `glTexImage2D` with no source data (last parameter is NULL).
  * Attach the "fboColor" texture to the framebuffer with the mipmap level.
  * Bind and render the "source" texture.
* Create an empty "destination" texture.
* For each mimap level:
  * Attach the "fboColor" texture to the framebuffer with the mipmap level.
  * Bind the "destination" texture.
  * Call `glCopyTexImage2D` with DXT or RGTC format (3rd param).
  * Optionally call `glGetTexLevelParameteriv` with `GL_TEXTURE_COMPRESSED_IMAGE_SIZE` to get the size of the current mimap level (in bytes).
  * Optionally call `glGetCompressedTexImage` to get the raw compressed pixels.
* Delete framebuffer, renderbuffer, "source" texture, "fboColor" texture.
* Update the "destination" texture max and min mipmap levels via `glTexParameteri` (the `GL_TEXTURE_BASE_LEVEL` and `GL_TEXTURE_MAX_LEVEL`).
* Keep the "destination" texture.

![sample.png](sample.png)

Sample image `lena.png` was converted from `lena.tiff` downloaded from http://eeweb.poly.edu/~yao/EL5123/SampleData.html

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
