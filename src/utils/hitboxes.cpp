#include "utils/hitboxes.hpp"

// FOR HITBOX CLASS //
#include "player_info.h"

#include <SDL3/SDL.h>
#include "main.h"

//////////////
// HITBOXES //
//////////////

bool colliding(const Hitbox& a, const Hitbox& b) {
    float a_full_x = a.x + a.x_offset;
    float a_full_y = a.y + a.y_offset;
    float b_full_x = b.x + b.x_offset - 100;
    float b_full_y = b.y + b.y_offset - 100;
    // AABB
    return (a_full_x < b_full_x + b.width  &&
            a_full_x + a.width > b_full_x    &&
            a_full_y < b_full_y + b.height && 
            a_full_y + a.height > b_full_y
           );
}

void Hitbox::render(SDL_Renderer* renderer, const std::string& hex_colour, bool player) {
    if (hex_colour.size() != 7 || hex_colour[0] != '#') {
        LOGGER.log(LogLevel::ERROR, "Invalid hex colour '%s'", hex_colour.c_str());
        return;
    }

    uint8_t red =   std::stoi(hex_colour.substr(1, 2), nullptr, 16);
    uint8_t green = std::stoi(hex_colour.substr(3, 2), nullptr, 16);
    uint8_t blue =  std::stoi(hex_colour.substr(5, 2), nullptr, 16);

    SDL_SetRenderDrawColor(renderer, red, green, blue, 255);
    SDL_FRect hitbox_rect = {};
    if (player) {
        hitbox_rect = {(float)(RENDER_OFFSET_X + x_offset) * ZOOM, (float)(RENDER_OFFSET_Y + y_offset) * ZOOM, (float)width * ZOOM, (float)height * ZOOM}; // TODO: This shouldnt be needed!
    } else {
        hitbox_rect = {(float)(RENDER_OFFSET_X - CAMERA_POS.x + x_offset + x - 100) * ZOOM, (float)(RENDER_OFFSET_Y - CAMERA_POS.y + y_offset + y - 100) * ZOOM, (float)width * ZOOM, (float)height * ZOOM};
    }
    SDL_RenderRect(renderer, &hitbox_rect);
}

/////////////////////
// HITBOX HANDLING //
/////////////////////

// NODES //
void Node::render_hitboxes(SDL_Renderer* renderer, const std::string& hex_colour, bool player) {;
    if (hex_colour.size() != 7 || hex_colour[0] != '#') {
        LOGGER.log(LogLevel::ERROR, "Invalid hex colour '%s'", hex_colour.c_str());
        return;
    }

    for (Hitbox hitbox : hitboxes) {
        hitbox.render(renderer, hex_colour, player);
    }

    if (children[0] != nullptr) {
        for (int i = 0; i < 4; ++i) {
            children[i]->render_hitboxes(renderer, hex_colour, player);
        }
    }
}

void Node::render(const std::string& hex_colour) {
    if (hex_colour.size() != 7 || hex_colour[0] != '#') {
        LOGGER.log(LogLevel::ERROR, "Invalid hex colour '%s'", hex_colour.c_str());
        return;
    }

    uint8_t red =   std::stoi(hex_colour.substr(1, 2), nullptr, 16);
    uint8_t green = std::stoi(hex_colour.substr(3, 2), nullptr, 16);
    uint8_t blue =  std::stoi(hex_colour.substr(5, 2), nullptr, 16);

    SDL_SetRenderDrawColor(MAIN_REN, red, green, blue, 255);
    SDL_FRect rect = {(float)(min.x + RENDER_OFFSET_X - PLAYER->x) * ZOOM, // FIXME: why is this PLAYER->x, not CAMERA.x?
                      (float)(min.y + RENDER_OFFSET_Y - PLAYER->y) * ZOOM,
                      (float)((max.x - min.x)) * ZOOM,
                      (float)((max.y - min.y)) * ZOOM
                     };

    SDL_RenderRect(MAIN_REN, &rect);

    if (children[0] != nullptr) {
        for (int i = 0; i < 4; ++i) {
            children[i]->render(hex_colour);
        }
    }
}

HitboxQuadtree* MOVEBOXES = nullptr;

// INIT & CLEANUP //

HitboxQuadtree::HitboxQuadtree(XY min, XY max, int node_capacity) : node_capacity(node_capacity) {
    root = new Node(min, max);
}

HitboxQuadtree::~HitboxQuadtree() {
    // TODO: cleanup
}

// BUILDING TREE //

void HitboxQuadtree::insert(Hitbox hitbox) {
    add_entry(root, hitbox);
}

