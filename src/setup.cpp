#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>

#include "main.h"
#include "logger.hpp"
#include "locations.hpp"
#include "info.h"
#include "debug.h"

int setup() {
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, INFO_NAME.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, INFO_VERSION.toString().c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, INFO_DOMAIN.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, INFO_AUTHOR.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING, INFO_COPYRIGHT.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING, INFO_URL.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, "game");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
        printf("\033[41;37mCRITICAL\033[0m: [setup.cpp:setup] SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    } else {
        SDL_Log("INFO: SDL initialized successfully.");
    }
    LOGGER.set_logfile(PATH_LOG_FILE); // Logger function has its own error handeling

    std::string windowTitle = INFO_NAME + " - " + INFO_VERSION.toString();
    MAIN_WIN = SDL_CreateWindow(windowTitle.c_str(), SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    if ( MAIN_WIN == NULL ) {
        LOGGER.log(LogLevel::CRITICAL, "[setup.cpp:setup] Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return 1;
    } else {
        MAIN_REN = SDL_CreateRenderer(MAIN_WIN, NULL);
        std::string icon_path = std::string(RESOURCE_DIR) + "/icons/icon.png";
        SDL_Surface* icon = IMG_Load(icon_path.c_str());
        if ( !SDL_SetWindowIcon(MAIN_WIN, icon) ) {
            LOGGER.log(LogLevel::WARNING, "[setup.cpp:setup] Windowicon could not be set: %s\n", SDL_GetError());
        }
        LOGGER.log(LogLevel::INFO, "Setup call done");

        if ( DEBUG_TEST_LOGGER ) {
            LOGGER.log(LogLevel::DEBUG,    "Testing Logger");
            LOGGER.log(LogLevel::INFO,     "Testing Logger");
            LOGGER.log(LogLevel::WARNING,  "Testing Logger");
            LOGGER.log(LogLevel::ERROR,    "Testing Logger");
            LOGGER.log(LogLevel::CRITICAL, "Testing Logger");
        }
        return 0;
    }
}

void cleanup(SDL_Renderer* renderer, SDL_Window* window) {
    LOGGER.log(LogLevel::INFO, "Cleaning up");
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}