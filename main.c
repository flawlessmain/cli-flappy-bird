#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>

#define true 1
#define gotoxy(x,y)     printf("\033[%d;%dH", y, x);
#define hide_cursor()     printf("\033[?25l");
#define HEIGHT 24
#define WIDTH 80

struct Point {
    int x_coord, y_coord;
    char symbol;
} typedef point;

int SCORE = 0;

void render(point*, point[][HEIGHT]);
void input(point*);
void tick(point*, point[][HEIGHT]);
void collisions(point*, point[]);
void create_wall(point[]);

struct termios old, new;

int main(void){

    struct timespec delay;
    point player;
    point walls[3][HEIGHT];
    pthread_t render_thread, input_thread;
    
    delay.tv_sec = 0;
    delay.tv_nsec = 100000000;

    player.x_coord = WIDTH / 10;
    player.y_coord = HEIGHT / 2;
    player.symbol = '*';

    srand(time(NULL));

    for (int i = 0; i < 3; i++){
        create_wall(walls[i]);
    }
        
    hide_cursor();

    pthread_create(&input_thread, NULL, input, &player);


    while (true){
        render(&player, walls);
        tick(&player, walls);
        collisions(&player, walls);
        nanosleep(&delay, NULL);
    }
    return 0;
}

void tick(point *player, point walls[][HEIGHT]){
    if (player->y_coord < HEIGHT - 1)    player->y_coord++;
    int offset = (rand() % (WIDTH / 2));
    for (int i = 0; i < 3; i++){
        for (int j = 0; j < HEIGHT; j++){
            if (walls[i][j].x_coord > 0){
                walls[i][j].x_coord--;
            } else {
                walls[i][j].x_coord += WIDTH + offset;
            }
        } 
    }
}

void input(point *player){
    char input;
    while (true){
        tcgetattr(0, &old);
        tcgetattr(0, &new);
        new.c_lflag &= ~ICANON;
        tcsetattr(0, TCSANOW, &new);
        read(0, &input, 1);
        tcsetattr(0, TCSANOW, &old);
        if (input == 32) {
            if (player->y_coord >= 7)    player->y_coord-=7;
        }
    }
}

void render(point *player, point walls[][HEIGHT]){
    system("clear");
    
    gotoxy(0,0);
    printf("SCORE: %d", SCORE);

    gotoxy(player->x_coord, player->y_coord);
    printf("%c\n", player->symbol);

    for (int i = 0; i < 3; i++){
        for (int j = 0; j < HEIGHT; j++){
            if ((walls[i][j].x_coord < WIDTH) && (walls[i][j].x_coord > 0)){
                gotoxy(walls[i][j].x_coord, walls[i][j].y_coord);
                printf("%c\n", walls[i][j].symbol);        
            }
        }
    }
    
    for (int i = 0; i < WIDTH; i++){
        gotoxy(i,HEIGHT - 1);
        printf("=\n");
    }
}

void collisions(point *player, point wall[]){
    
}

void create_wall(point wall[]){

    int gap = (rand() % (HEIGHT / 2)) + 2;
    int x_coord = (rand() % WIDTH) + 25;
   
    for (int i = 0; i < gap; i++){
        wall[i].x_coord = x_coord;
        wall[i].y_coord = i;
        wall[i].symbol = '=';
    } for (int i = gap; i < gap + 10; i++){
        wall[i].symbol = ' ';
        wall[i].x_coord = x_coord;
        wall[i].y_coord = i;
    } for (int i = gap + 10; i < HEIGHT; i++){
        wall[i].symbol = '=';
        wall[i].x_coord = x_coord;
        wall[i].y_coord = i;
    }
}