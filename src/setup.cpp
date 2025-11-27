#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>

#include "main.h"
#include "utils/logger.hpp"
#include "settings/locations.hpp"
#include "settings/info.h"
#include "settings/debug.hpp"
#include "settings/settings.hpp"

int setup() {
    // APP METADATA //
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, INFO_NAME.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, INFO_VERSION.toString().c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, INFO_DOMAIN.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, INFO_AUTHOR.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING, INFO_COPYRIGHT.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING, INFO_URL.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, "game");

    // SDL //
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        printf("\033[41;37mCRITICAL\033[0m: [setup.cpp:setup] SDL could not initialize: %s\n", SDL_GetError());
        return 1;
    } else {
        SDL_Log("INFO: SDL initialized successfully.");
    }

    // SETTINGS //
    LOGGER.log(LogLevel::INFO, "Loading Settings");
    init_locations_settings("data/config/locations.ini");
    init_debug_settings(LOCATIONS["config_dir"].get() + "/debug.ini");
    init_main_settings(LOCATIONS["config_dir"].get() + "/main.ini");

    // LOGGER //
    LOGGER.set_logfile(LOCATIONS["log_file"].get()); // Logger function has its own error handeling

    // GPU DEVICE //
    if (!SETTINGS_BOOL["old_renderring"].get()) {
        GPU = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL, false, SETTINGS_STRING["gpu_driver"].get().c_str());
        if (GPU == NULL) {
            LOGGER.log(LogLevel::ERROR, "[setup.cpp:setup] Could not create GPU device: %s", SDL_GetError());
            SETTINGS_BOOL["old_renderring"].set(true);
        }
    }

    if (SETTINGS_BOOL["old_renderring"].get()) {
        LOGGER.log(LogLevel::WARNING, "Using standard SDL renderring: less performance");
    }

    // WINDOW //
    std::string windowTitle = INFO_NAME + " - " + INFO_VERSION.toString();
    MAIN_WIN = SDL_CreateWindow(windowTitle.c_str(), SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    if ( MAIN_WIN == NULL ) {
        LOGGER.log(LogLevel::CRITICAL, "[setup.cpp:setup] Window could not be created: %s", SDL_GetError());
        return 1;
    }

    // CLAIM WINDOW //
    if (!SETTINGS_BOOL["old_renderring"].get()) {
        if (!SDL_ClaimWindowForGPUDevice(GPU, MAIN_WIN)) {
            LOGGER.log(LogLevel::CRITICAL, "[setup.cpp:setup] Could not claim Main Window for GPU: %s", SDL_GetError());
            return 1;
        }
    }

    // RENDERER //
    MAIN_REN = SDL_CreateRenderer(MAIN_WIN, NULL);
    std::string icon_path = std::string(LOCATIONS["resource_dir"].get()) + "/icons/icon.png";
    SDL_Surface* icon = IMG_Load(icon_path.c_str());
    if ( !SDL_SetWindowIcon(MAIN_WIN, icon) ) {
        LOGGER.log(LogLevel::WARNING, "[setup.cpp:setup] Windowicon could not be set: %s", SDL_GetError());
    }
    LOGGER.log(LogLevel::INFO, "SDL setup done!");

    // SETTINGS //
    if (DEBUG["test_logger"].get()) {
        LOGGER.log(LogLevel::DEBUG,    "Testing Logger");
        LOGGER.log(LogLevel::INFO,     "Testing Logger");
        LOGGER.log(LogLevel::WARNING,  "Testing Logger");
        LOGGER.log(LogLevel::ERROR,    "Testing Logger");
        LOGGER.log(LogLevel::CRITICAL, "Testing Logger");
    }
    return 0;
}

void cleanup(SDL_Renderer* renderer, SDL_Window* window) {
    LOGGER.log(LogLevel::INFO, "Cleaning up");
    SDL_DestroyGPUDevice(GPU);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}