bool HitboxQuadtree::add_entry(Node* node, Hitbox hitbox) {
    if ((hitbox.x + (hitbox.width / 2)) < node->min.x || (hitbox.x + (hitbox.width / 2)) > node->max.x || (hitbox.y + (hitbox.height / 2)) < node->min.y || (hitbox.y + (hitbox.height / 2)) > node->max.y) {
        //LOGGER.log(LogLevel::DEBUG, "Rejecting hitbox: %d %d to %d %d (node %d %d to %d %d)", hitbox.x, hitbox.y, hitbox.x + hitbox.width, hitbox.y + hitbox.height,  node->min.x, node->min.y, node->max.x, node->max.y);
        return false;
    }
    if (node->children[0] != nullptr) {
        //LOGGER.log(LogLevel::DEBUG, "Passing hitbox: %d %d to %d %d (node %d %d to %d %d)", hitbox.x, hitbox.y, hitbox.x + hitbox.width, hitbox.y + hitbox.height,  node->min.x, node->min.y, node->max.x, node->max.y);
        bool taken = false;
        for (int i = 0; i < 4; ++i) {
            if (add_entry(node->children[i], hitbox)){
                taken = true;
                break;
            }
        }
        if (taken) {
            return true;
        } else {
            LOGGER.log(LogLevel::WARNING, "[hitboxes.cpp:add_entry] Could not pass hitbox to children: %d %d (node %d %d to %d %d)", hitbox.x, hitbox.y, node->min.x, node->min.y, node->max.x, node->max.y);
            return false;
        }
    } else {
        //LOGGER.log(LogLevel::DEBUG, "Accepting hitbox: %d %d to %d %d (node %d %d to %d %d)", hitbox.x, hitbox.y, hitbox.x + hitbox.width, hitbox.y + hitbox.height,  node->min.x, node->min.y, node->max.x, node->max.y);
        node->hitboxes.push_back(hitbox);

        int hitbox_length = node->hitboxes.size();
        if (hitbox_length > node_capacity) {
            if (DEBUG["all_debug_logs"]) LOGGER.log(LogLevel::DEBUG, "[hitboxes.cpp:add_entry] Subdividing node at %d %d and %d %d", node->min.x, node->min.y, node->max.x, node->max.y);
            subdivide(node);

            bool taken = false;
            for (Hitbox old_hitbox : node->hitboxes) {
                taken = false;
                for (int i = 0; i < 4; ++i) {
                    if (add_entry(node->children[i], old_hitbox)) {
                        taken = true;
                        break;
                    }
                }
                if (!taken) {
                    LOGGER.log(LogLevel::WARNING, "[hitboxes.cpp:add_entry] Could not pass hitbox to children: %d %d (node %d %d to %d %d)", hitbox.x, hitbox.y, node->min.x, node->min.y, node->max.x, node->max.y);
                }
            }
            node->hitboxes.clear();
        }
    }
    return true;
}

void HitboxQuadtree::subdivide(Node* node) {
    if (DEBUG["annoying_logs"]) LOGGER.log(LogLevel::DEBUG, "Splitting node: %d %d to %d %d", node->min.x, node->min.y, node->max.x, node->max.y);

    // Calculate midpoints //
    XY mid;
    mid.x = ceil((node->max.x - node->min.x) / 2) + node->min.x;
    mid.y = ceil((node->max.y - node->min.y) / 2) + node->min.y;

    if (DEBUG["annoying_logs"]) LOGGER.log(LogLevel::DEBUG, "Mid of node: %d %d", mid.x, mid.y);

    // Create children //
    if (DEBUG["annoying_logs"]) LOGGER.log(LogLevel::DEBUG, "New node: %d %d to %d %d", node->min.x, node->min.y, mid.x, mid.y);
    if (DEBUG["annoying_logs"]) LOGGER.log(LogLevel::DEBUG, "New node: %d %d to %d %d", mid.x, node->min.y, node->max.x, mid.y);
    if (DEBUG["annoying_logs"]) LOGGER.log(LogLevel::DEBUG, "New node: %d %d to %d %d", node->min.x, mid.y, mid.x, node->max.y);
    if (DEBUG["annoying_logs"]) LOGGER.log(LogLevel::DEBUG, "New node: %d %d to %d %d", mid.x, mid.y, node->max.x, node->max.y);

    node->children[0] = new Node({node->min.x, node->min.y}, {mid.x, mid.y});             // top-left
    node->children[1] = new Node({mid.x, node->min.y},       {node->max.x, mid.y});       // top-right
    node->children[2] = new Node({node->min.x, mid.y},       {mid.x, node->max.y});       // bottom-left
    node->children[3] = new Node({mid.x, mid.y},             {node->max.x, node->max.y}); // bottom-right
}

// RETURNING HITBOXES //

void HitboxQuadtree::query(Node* node, XY pos, int range, std::vector<Hitbox>& result) {
    if (node == nullptr) return;

    //LOGGER.log(LogLevel::DEBUG, "Max x,y node: %d %d | pos x,y: %d %d", node->max.x, node->max.y, pos.x, pos.y);
    if (node->max.x < (pos.x - range) || node->min.x > (pos.x + range) ||
        node->max.y < (pos.y - range) || node->min.y > (pos.y + range)) {
        return; // no overlap
    }

    for (const auto& hitbox : node->hitboxes) {
        result.push_back(hitbox);
    }

    for (int i = 0; i < 4; ++i) {
        query(node->children[i], pos, range, result);
    }
}

std::vector<Hitbox> HitboxQuadtree::get_relevant(XY pos, int range) {
    std::vector<Hitbox> result;
    query(root, pos, range, result);
    return result;
}