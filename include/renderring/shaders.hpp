#ifndef SHADERS_HPP
#define SHADERS_HPP

#include <SDL3/SDL.h>

SDL_GPUShader* LoadShader(SDL_GPUDevice* device, const char* shaderFilename, Uint32 samplerCount, Uint32 uniformBufferCount, Uint32 storageBufferCount, Uint32 storageTextureCount);

#endif