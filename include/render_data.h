#ifndef RENDER_DATA_H
#define RENDER_DATA_H

#include <vector>
#include <SDL3_image/SDL_image.h>
#include <SDL3/SDL.h>
#include <stdio.h>
#include <string>
#include <algorithm>

#include "locations.h"

struct TextureData {
    std::string texture_name;
    SDL_Texture* texture;
};

// A class for managing loading and rendering of textures from files.
// Refer to ```load_texture``` and ```render_texture```.
class TextureManager {
    public:
        // Sets the renderer of the ```TextureManager``` class. Is required once before doing anything else with the ```TexturManager```.
        void set_renderer(SDL_Renderer* renderer_to_set_to) {
            renderer = renderer_to_set_to;
            missing_texture = load_texture_old(PATH_MISSING_TEXTURE_TILE.c_str());
        }

        // Checks if a texture with the name ```name``` exists in ```textures```
        bool textureExists(const std::string& name) {
            return std::find_if(textures.begin(), textures.end(), 
            [&name](const TextureData& textureData) {
                return textureData.texture_name == name;
            }) != textures.end();
        }

        // Finds the index of a texture form ```name```.
        // Outputs ```0``` (missing.png texture) if not found.
        int findTextureIndex(const std::string& name) {
            auto i = std::find_if(textures.begin(), textures.end(), 
                [&name](const TextureData& textureData) {
                    return textureData.texture_name == name;
                });

            if (i != textures.end()) {
                return std::distance(textures.begin(), i);
            }
            return 0;
        }

        // ```name``` must be an unique string. The texture will not be loaded if the name already exists in ```textures```
        // If loading two textures with the same ```name``` is absolutely needed, please refer to the ```load_texture_old()``` function.
        int load_texture(const char* picture_path, std::string name) {
            if (textureExists(name)) {
                return 2;
            } else {
                std::string picture_path_full = TEXTURE_DIR + picture_path;
                SDL_Texture* texture = IMG_LoadTexture(renderer, picture_path_full.c_str());
                if (!texture) {
                    printf("[load_texture] Failed to load texture: %s\n", SDL_GetError());
                    return 1;
                } else {
                    SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
                    add_texture(name, texture);
                    return 0;
                }
            }
        }

        void add_texture(std::string texture_name, SDL_Texture* texture) {
            TextureData data = {texture_name, texture};
            textures.push_back(data);
        }

        // Old version of load_texture
        SDL_Texture* load_texture_old(const char* picture_path) {
            std::string picture_path_full = TEXTURE_DIR + picture_path;
            SDL_Texture* texture = IMG_LoadTexture(renderer, picture_path_full.c_str());
            if (!texture) {
                printf("[load_texture_old] Failed to load texture: %s\n", SDL_GetError());
                return missing_texture;
            } else {
                SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
                return texture;
            }
        }

        SDL_Texture* get_texture(size_t index) {
            if (index < textures.size()) {
                return textures[index].texture;
            }
            return nullptr;
        }

        std::string get_name(size_t index) {
            if (index < textures.size()) {
                return textures[index].texture_name;
            }
            return nullptr;
        }

        size_t get_textureCount() const {
            return textures.size();
        }

        void render_texture(std::string texture_name, float x_pos, float y_pos, float size) {
            int texture_index = findTextureIndex(texture_name);
            if (texture_index == 0) {
                printf("[renderTexture] Something went wrong! Could not find Texture with name %s\n", texture_name.c_str());
            }
            TextureData data = textures[texture_index];
            if (data.texture) {
                SDL_FRect dst = { x_pos, y_pos, size, size };
                SDL_RenderTexture(renderer, data.texture, NULL, &dst);
            }
        }

        ~TextureManager() {
            // Clean up
            for (TextureData& data : textures) {
                SDL_DestroyTexture(data.texture);
            }
        }

    private:
        SDL_Renderer* renderer;
        SDL_Texture* missing_texture;
        std::vector<TextureData> textures = {{"missing", missing_texture}};
};

#endif