#include "mapitems.h"
#include <stdlib.h>
#include <time.h>

using my3d::Vertex;
using my3d::Triangle;
using my3d::Transformation;
using my3d::Position;
using my3d::Y;
using namespace mapitems;

Map::Map(int map_size, float block_dist, float block_size, Color block_color, int steps_per_block)
    : map_size(map_size), block_dist(block_dist), block_size(block_size), block_color(block_color),
    steps_per_block(steps_per_block)
{
    step_length = block_dist / (float)steps_per_block;
    for (int i = 0; i < map_size; i++) {
        for (int j = 0; j < map_size; j++) {
            squares.push_back(get_square(i, j));
        }
    }
}

void Map::show(Context & context)
{
    std::vector<Mesh>::iterator it;
    for (it = squares.begin(); it != squares.end(); ++it) {
        context.draw_mesh(*it);
    }
}

Mesh Map::get_square(int ii, int ij)
{
    Mesh out;
    out.transformation = Transformation().translate(Position(ij, 0, ii) * block_dist);
    std::vector<Vertex> vertexes;
    std::vector<Triangle> triangles;
    for (int i = -1; i <= 1; i += 2) {
        for (int j = -1; j <= 1; j += 2) {
            Vertex v;
            v.color = block_color;
            v.position = Position(j, 0, i) / 2 * block_size;
            vertexes.emplace_back(v);
        }
    }
    triangles.emplace_back(0, 1, 2);
    triangles.emplace_back(1, 2, 3);

    out.triangles = triangles;
    out.vertexes = vertexes;
    out.accept_light = true;

    return out;
}

Snake::Snake(tuple<int> head, float snake_width, Map map, direction dir, int length)
    : head_offset(0.0), score(0), steps_per_block(map.steps_per_block),
    step_len(map.step_length), map_size(map.map_size), snake_width(snake_width)
{
    next_dir = dir;
    turn_info a = { head, dir };
    snake.emplace_back(a);
    switch (dir) {
    case direction::i_p:
        for (int i = 0; i < length - 1; ++i) {
            a.block.i--;
            snake.emplace_back(a);
        }
        break;
    case direction::i_n:
        for (int i = 0; i < length - 1; ++i) {
            a.block.i++;
            snake.emplace_back(a);
        }
        break;
    case direction::j_p:
        for (int i = 0; i < length - 1; ++i) {
            a.block.j--;
            snake.emplace_back(a);
        }
        break;
    case direction::j_n:
        for (int i = 0; i < length - 1; ++i) {
            a.block.j++;
            snake.emplace_back(a);
        }
        break;
    }
}

bool Snake::move()
{
    bool collide = false;
    head_offset += step_len;
    float block_dist = steps_per_block * step_len;
    if (head_offset >= block_dist) {
        head_offset -= block_dist;
        snake.pop_back();
        turn_info new_head = snake[0];
        new_head.block = apply_dir_delta_on_block(new_head.block, new_head.dir);
        for (std::vector<turn_info>::iterator it = snake.begin() + 1; it != snake.end(); ++it) {
            if (new_head.block.i == it->block.i && new_head.block.j == it->block.j) {
                collide = true;
                break;
            }
        }
        if (new_head.dir != next_dir) {
            new_head.dir = next_dir;
        }
        snake.insert(snake.begin(), new_head);
    }
    return collide;
}

void Snake::turn(direction dir)
{
    next_dir = dir;
}

void Snake::show(Context & context)
{
    float block_dist = steps_per_block * step_len;
    /* Draw head */
    context.draw_mesh(get_snake_head(snake[0]));
    /* Draw the offset of the head */
    context.draw_mesh(get_snake_body(snake[0].block, snake[0].dir, head_offset));
    int segment_len = 1;
    for (std::vector<turn_info>::iterator it = snake.begin() + 1; it != snake.end() - 1; ++it) {
        /* Draw body */
        context.draw_mesh(get_snake_body(it->block, it->dir, block_dist));
    }
    /* Draw tail */
    tuple<int> block = apply_dir_delta_on_block(snake.back().block, snake.back().dir);
    context.draw_mesh(get_snake_body(block, dir_reverse(snake.back().dir), block_dist - head_offset + snake_width));


}

