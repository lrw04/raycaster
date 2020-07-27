#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdio>
#include <cmath>
#include <ctime>

#include <SDL.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif

int SW, SH, BLK;
double FOV, WALK, TURN;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

std::vector<std::string> world;
int ww, wh;
double px, py, pa;

bool oor(double x, double y) { return x < 0 || x >= ww || y < 0 || y >= wh; }

double distance(double x0, double y0, double x1, double y1) {
    return pow(pow(x0 - x1, 2) + pow(y0 - y1, 2), 0.5);
}

void draw_column(int x, int siz, int col) {
    SDL_Rect tmp = {x, 0, 1, SH};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
    SDL_RenderDrawRect(renderer, &tmp);

    tmp = {x, SH / 2 - siz / 2, 1, siz};
    SDL_SetRenderDrawColor(renderer, col, col, col, 0xff);
    SDL_RenderDrawRect(renderer, &tmp);

    tmp = {x, SH / 2 + siz / 2, 1, SH};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0x3f, 0xff);
    SDL_RenderDrawRect(renderer, &tmp);
}

int bx(double x, double angle) { return cos(angle) < 0 ? (int) x - 1 : (int) x; }

int by(double y, double angle) { return sin(angle) < 0 ? (int) y - 1 : (int) y; }

void render() {
    for (int col = 0; col < SW; col++) {
        double theta = pa + atan((SW / 2 - col) / (SW / (2 * tan(2 * FOV))))/*FOV / 2 - FOV / SCREEN_WIDTH * col*/;
        double dist = INFINITY;

        double x0, y0, dx, dy;

        // horizontal intersections
        if (sin(theta)) { // can intersect
            y0 = sin(theta) < 0 ? floor(py) : ceil(py);
            x0 = px + (y0 - py) / tan(theta);
            dy = sin(theta) < 0 ? -1 : 1;
            dx = dy / tan(theta);
            while (!oor(floor(x0), by(y0, theta))) {
                if (world[by(y0, theta)][(int) floor(x0)] == '#') {
                    dist = std::min(dist, distance(px, py, x0, y0));
                    break;
                }
                x0 += dx;
                y0 += dy;
            }
        }

        // vertical intersections
        if (cos(theta)) {
            x0 = cos(theta) < 0 ? floor(px) : ceil(px);
            y0 = py + (x0 - px) * tan(theta);
            dx = cos(theta) < 0 ? -1 : 1;
            dy = dx * tan(theta);
            while (!oor(bx(x0, theta), floor(y0))) {
                if (world[(int) floor(y0)][bx(x0, theta)] == '#') {
                    dist = std::min(dist, distance(px, py, x0, y0));
                    break;
                }
                x0 += dx;
                y0 += dy;
            }
        }

        draw_column(col, (int) floor(BLK / (dist * cos(theta - pa))), (int) floor(0xff / (dist + 4) * 4));
    }
}

void drawRectangle(int x, int y, int sx, int sy, int r, int g, int b) {
    SDL_Rect tmp = {x, y, sx, sy};
    SDL_SetRenderDrawColor(renderer, r, g, b, 0xff);
    SDL_RenderDrawRect(renderer, &tmp);
}

void drawLine(int x0, int y0, int x1, int y1, int r, int g, int b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 0xff);
    SDL_RenderDrawLine(renderer, x0, y0, x1, y1);
}

void walk(long t) {
    px += t * WALK * cos(pa) / CLOCKS_PER_SEC;
    py += t * WALK * sin(pa) / CLOCKS_PER_SEC;
}

void turn(long t) { pa += t * TURN / CLOCKS_PER_SEC; }

void side(long t) { // pa + pi/2
    px -= t * WALK * sin(pa) / CLOCKS_PER_SEC;
    py += t * WALK * cos(pa) / CLOCKS_PER_SEC;
}

void load(char* wn) {
    world.clear();
    std::ifstream wst(wn);
    std::string s;
    wst >> pa >> FOV >> WALK >> TURN;
    pa *= M_PI / 180;
    FOV *= M_PI / 180;
    BLK = SW / (2 * tan(FOV / 2));
    while (wst >> s) world.push_back(s);
    wst.close();

    ww = world[0].length();
    wh = world.size();

    std::reverse(world.begin(), world.end()); // Fix coordinates

    for (int i = 0; i < wh; i++)
        for (int j = 0; j < ww; j++)
            if (world[i][j] == 'P') { px = j + 0.5; py = i + 0.5; }
    pa = 0;
}

void config(char* cfg) {
    std::ifstream cst(cfg);
    cst >> SW >> SH;
    cst.close();
    BLK = SW / (2 * tan(FOV / 2));
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s WORLD CONFIG\n", argv[0]);
        return -1;
    }
    load(argv[1]);
    config(argv[2]);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    window = SDL_CreateWindow("Naive Raycaster", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SW, SH, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Cannot create renderer: %s\n", SDL_GetError());
        return -1;
    }

    long start = clock(), prev = clock();
    int frames = 0;

    while (true) {
        long cur = clock();
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) goto quit;
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case 'q': goto quit;
                    case 'r': load(argv[1]); break;
                    default: break;
                }
            }
        }
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_W]) walk(cur - prev);
        if (keys[SDL_SCANCODE_S]) walk(prev - cur);
        if (keys[SDL_SCANCODE_A]) side(cur - prev);
        if (keys[SDL_SCANCODE_D]) side(prev - cur);
        if (keys[SDL_SCANCODE_LEFT]) turn(cur - prev);
        if (keys[SDL_SCANCODE_RIGHT]) turn(prev - cur);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xff);
        SDL_RenderClear(renderer);

        render();

        SDL_RenderPresent(renderer);

        frames++;
        if (cur - start) printf("%d FPS, (%.2lf, %.2lf), %.2lf\r", frames * CLOCKS_PER_SEC / (cur - start), px, py, pa);
        prev = cur;
        if (clock() - start > CLOCKS_PER_SEC) {
            frames = 0;
            start = clock();
        }
    }

quit:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}