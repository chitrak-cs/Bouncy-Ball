// g++ main.c -Iinclude -Llib -o main.exe -lmingw32 -lSDL2main -lSDL2
// use this command to run this file

#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h>

#define WIDTH 900
#define HEIGHT 600
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define COLOR_GREY 0xf1f1f1f1
#define COLOR_BACKGROUND 0x0f0f0f0f
#define COLOR_TRAJECTORY 0xff763b
#define A_GRAVITY 0.2
#define DAMPING 0.85
#define TRAJECTORY_LENGTH 100
#define TRAJECTORY_WIDTH 5

struct Circle{
    double x;
    double y;
    double radius;
    double v_x;
    double v_y;
};

// function to render the circle in screen
void FillCircle(SDL_Surface *surface, struct Circle circle, Uint32 color){
    int low_x = circle.x - circle.radius;
    int high_x = circle.x + circle.radius;

    int low_y = circle.y - circle.radius;
    int high_y = circle.y + circle.radius;


    for(int x = low_x; x<= high_x; x++){
        for(int y = low_y; y<= high_y; y++){
            // is coordinate within the circle
            double centre_dist = sqrt((x-circle.x)*(x-circle.x) + (y-circle.y)*(y-circle.y));
            if(centre_dist <= circle.radius){
                SDL_Rect pixel = (SDL_Rect) {x,y,1,1};
                SDL_FillRect(surface, &pixel, color);
            }
        }
    }
}

void FillTrajectory(SDL_Surface *surface, struct Circle trajectory[TRAJECTORY_LENGTH], int current_trajectory_index, Uint32 color){
    for(int i=0;i<current_trajectory_index;i++){
        double trajectory_size = TRAJECTORY_WIDTH * (double) i/100;
        trajectory[i].radius = trajectory_size;
        FillCircle(surface, trajectory[i],COLOR_TRAJECTORY);
    }
}

void step(struct Circle* circle){
    // calculate the new position
    circle->x += circle->v_x;
    circle->y += circle->v_y;
    circle->v_y += A_GRAVITY;

    // reflection from the boundaries of the screen
    if(circle->x + circle->radius >= WIDTH){
        circle->x = WIDTH - circle->radius;
        circle->v_x = -circle->v_x * DAMPING;
    }
    if(circle->y + circle->radius >= HEIGHT){
        circle->y = HEIGHT - circle->radius;
        circle->v_y = -circle->v_y * DAMPING;
    }
    if(circle->x - circle->radius <= 0){
        circle->x = circle->radius;
        circle->v_x = -circle->v_x * DAMPING;
    }
    if(circle->y - circle->radius <= 0){
        circle->y = circle->radius;
        circle->v_y = -circle->v_y * DAMPING;
    }

}

void UpdateTrajectory(struct Circle trajectory[TRAJECTORY_LENGTH], struct Circle circle, int current_index){
    if(current_index >= TRAJECTORY_LENGTH){
        // shift the array
        struct Circle trajectory_shifted_copy[current_index];
        for(int i=0;i<current_index;i++){
            if(i>0) trajectory_shifted_copy[i] = trajectory[i+1];
        }
        for(int i=0;i<current_index;i++){
            trajectory[i] = trajectory_shifted_copy[i];
        }
        trajectory[current_index] = circle;
    }
    else{
        trajectory[current_index] = circle;
    }
}

int main(int argc, char* argv[]) {
    printf("Now it is time to start");
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Bouncy Ball", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT,SDL_WINDOW_BORDERLESS);
    SDL_Surface *surface = SDL_GetWindowSurface(window);

    // SDL_Rect rect = (SDL_Rect) {200,200,200,200};
    // SDL_FillRect(surface, &rect, 0xffffffff);
    
    // rendering the circle in the screen
    struct Circle circle = (struct Circle){200,200,100,5,0};
    struct Circle trajectory[TRAJECTORY_LENGTH];
    int current_trajectory_index = 0;

    SDL_Rect erase_rect = (SDL_Rect) {0,0,WIDTH,HEIGHT};

    SDL_Event event;
    int simulation_running = 1;
    while(simulation_running){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT) simulation_running = 0; // by quiting the function by clicking cross
            if(event.type == SDL_KEYDOWN){
                if(event.key.keysym.sym == SDLK_SPACE) simulation_running = 0;
            }
        }
        SDL_FillRect(surface, &erase_rect, COLOR_BACKGROUND);
        FillTrajectory(surface, trajectory, current_trajectory_index, COLOR_GREY);
        FillCircle(surface, circle, COLOR_WHITE);
        step(&circle);
        UpdateTrajectory(trajectory,circle,current_trajectory_index);
        if(current_trajectory_index < TRAJECTORY_LENGTH){
            current_trajectory_index++;
        }
        SDL_UpdateWindowSurface(window);
        SDL_Delay(20);
    }

    SDL_Delay(10);
}