Mesh Snake::get_snake_head(turn_info head)
{
    std::vector<Vertex> vertexes;
    std::vector<Triangle> triangles;

    Vertex v;

    v.color = { 255, 0, 0 };

    v.position = { 0, 0, 0 };
    vertexes.emplace_back(v);

    v.position = { 0.866025, 0.000000 ,-0.500000 };
    vertexes.emplace_back(v);

    v.position = { -0.000000 ,0.000000, 1.000000 };
    vertexes.emplace_back(v);

    v.position = { -0.866025 ,0.000000, -0.500000 };
    vertexes.emplace_back(v);

    v.position = { 0.866025 ,0.500000, -0.500000 };
    vertexes.emplace_back(v);

    v.position = { -0.000000 ,0.500000 ,1.000000 };
    vertexes.emplace_back(v);

    v.position = { -0.866025, 0.500000, -0.500000 };
    vertexes.emplace_back(v);


    triangles.emplace_back(1, 2, 3);
    triangles.emplace_back(2, 1, 4);
    triangles.emplace_back(2, 4, 5);
    triangles.emplace_back(1, 3, 6);
    triangles.emplace_back(1, 6, 4);
    triangles.emplace_back(3, 2, 5);
    triangles.emplace_back(3, 5, 6);
    triangles.emplace_back(5, 4, 6);
    Mesh out(vertexes, triangles);
    float block_dist = steps_per_block * step_len;
    tuple<float> pos = { head.block.i * block_dist , head.block.j * block_dist };
    pos = apply_dir_delta_on_pos(pos, head.dir, head_offset);
    out.transformation = Transformation().scale(0.5)
        .rotate(Y, float(head.dir) * M_PI / 2)
        .translate(pos.j, 0, pos.i);
    out.accept_light = true;
    return out;
}

Mesh Snake::get_snake_body(tuple<int> block, direction dir, float length)
{
    std::vector<Vertex> vs;
    Vertex v;
    v.color = { 255, 0, 0 };
    vs.emplace_back(v);
    float w2 = snake_width / 2;
    v.position = { -w2,0.000000, length }; vs.emplace_back(v);
    v.position = { -w2,0.000000,0.000000 }; vs.emplace_back(v);
    v.position = { w2,0.000000,0.000000 }; vs.emplace_back(v);
    v.position = { w2,0.000000, length }; vs.emplace_back(v);
    v.position = { -w2,0.250000, length }; vs.emplace_back(v);
    v.position = { -w2,0.250000,0.000000 }; vs.emplace_back(v);
    v.position = { w2,0.250000, length }; vs.emplace_back(v);
    v.position = { w2,0.250000,0.000000 }; vs.emplace_back(v);

    std::vector<Triangle> ts;
    ts.emplace_back(1, 2, 3);
    ts.emplace_back(1, 3, 4);
    ts.emplace_back(2, 1, 5);
    ts.emplace_back(2, 5, 6);
    ts.emplace_back(1, 4, 7);
    ts.emplace_back(1, 7, 5);
    ts.emplace_back(4, 3, 8);
    ts.emplace_back(4, 8, 7);
    ts.emplace_back(3, 2, 6);
    ts.emplace_back(3, 6, 8);
    ts.emplace_back(6, 5, 7);
    ts.emplace_back(6, 7, 8);

    Mesh out;
    out.accept_light = true;
    out.triangles = ts;
    out.vertexes = vs;

    float block_dist = steps_per_block * step_len;
    tuple<float> pos = { block.i * block_dist, block.j * block_dist };
    float rot = dir_rotation(dir);
    out.transformation = Transformation().translate(0, 0, -snake_width / 2).rotate(Y, rot).translate(pos.j, 0, pos.i);
    return out;
}

direction Snake::dir_reverse(direction dir)
{
    return direction((int(dir) + 2) % 4);
}

float Snake::dir_rotation(direction dir)
{
    return float(dir) * M_PI / 2;
}

