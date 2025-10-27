#include "texture_agent.hpp"
#include <iostream>

/* Manages the Textures in the Game:
       - Loading textures
       - Rendering Textures
       - Managing Spritesheets
     Usage:
       - construct the Agent; pass in the renderer: ```TextureAgent my_agent(myrenderer)```
       - check for any errors by running ```my_angent.checkerrors()```. this will return ```true``` if there aren't any errors
*/

//////////////////////
// INIT AND CLEANUP //
//////////////////////

TextureAgent::TextureAgent(SDL_Renderer* renderer) {
    if (renderer != nullptr) {
        set_renderer(renderer);
    } else {
        LOGGER.log(LogLevel::WARNING, "[render_agent.cpp:TextureAgent] Given renderer is invalid!");
    }
    LOGGER.log(LogLevel::INFO, "[texture_agent.cpp:TextureAgent] Initailized TextureAgent.");
}

/* Checks for any errors after the constructor
    Returns ```True``` if errors are found
*/
bool TextureAgent::get_errors() {
    if (renderer == nullptr) {
        LOGGER.log(LogLevel::INFO, "[texture_agent.cpp:get_errors] Could not load the renderer.");
        return true;
    }
    return false;
}

/*
    Sets the renderer of TextureAgent to the ```new_renderer```
*/
void TextureAgent::set_renderer(SDL_Renderer* new_renderer) {
    renderer = new_renderer;
}

TextureAgent::~TextureAgent() {
    for (AgentTextureData& data : agent_textures) {
        SDL_DestroyTexture(data.texture);
    }
}

//////////////////////
// HELPER FUNCTIONS //
//////////////////////

/*
    Adds a texture to ```agent_textures```
*/
void TextureAgent::add_texture(std::string name, SDL_Texture* texture) {
    AgentTextureData texture_data = {name, texture};
    agent_textures.push_back(texture_data);
}

/*
    Returns the full data of the texture of ```index``` in ```angent_textures```
*/
AgentTextureData TextureAgent::get_texture_data(size_t index) {
    if (index < agent_textures.size()) {
        return agent_textures[index];
    }
    return agent_textures[0];
}

/*
    Returns the SDL_Texture of the texture of ```index``` in ```angent_textures```
*/
SDL_Texture* TextureAgent::get_texture(size_t index) {
    if (index < agent_textures.size()) {
        return agent_textures[index].texture;
    }
    return nullptr;
}

/*
    Returns the name of the texture of ```index``` in ```angent_textures```
*/
std::string TextureAgent::get_name(size_t index) {
    if (index < agent_textures.size()) {
        return agent_textures[index].name;
    }
    return nullptr;
}

/*
    Returns the amount of textures in ```agent_textures```
*/
size_t TextureAgent::get_texture_count() {
    return agent_textures.size();
}

/*
    Checks if a texture of the name ```name``` exists in ```agent_textures```
*/
bool TextureAgent::get_texture_exists(const std::string& name) {
    for (AgentTextureData texture_data : agent_textures) {
        if (texture_data.name == name) {
                return true;
        }
    }
    return false;
}

/* Finds the index of a texture from ```name```.
    Outputs ```-1``` if not found.
*/
int TextureAgent::get_index(const std::string& name) {
    if (get_texture_exists(name)) {
        int i = 0;
        for (AgentTextureData texture_data : agent_textures) {
                if (texture_data.name == name) {
                    return i;
                }
                i++;
        }
    }
    return -1;
}

//////////////////////
// LOADING TEXTURES //
//////////////////////

int TextureAgent::load_texture(const char* texture_path, std::string name) {
    // Check for existing textures //
    if (name != "none") {
        if (get_texture_exists(name)) {
                if (DEBUG_ALL_DEBUG_LOGS) {
                    LOGGER.log(LogLevel::DEBUG, "[texture_agent.cpp:load_texture] Caught texture from loading twice!");
                }
                return 2;
        }
    }
    // Load Texture //
    std::string texture_path_full = texture_path;
    SDL_Texture* texture = IMG_LoadTexture(renderer, texture_path_full.c_str());
    if (!texture) {
        LOGGER.log(LogLevel::WARNING, "[texture_agent.cpp:load_texture] Failed to load texture: %s", SDL_GetError());
        return 1;
    } else {
        SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
        add_texture(name, texture);
        return 0;
    }
};

////////////////////////
// RENDERING TEXTURES //
////////////////////////

void TextureAgent::render_texture(std::string texture_name, float x_pos, float y_pos, float size, int rotation) {
    int texture_index = get_index(texture_name);
    if (texture_index == -1) {
        LOGGER.log(LogLevel::WARNING,"[texture_agent.cpp:render_texture] Could not find Texture with name '%s'", texture_name.c_str());
    }
    AgentTextureData data = agent_textures[texture_index];
    if (data.texture) {
        SDL_FRect destination_rect = { x_pos, y_pos, size, size };
        SDL_RenderTextureRotated(renderer, data.texture, NULL, &destination_rect, rotation, NULL, SDL_FLIP_NONE);
    }
}