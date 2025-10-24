#ifndef RENDER_AGENT_HPP
#define RENDER_AGENT_HPP

#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL.h>
#include <string>
#include <algorithm>

#include "main.h"
#include "logger.hpp"
#include "locations.h"

/*
   Holds the Data of a ```SDL_Texture``` of ```TextureAgent```
*/
struct AgentTextureData {
     std::string name;
     SDL_Texture* texture;
};

/* Manages the Textures in the Game:
       - Loading textures
       - Rendering Textures
       - Managing Spritesheets
     Usage:
       - construct the Agent; pass in the renderer: ```TextureAgent my_agent(myrenderer)```
       - check for any errors by running ```my_angent.checkerrors()```. this will return ```true``` if there aren't any errors
*/
class TextureAgent {
     private:
          SDL_Renderer* renderer = nullptr;
          std::vector<AgentTextureData> agent_textures = {};
          
     public:
          //////////////////////
          // INIT AND CLEANUP //
          //////////////////////

          TextureAgent(/*SDL_Renderer* renderer*/) {
               /*if (renderer != nullptr) {
                    set_renderer(renderer);
               }*/
               load_texture(PATH_MISSING_TEXTURE_TILE.c_str(), "missing");
               LOGGER.log(LogLevel::INFO, "[render_agent.hpp:TextureAgent] Initailized TextureAgent.");
          }

          /* Checks for any errors after the constructor
             Returns ```True``` if errors are found
          */
          bool get_errors() {
               if (renderer == nullptr) {
                    return true;
                    LOGGER.log(LogLevel::INFO, "[render_agent.hpp:get_errors] Could not load the renderer.");
               }
          }

          /*
             Sets the renderer of TextureAgent to the ```new_renderer```
          */
          void set_renderer(SDL_Renderer* new_renderer) {
               renderer = new_renderer;
          }

          ~TextureAgent() {
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
          void add_texture(std::string name, SDL_Texture* texture) {
               AgentTextureData texture_data = {name, texture};
               agent_textures.push_back(texture_data);
          }

          /*
             Returns the full data of the texture of ```index``` in ```angent_textures```
          */
          AgentTextureData get_texture_data(size_t index) {
               if ((index < agent_textures.size()) and (index > 0)) {
                    return agent_textures[index];
               }
          }

          /*
             Returns the SDL_Texture of the texture of ```index``` in ```angent_textures```
          */
          SDL_Texture* get_texture(size_t index) {
               if ((index < agent_textures.size()) and (index > 0)) {
                    return agent_textures[index].texture;
               }
               return nullptr;
          }

          /*
             Returns the name of the texture of ```index``` in ```angent_textures```
          */
          std::string get_name(size_t index) {
               if ((index < agent_textures.size()) and (index > 0)) {
                    return agent_textures[index].name;
               }
               return nullptr;
          }

          /*
             Returns the amount of textures in ```agent_textures```
          */
          size_t get_texture_count() {
               return agent_textures.size();
          }

          /*
             Checks if a texture of the name ```name``` exists in ```agent_textures```
          */
          bool get_texture_exists(const std::string& name) {
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
          int get_index(const std::string& name) {
               if (get_texture_exists(name)) {
                    int i = 0;
                    for (AgentTextureData texture_data : agent_textures) {
                         if (texture_data.name == name) {
                              return i;
                         }
                         i++;
                    }
               } else {
                    return -1;
               }
          }

          //////////////////////
          // LOADING TEXTURES //
          //////////////////////

          int load_texture(const char* texture_path, std::string name = "none") {
               // Check for existing textures //
               if (name != "none") {
                    if (get_texture_exists(name)) {
                         return 2;
                    }
               }
               // Load Texture //
               std::string texture_path_full = TEXTURE_DIR + texture_path;
               SDL_Texture* texture = IMG_LoadTexture(renderer, texture_path_full.c_str());
               if (!texture) {
                    LOGGER.log(LogLevel::WARNING, "[render_agent.hpp:load_texture] Failed to load texture: %s", SDL_GetError());
                    return 1;
               } else {
                    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
                    add_texture(name, texture);
                    return 0;
               }
          };

          /////////////////////
          // RENDER TEXTURES //
          /////////////////////

          void render_texture(std::string texture_name, float x_pos, float y_pos, float size) {
               int texture_index = get_index(texture_name);
               if (texture_index == -1) {
                    LOGGER.log(LogLevel::WARNING,"[render_agent.hpp:render_texture] Could not find Texture with name '%s'", texture_name.c_str());
               }
               AgentTextureData data = agent_textures[texture_index];
               if (data.texture) {
                    SDL_FRect dst = { x_pos, y_pos, size, size };
                    SDL_RenderTexture(renderer, data.texture, NULL, &dst);
               }
          }
};

#endif