tuple<int> Snake::apply_dir_delta_on_block(tuple<int> block, direction dir)
{
    switch (dir) {
    case direction::i_p:
        if (block.i == map_size - 1) {
            block.i = 0;
        }
        else {
            block.i += 1;
        }
        break;
    case direction::i_n:
        if (block.i == 0) {
            block.i = map_size - 1;
        }
        else {
            block.i -= 1;
        }
        break;
    case direction::j_p:
        if (block.j == map_size - 1) {
            block.j = 0;
        }
        else {
            block.j += 1;
        }
        break;
    case direction::j_n:
        if (block.j == 0) {
            block.j = map_size - 1;
        }
        else {
            block.j -= 1;
        }
        break;
    }
    return block;
}

tuple<float> Snake::apply_dir_delta_on_pos(tuple<float> pos, direction dir, float delta)
{
    switch (dir) {
    case direction::i_p:
        pos.i += delta; break;
    case direction::i_n:
        pos.i -= delta; break;
    case direction::j_p:
        pos.j += delta; break;
    case direction::j_n:
        pos.j -= delta; break;
    }
    return pos;
}

bool Snake::eat_food(Food food)
{
    bool eaten = false;
    if (snake[0].block.i == food.coord.i && snake[0].block.j == food.coord.j) {
        eaten = true;
        snake.emplace(snake.end(), snake.back());
        ++score;
    }
    return eaten;
}

tuple<float> Snake::get_head_pos(void)
{
    float block_dist = steps_per_block * step_len;
    tuple<float> pos = { (float)snake[0].block.i * block_dist, (float)snake[0].block.j * block_dist };
    pos = apply_dir_delta_on_pos(pos, snake[0].dir, head_offset);

    return pos;
}

direction Snake::get_head_dir(void)
{
    return snake[0].dir;
}

Food::Food(Map map, Snake snake) : block_dist(map.block_dist)
{
    /* Generate a new food according to the size of the map */
    /* Avoid coord collide with snake */
    update_coord(map, snake);

    /* Generate a mesh and store it */
    std::vector<Vertex> vs;
    Vertex v;
    v.color = { 0 , 0 , 255 };
    vs.emplace_back(v);
    float w2 = map.block_size * 0.8 / 2;
    v.position = { -w2,0.150000, w2 }; vs.emplace_back(v);
    v.position = { -w2,0.150000,-w2 }; vs.emplace_back(v);
    v.position = { w2,0.150000,w2 }; vs.emplace_back(v);
    v.position = { w2,0.150000, -w2 }; vs.emplace_back(v);
    v.position = { 0.000000,2.000000, 0.000000 }; vs.emplace_back(v);

    std::vector<Triangle> ts;
    ts.emplace_back(1, 2, 3);
    ts.emplace_back(2, 3, 4);
    ts.emplace_back(1, 2, 5);
    ts.emplace_back(1, 3, 5);
    ts.emplace_back(3, 4, 5);
    ts.emplace_back(2, 4, 5);

    out.accept_light = true;
    out.triangles = ts;
    out.vertexes = vs;
    out.transformation = Transformation().translate(coord.j * block_dist, 0, coord.i * block_dist);
}

void Food::show(Context & context)
{
    /* Show the mesh */
    context.draw_mesh(out);
}

void Food::rotate(void)
{
    rotate_count = (rotate_count + 1) % FOOD_ROT_PERIOD;
    out.transformation = Transformation().rotate(Y, rotate_count * M_PI / FOOD_ROT_PERIOD)
        .translate(coord.j * block_dist, 0, coord.i * block_dist);
}

void Food::update_coord(Map map, Snake snake)
{
    srand(time(NULL));
    bool valid = true;
    while (1) {
        coord.i = rand() % map.map_size;
        coord.j = rand() % map.map_size;
        valid = true;
        for (turn_info segment : snake.snake) {
            if (segment.block.i == coord.i && segment.block.j == coord.j) {
                valid = false;
                break;
            }
        }
        if (valid) break;
    }
}

tuple<int> Food::get_coord(void)
{
    return coord;
}

