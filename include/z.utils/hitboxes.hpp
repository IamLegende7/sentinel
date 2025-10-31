#ifndef HITBOXES_HPP
#define HITBOXES_HPP

// FOR HITBOX CLASS //
#include "main.h"

// FOR QUADTREE //
#include <vector>
#include <cmath>

#include "helper_utils.h"
#include "logger.hpp"

//////////////
// HITBOXES //
//////////////

class Hitbox {
    public:
        int type; // 0: none; 1: collides with walls & stuff; 2: takes damage from attacks
        int x, y; // Stuck to the units x & y coords
        int x_offset, y_offset;
        int width, height;

        bool colliding(const Hitbox& B);
        void render(SDL_Renderer* renderer, const std::string& hex_colour, bool player = false);
};

/////////////////////
// HITBOX HANDLING //
/////////////////////

struct Node {
    XY min;
    XY max;
    std::vector<Hitbox> hitboxes;
    Node* children[4] = {nullptr, nullptr, nullptr, nullptr};

    Node(XY min, XY max) : min(min), max(max) {}
    void render_hitboxes(SDL_Renderer* renderer, const std::string& hex_colour, bool player = false);
    void render(const std::string& hex_colour);
};

inline std::vector<Node> RENDER_NODES;

//inline std::vector<std::vector<Hitbox>> MOVEBOXES_TILES;         // initially collects all Moveboxes in the game

class HitboxQuadtree {
    private:
        int node_capacity;

        bool add_entry(Node* node, Hitbox hitbox);
        void subdivide(Node* node);

        void query(Node* node, XY pos, int range, std::vector<Hitbox>& result);

    public:
        Node* root = nullptr;

        // Init & cleanup //
        HitboxQuadtree(XY min, XY max, int node_capacity = 8);
        ~HitboxQuadtree();

        // Building tree //
        void insert(Hitbox hitbox);

        // Returning Hitboxes //
        std::vector<Hitbox> get_relevant(XY pos, int range = 150);
};

#endif

extern HitboxQuadtree* MOVEBOXES;