#ifndef TEXTURE_AGENT_HPP
#define TEXTURE_AGENT_HPP

#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL.h>
#include <string>

#include "main.h"
#include "logger.hpp"
#include "locations.hpp"
#include "debug.hpp"

/*
   Holds the Data of a ```SDL_Texture``` of ```TextureAgent```
*/
struct AgentTextureData {
    std::string name;
    SDL_Texture* texture = NULL;
};

/*
   See src/render/texture_agent.cpp
*/
class TextureAgent {
    private:
        SDL_Renderer* renderer = nullptr;
        std::vector<AgentTextureData> agent_textures;
          
    public:
        // Init and cleanup //
        TextureAgent(SDL_Renderer* renderer);
        ~TextureAgent();
        
        bool get_errors();
        void set_renderer(SDL_Renderer* new_renderer);

        // Helper functions //
        void add_texture(std::string name, SDL_Texture* texture);
        AgentTextureData get_texture_data(size_t index);
        SDL_Texture* get_texture(size_t index);
        std::string get_name(size_t index);
        size_t get_texture_count();
        bool get_texture_exists(const std::string& name);
        int get_index(const std::string& name);

        // Loading textures //
        int load_texture(const char* texture_path, std::string name = "none");;

        // Renderering textures //
        void render_texture(std::string texture_name, float x_pos, float y_pos, float size, int rotation = 0);
};

#endif