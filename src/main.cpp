#define SDL_MAIN_USE_CALLBACKS

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_image/SDL_image.h>

#include "main.hpp"
#include "utils/logger.hpp"

#include "settings/info.h"
#include "settings/locations.hpp"
#include "settings/debug.hpp"
#include "settings/main.hpp"


// ██╗████████╗███████╗██████╗  █████╗ ████████╗███████╗
// ██║╚══██╔══╝██╔════╝██╔══██╗██╔══██╗╚══██╔══╝██╔════╝
// ██║   ██║   █████╗  ██████╔╝███████║   ██║   █████╗  
// ██║   ██║   ██╔══╝  ██╔══██╗██╔══██║   ██║   ██╔══╝  
// ██║   ██║   ███████╗██║  ██║██║  ██║   ██║   ███████╗
// ╚═╝   ╚═╝   ╚══════╝╚═╝  ╚═╝╚═╝  ╚═╝   ╚═╝   ╚══════╝
SDL_AppResult SDL_AppIterate(void* appState) {

    SDL_GPUCommandBuffer* cmd_buffer;
    cmd_buffer = SDL_AcquireGPUCommandBuffer(GPU);
    if (cmd_buffer == NULL) {
        SDL_Log("SDL_AcquireGPUCommandBuffer failed: %s",
            SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_GPUTexture* swapchain_texture;
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(cmd_buffer, WINDOW, &swapchain_texture, NULL, NULL)) {
        SDL_Log("SDL_WaitAndAcquireGPUSwapchainTexture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (swapchain_texture == NULL) {
        LOG(LogLevel::CRITICAL, "Aquiering Swapchain Texture Failed: %s", SDL_GetError());
    }

    SDL_GPUColorTargetInfo target_info = {};
        target_info.texture = swapchain_texture;
        target_info.cycle = true;
        target_info.load_op = SDL_GPU_LOADOP_CLEAR;
        target_info.store_op = SDL_GPU_STOREOP_STORE;
        target_info.clear_color = {0.1f, 0.1f, 0.1f, 1.0f};

    SDL_GPURenderPass* render_pass;
    render_pass = SDL_BeginGPURenderPass(cmd_buffer, &target_info, 1, NULL);
    SDL_EndGPURenderPass(render_pass);

    SDL_SubmitGPUCommandBuffer(cmd_buffer);

    return SDL_APP_CONTINUE;
}


// ███████╗██╗   ██╗███████╗███╗   ██╗████████╗
// ██╔════╝██║   ██║██╔════╝████╗  ██║╚══██╔══╝
// █████╗  ██║   ██║█████╗  ██╔██╗ ██║   ██║   
// ██╔══╝  ╚██╗ ██╔╝██╔══╝  ██║╚██╗██║   ██║   
// ███████╗ ╚████╔╝ ███████╗██║ ╚████║   ██║   
// ╚══════╝  ╚═══╝  ╚══════╝╚═╝  ╚═══╝   ╚═╝   
SDL_AppResult SDL_AppEvent(void* appState, SDL_Event* event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    return SDL_APP_CONTINUE;
}


// ██╗███╗   ██╗██╗████████╗
// ██║████╗  ██║██║╚══██╔══╝
// ██║██╔██╗ ██║██║   ██║   
// ██║██║╚██╗██║██║   ██║   
// ██║██║ ╚████║██║   ██║   
// ╚═╝╚═╝  ╚═══╝╚═╝   ╚═╝   
SDL_AppResult SDL_AppInit(void** appState, int argc, char** argv) {
    // APP METADATA //
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_NAME_STRING, INFO_NAME.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_VERSION_STRING, INFO_VERSION.toString().c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_IDENTIFIER_STRING, INFO_DOMAIN.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_CREATOR_STRING, INFO_AUTHOR.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_COPYRIGHT_STRING, INFO_COPYRIGHT.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_URL_STRING, INFO_URL.c_str());
    SDL_SetAppMetadataProperty(SDL_PROP_APP_METADATA_TYPE_STRING, "game");

    // SDL //
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        LOG(LogLevel::CRITICAL, "SDL could not initialize: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    } else {
        LOG(LogLevel::INFO, "SDL initialized successfully.");
    }

    // SETTINGS //
    LOG(LogLevel::INFO, "Loading Settings");
    init_locations_settings("data/config/locations.ini"); /* Not really ideal, right? */
    init_debug_settings(std::string(LOCATIONS["config_dir"]) + "/debug.ini");
    init_main_settings(std::string(LOCATIONS["config_dir"]) + "/main.ini");

    // LOGGER //
    LOGGER.set_logfile(LOCATIONS["log_file"]); // Logger function has its own error handeling

    // GPU DEVICE //
    if (!SETTINGS["software_renderring"]) {
        GPU = SDL_CreateGPUDevice(SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL, false, std::string(SETTINGS["gpu_driver"]).c_str());
        if (GPU == NULL) {
            LOG(LogLevel::ERROR, "Could not create GPU device: %s", SDL_GetError());
            SETTINGS["software_renderring"].set(true);
        } else {
            LOG(LogLevel::INFO, "Using GPU driver %s", SDL_GetGPUDeviceDriver(GPU));
        }
    }

    if (SETTINGS["software_renderring"]) {
        LOG(LogLevel::WARNING, "Using software renderring");
    }

    // WINDOW //
    std::string windowTitle = INFO_NAME + " - " + INFO_VERSION.toString();
    WINDOW = SDL_CreateWindow(windowTitle.c_str(), SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (WINDOW == NULL) {
        LOG(LogLevel::CRITICAL, "Window could not be created: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    // CLAIM WINDOW //
    if (!SETTINGS["software_renderring"]) {
        if (!SDL_ClaimWindowForGPUDevice(GPU, WINDOW)) {
            LOG(LogLevel::ERROR, "Could not claim Main Window for GPU: %s", SDL_GetError());
            SETTINGS["software_renderring"].set(true);
            LOG(LogLevel::WARNING, "Using software renderring");
            // GPU device cleanup
            SDL_DestroyGPUDevice(GPU);
        }
    }

    if (SETTINGS["software_renderring"]) {
        // RENDERER //
        RENDERER = SDL_CreateRenderer(WINDOW, NULL);
        if (RENDERER == NULL) {
            LOG(LogLevel::CRITICAL, "Renderrer could not be created: %s", SDL_GetError());
        }

        // VSYNC //
        if (!SDL_SetRenderVSync(RENDERER, 1)) {
            LOG(LogLevel::ERROR, "VSync could not be enabled: %s", SDL_GetError());
        }
    }


    std::string icon_path = std::string(LOCATIONS["resource_dir"]) + "/icons/icon.png";
    SDL_Surface* icon = IMG_Load(icon_path.c_str());
    if ( !SDL_SetWindowIcon(WINDOW, icon) ) {
        LOG(LogLevel::WARNING, "Windowicon could not be set: %s", SDL_GetError());
    }
    LOG(LogLevel::INFO, "SDL setup done!");

    // TESTS //
    if (DEBUG["test_logger"]) {
        LOG(LogLevel::DEBUG,    "Testing Logger");
        LOG(LogLevel::INFO,     "Testing Logger");
        LOG(LogLevel::WARNING,  "Testing Logger");
        LOG(LogLevel::ERROR,    "Testing Logger");
        LOG(LogLevel::CRITICAL, "Testing Logger");
    }
    return SDL_APP_CONTINUE;
}


 // ██████╗ ██╗   ██╗██╗████████╗
// ██╔═══██╗██║   ██║██║╚══██╔══╝
// ██║   ██║██║   ██║██║   ██║   
// ██║▄▄ ██║██║   ██║██║   ██║   
// ╚██████╔╝╚██████╔╝██║   ██║   
//  ╚══▀▀═╝  ╚═════╝ ╚═╝   ╚═╝   
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    LOG(LogLevel::INFO, "Cleaning up");
    //SDL_DestroyGPUDevice(GPU);
    //SDL_DestroyRenderer(RENDERER);
    //SDL_DestroyWindow(WINDOW);
    if (GPU != NULL) {
        if (WINDOW != NULL) {
            SDL_ReleaseWindowFromGPUDevice(GPU, WINDOW);
            SDL_DestroyWindow(WINDOW);
        }

        SDL_DestroyGPUDevice(GPU);
    }

    SDL_Quit();
}