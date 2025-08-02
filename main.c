#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <pthread.h>

#define true 1
#define false 0
#define gotoxy(x,y)     printf("\033[%d;%dH", y, x);
#define hide_cursor()     printf("\033[?25l");
#define HEIGHT 24
#define WIDTH 80
#define NUMBER_OF_WALLS 5
#define NANO_SEC_DELAY 100000000

#define str0    "  __ _                           _     _         _    ____ _     ___ "
#define str1    " / _| | __ _ _ __  _ __  _   _  | |__ (_)_ __ __| |  / ___| |   |_ _|"
#define str2    "| |_| |/ _` | '_ \\| '_ \\| | | | | '_ \\| | '__/ _` | | |   | |    | |"
#define str3    "|  _| | (_| | |_) | |_) | |_| | | |_) | | | | (_| | | |___| |___ | |" 
#define str4    "|_| |_|\\__,_| .__/| .__/ \\__, | |_.__/|_|_|  \\__,_|  \\____|_____|___|"
#define str5    "            |_|   |_|    |___/"                                       

struct Point {
    int x_coord, y_coord;
    char symbol;
} typedef point;

struct termios old, new;
struct timespec delay;

pthread_t render_thread, input_thread;
point player;
point walls[NUMBER_OF_WALLS][HEIGHT];

int SCORE;
int GAME;
const int PLAYER_X_COORD = WIDTH / 10;
const int PLAYER_Y_COORD = HEIGHT / 2;

void render();
void start_input();
void *input(void*);
void tick();
void create_wall(point[]);
void collision();
void start_menu();
void game_loop();
void init();
void logo();
void clearwalls();

int main(void){

    srand(time(NULL));
    hide_cursor();

    while (true){
        init();
        start_menu();
        start_input();
        game_loop();
    }
    return 0;
}

void init(){
    delay.tv_sec = 0;
    delay.tv_nsec = NANO_SEC_DELAY;
    
    player.x_coord = PLAYER_X_COORD;
    player.y_coord = PLAYER_Y_COORD;
    player.symbol = '*';
    
    clearwalls();
    for (int i = 0; i < NUMBER_OF_WALLS; i++){
        create_wall(walls[i]);
    }
}

void game_loop(){
    SCORE = 0;
    pthread_create(&input_thread, NULL, input, NULL);
    while (GAME){
        render();
        tick();
        collision();
        delay.tv_nsec -= (SCORE * 5000);
        nanosleep(&delay, NULL);
    }
}

void tick(){
    player.y_coord++;
    int offset = (rand() % 40);
    for (int i = 0; i < NUMBER_OF_WALLS; i++){
        if (walls[i][0].x_coord <= 0){
            create_wall(walls[i]);
            for (int j = 0; j < HEIGHT; j++) walls[i][j].x_coord = (WIDTH + offset);
        } else{
            for (int j = 0; j < HEIGHT; j++)    walls[i][j].x_coord--;
        }
    }
}

void *input(void *arg){
    char input;
    while (GAME){
        tcgetattr(0, &old);
        tcgetattr(0, &new);
        new.c_lflag &= ~ICANON;
        tcsetattr(0, TCSANOW, &new);
        read(0, &input, 1);
        tcsetattr(0, TCSANOW, &old);
        if (input == 32) {
            if (GAME){
                if (player.y_coord >= 8){
                    player.y_coord-=7;
                } else {
                    player.y_coord -= (player.y_coord - 1);
                }
            } 
            
        }
        
    }
}

void start_input(){
    char input;
    while (true){
        tcgetattr(0, &old);
        tcgetattr(0, &new);
        new.c_lflag &= ~ICANON;
        tcsetattr(0, TCSANOW, &new);
        read(0, &input, 1);
        tcsetattr(0, TCSANOW, &old);
        GAME = false;
        if (input == 32) {
            if (!GAME){
                GAME = true;
                break;   
            }
        }
    }
}

void render(){
    system("clear");
    gotoxy(player.x_coord, player.y_coord);
    printf("%c\n", player.symbol);

    for (int i = 0; i < NUMBER_OF_WALLS; i++){
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

    gotoxy(0,0);
    printf("SCORE: %d\n", SCORE);

}

void collision(){
    if (player.y_coord > HEIGHT) {
        system("clear");
        GAME = false;
    }
    for (int i = 0; i < NUMBER_OF_WALLS; i++){
        if (walls[i][0].x_coord == PLAYER_X_COORD){
            if (walls[i][player.y_coord].symbol == '=') { 
                system("clear");
                GAME = false;
                break;
            }
            else SCORE++;
        } 

    }
}

void create_wall(point wall[]){

    int gap = (rand() % (HEIGHT / 2)) + 2;
    int x_coord = (rand() % WIDTH) + 25;
   
    for (int i = 0; i < gap; i++){
        wall[i].x_coord = x_coord;
        wall[i].y_coord = i;
        wall[i].symbol = '=';
    } for (int i = gap; i < gap + 9; i++){
        wall[i].symbol = ' ';
        wall[i].x_coord = x_coord;
        wall[i].y_coord = i;
    } for (int i = gap + 10; i < HEIGHT; i++){
        wall[i].symbol = '=';
        wall[i].x_coord = x_coord;
        wall[i].y_coord = i;
    }
}

void start_menu(){
    system("clear");
    render();
    logo();
    gotoxy(25, 9);
    printf("Press SPACE to start.\n");
}

void logo(){
    printf("%s\n",str0);
    printf("%s\n",str1);
    printf("%s\n",str2);
    printf("%s\n",str3);
    printf("%s\n",str4);
    printf("%s\n",str5);
}

void clearwalls(){
    for (int i = 0; i < NUMBER_OF_WALLS; i++){
        for (int j = 0; j < HEIGHT; j++){
            walls[i][j].x_coord = 0;
            walls[i][j].y_coord = 0;
            walls[i][j].symbol = ' ';
        }
    }
}