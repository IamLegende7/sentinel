#ifndef TEXTURE_AGENT_GPU_HPP
#define TEXTURE_AGENT_GPU_HPP

#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL.h>
#include <string>
#include <filesystem>

#include "utils/logger.hpp"
#include "settings/locations.hpp"
#include "settings/debug.hpp"

/*
   Holds the Data of a ```SDL_GPUTexture``` of ```TextureAgentGPU```
*/
struct TextureAgent_GPUTextureData {
    std::string name;
    SDL_GPUTexture texture;
};

struct TextureAgent_TextureData {
    std::string name;
    SDL_Texture texture;
};

struct TextureAgentGPUSettings {
    bool multithreading;
    bool old_renderring;

    TextureAgentGPUSettings(bool multithreading = false, bool old_renderring = false) : multithreading(multithreading), old_renderring(old_renderring) {}
    ~TextureAgentGPUSettings() {

    }
};

/* TextureAgent but (optionally) using SDL_GPU;
     - Better, faster, more performant renderring,
     - Sprite Batching, Spritesheets
     - optional multithreading
*/
/* Manages the Textures in the Game:
       - Loading textures
       - Rendering Textures
       - Managing Spritesheets
     Usage:
       - construct the Agent; pass in the renderer: ```TextureAgent my_agent(myrenderer)```
       - check for any errors by running ```my_angent.checkerrors()```. this will return ```true``` if there aren't any errors
*/
class TextureAgentGPU {
    private:
        TextureAgentGPUSettings settings;
        SDL_Renderer& renderer;
        std::vector<auto> agent_textures;

    public:
        // Init & Cleanup //
        TextureAgentGPU(SDL_Renderer& renderer, TextureAgentGPUSettings settings = TextureAgentGPUSettings());
        ~TextureAgentGPU();

        // Texture Management //
        void load_texture(std::string path, std::string name = "none");
};



#endif