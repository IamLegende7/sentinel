#include "render/texture_agent_gpu.hpp"

TextureAgentGPU::TextureAgentGPU(SDL_Renderer& renderer, TextureAgentGPUSettings settings) : renderer(renderer), settings(settings) {
    LOGGER.log(LogLevel::INFO, "[texture_agent.cpp:TextureAgent] Initailized TextureAgentGPU.");
}

void TextureAgentGPU::load_texture(std::string path, std::string name) {
    std::filesystem::path texture_path = path;

    // Check for existing textures //
    if (name != "none") {
        if (get_texture_exists(name)) {
                if (DEBUG["all_debug_logs"]) {
                    LOGGER.log(LogLevel::DEBUG, "[texture_agent.cpp:load_texture] Caught texture from loading twice!");
                }
                return 2;
        }
    }

    // Load Texture //
    if (texture_path.extension() == ".png") { // ends on .png --> simple texture
        std::string texture_path_full = texture_path;
        SDL_Texture* texture = IMG_LoadTexture(renderer, texture_path_full.c_str());
        if (!texture) {
            LOGGER.log(LogLevel::WARNING, "[texture_agent.cpp:load_texture] Failed to load texture: %s", SDL_GetError());
            return 1;
        } else {
            SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
            agent_textures.push_back({name, texture});
            return 0;
        }
    } else if (texture_path.extension() == ".json" || texture_path == ".jsonc" ) { // ends on .json / .jsonc --> spritesheet / animated sprite / whatever
        LOGGER.log(LogLevel::ERROR, "Spritesheets not coded yet!");
    }
}