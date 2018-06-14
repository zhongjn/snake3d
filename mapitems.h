#pragma once
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include "my3d/include/my3d.h"

using my3d::Context;
using my3d::Mesh;
using my3d::Color;

namespace mapitems {
#define FOOD_ROT_PERIOD 120

    template<typename T>
    struct tuple {
        T i, j;
    };

    enum direction {
        i_p = 0, // i正方向
        j_p = 1, // j正方向
        i_n = 2,
        j_n = 3
    };

    struct turn_info {
        tuple<int> block;
        direction dir;
    };

    class Map
    {
        friend class Snake;
        friend class Food;
    public:
        Map(int map_size = 40, float block_dist = 1.4f, float block_size = 1.0f,
            Color block_color = Color(120, 120, 120), int steps_per_block = 20);
        void show(Context& context);

    private:
        int map_size;
        float block_dist;
        float block_size;
        Color block_color;
        int steps_per_block;
        float step_length;
        std::vector<Mesh> squares;
        Mesh get_square(int ii, int ij);
    };

    class Snake
    {
        friend class Food;
    public:
        Snake(tuple<int> head, float snake_width, Map map, direction dir = i_p, int length = 5);
        bool move(void);
        void turn(direction dir);
        void show(Context& context);
        bool eat_food(Food food);
        tuple<float> get_head_pos(void);
        direction get_head_dir(void);
        int score;

    private:    
        std::vector<turn_info> snake;
        direction next_dir;
        float head_offset;
        int steps_per_block;
        float step_len;
        int map_size;
        float snake_width;
        Mesh get_snake_head(turn_info head);
        Mesh get_snake_body(tuple<int> block, direction dir, float length);
        direction dir_reverse(direction dir);
        float dir_rotation(direction dir);
        tuple<int> apply_dir_delta_on_block(tuple<int> block, direction dir);
        tuple<float> apply_dir_delta_on_pos(tuple<float> pos, direction dir, float delta);
    };

    class Food
    {
        friend class Snake;
    public:
        Food(Map map, Snake snake);
        void show(Context& context);
        void rotate(void);
        void update_coord(Map map, Snake snake);
        tuple<int> get_coord(void);

    private:
        Mesh out;
        tuple<int> coord;
        float block_dist;
        int rotate_count;
    };
}