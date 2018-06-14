#include <windows.h>
#include <stdio.h>
#include "my3d/include/my3d.h"
#include "my3d/include/my3dpresent.h"
#include "mapitems.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <chrono>
#include <conio.h>
#include <list>

using namespace my3d;
using namespace mapitems;
const int width = 320, height = 240;

const float cam_height = 7.2f;
const float cam_pitch = 0.65f;
const float cam_back = 8.0f;

const int map_size = 20;

const float block_dist = 1.4f;
const float block_size = 1.0f;
const Color block_color = Color(120, 120, 120);

const int steps_per_block = 20;

const tuple<int> start_block = { map_size / 2, map_size / 2 };

int main(int argc, char **argv) {
    printf("Press any key to start...");
    getchar();
    system("cls");


    Map map = Map(map_size, block_dist, block_size, block_color, steps_per_block);
    Snake snake = Snake(start_block, 0.6 * block_size, map, i_p, 5);
    Food food = Food(map, snake);

    float dr = 0;
    int turn = 0;
    direction snake_dir = snake.get_head_dir();

    PointLight pl;
    pl.position = { 0, 3, 0 };
    pl.intensity = 3;
    pl.color = { 255, 255, 255 };

    AmbientLight al;
    al.color = { 255, 255, 255 };
    al.intensity = 0.03;


    Context context(width, height);

    present::initialize(GetConsoleWindow(), width, height, 2);

    Camera camera;
    camera.lens_size = { (float)width / (float)height, 1.0 };

    int frames = 0;
    auto prev = std::chrono::high_resolution_clock::now();



    while (true) {

        tuple<float> cur_pos = snake.get_head_pos();
        float smooth_r = dr + snake_dir * M_PI / 2;

#pragma region FPS
        frames++;
        if (frames == 100) {

            auto now = std::chrono::high_resolution_clock::now();
            auto d = std::chrono::duration_cast<std::chrono::milliseconds>(now - prev).count();
            prev = now;
            frames = 0;
            char title[50] = { 0 };
            sprintf_s(title, "SnakeDemo FPS=%.2f. Your score : %d", (float)(100.0 / (d / 1000.0)), snake.score);
            SetConsoleTitle(title);
        }
#pragma endregion


        camera.transformation = Transformation().rotate(X, cam_pitch)
            .translate(0, 0, -cam_back).rotate(Y, smooth_r).translate(0, cam_height, 0);

#pragma region scene
        context.scene_begin(camera);

        context.set_ambient_light(al);
        context.set_point_light(pl);

        context.set_world_transformation(Transformation().translate(-cur_pos.j, 0, -cur_pos.i));
        map.show(context);
        food.rotate();
        food.show(context);
        snake.show(context);

        context.set_world_transformation(Transformation());


        context.scene_end();
#pragma endregion

        present::set_all_pixels(context);
        present::present();

        /* Change direction */
        if (_kbhit()) {
            int ch = _getch();
            if (ch == 224) {
                int ch2 = _getch();
                if (ch2 == 75 || ch2 == 77) {
                    // -1=right 1=left 
                    if (ch2 == 75) {
                        turn = 1;
                    }
                    else {
                        turn = -1;
                    }
                    snake.turn(direction((int(snake.get_head_dir()) + turn + 4) % 4));
                }
            }
        }

        if (snake.move()) {
            /* Game over if the snake collide with its body */
            break;
        }
        if (snake.eat_food(food)) {
            food.update_coord(map, snake);
        }
        if (snake_dir != snake.get_head_dir()) {
            snake_dir = snake.get_head_dir();
            dr = -turn * M_PI / 2;
        }
        dr *= 0.8;
    }
    printf("Your score: %d\n", snake.score);
    printf("Press any key to exit.\n");
    getchar();
    return EXIT_SUCCESS;
}
