#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define GAME_TERMINATE -1
#define ALPHA 0.2
#define GAMMA 0.6
#define EPS 0.85
#define TRAIN_MODE 0
#define SPEED 10.0

// ALLEGRO Variables
ALLEGRO_DISPLAY* display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_BITMAP *image = NULL;
ALLEGRO_BITMAP *image2 = NULL;
ALLEGRO_BITMAP *image3 = NULL;
ALLEGRO_BITMAP *background = NULL;
ALLEGRO_BITMAP *bullet1 = NULL;
ALLEGRO_BITMAP *bullet2 = NULL;
ALLEGRO_KEYBOARD_STATE keyState ;
ALLEGRO_TIMER *timer = NULL;
ALLEGRO_TIMER *timer2 = NULL;
ALLEGRO_TIMER *timer3 = NULL;
ALLEGRO_SAMPLE *song=NULL;
ALLEGRO_FONT *font = NULL;
ALLEGRO_FONT *title_font = NULL;
FILE * sa_form_r = NULL;
FILE * sa_form_w = NULL;




//Custom Definition
const char *title = "Final Project 107060005";
const float FPS = 60;
const int WIDTH = 406;
const int HEIGHT = 568;
typedef struct character
{
    int x;
    int y;
    ALLEGRO_BITMAP *image_path;

}Character;

Character character1;
Character character2;
Character character3;

int imageWidth = 0;
int imageHeight = 0;
int draw = 0;
int done = 0;
int window = 1;
int end_game = 0;
int bullet1_place[8] = {-1,-1,-1,-1,-1,-1,-1,-1};
int bullet2_place[8] = {-1,-1,-1,-1,-1,-1,-1,-1};
float sa_form_array[7][8][7][4] = {0};
int status_next[4] = {3,7,3,0};
int status_now[4] = {3,7,3,0};
int status_trainer_next[4] = {3,7,3,0};
int status_trainer_now[4] = {3,7,3,0};
int reward = 0;
int killed_player = 0;
int game_round = 0;
bool judge_next_window = false;
bool ture = true; //true: appear, false: disappear
bool next = false; //true: trigger
bool dir = true; //true: left, false: right
int steps = 0;
int state_not_equal_zero = 0;
int state_not_equal_zero_after_exe = 0;
int player1_life = 10;
int player2_life = 10;
int go_win3 = 0;
int go_win1 = 0;

void show_err_msg(int msg);
void game_init();
void game_begin();
int process_event();
int game_run();
void game_destroy();
void sa_form_setup(void);
void sa_form_flash(int *,int *);
void sa_form_save(void);
float  sa_form_max_act(int *);
int  sa_form_act(int *);
void sa_train_mode(void);
void sa_game_mode(void);
void move_bullet(void);
int random_zero_to_three(void);
int random_zero_to_two(void);
void do_act(int ,int );
void flash_train_mode_status(void);
void flash_game_mode_status(void);
void clear(void);
int main(int argc, char *argv[]) {
    int msg = 0;
    sa_form_r = fopen("sa_form.txt","r");

    printf("preparing...\n");
    sa_form_setup();
    printf("sa_form set up success!!\n");
    printf("state_not_equal_zero = %d\n",state_not_equal_zero);
    game_init();
    game_begin();

    while (msg != GAME_TERMINATE) {
        msg = game_run();
        if (msg == GAME_TERMINATE){
            printf("Game Over\n");
        }
        else if(end_game == 1){
            //if(game_round %2000 == 0){
            //   printf("game_round = %d\n",game_round);
            //}
            clear();
            //if(game_round == 20000){
            //    break;
            //}
            window = 1;
        }
    }
    printf("sa_form saving...\n");
    sa_form_w = fopen("sa_form.txt","w");
    sa_form_save();
    printf("sa_form save success!!\n");
    printf("state_not_equal_zero_after_exe = %d\n",state_not_equal_zero_after_exe);
    game_destroy();
    fclose(sa_form_r);
    fclose(sa_form_w);
    system("pause");
    return 0;
}

void show_err_msg(int msg) {
    fprintf(stderr, "unexpected msg: %d\n", msg);
    game_destroy();
    exit(9);
}

void game_init() {
    if (!al_init()) {
        show_err_msg(-1);
    }
    if(!al_install_audio()){
        fprintf(stderr, "failed to initialize audio!\n");
        show_err_msg(-1);
    }
    if(!al_init_acodec_addon()){
        fprintf(stderr, "failed to initialize audio codecs!\n");
        show_err_msg(-1);
    }
    if (!al_reserve_samples(1)){
        fprintf(stderr, "failed to reserve samples!\n");
        show_err_msg(-1);
    }
    // Create display
    display = al_create_display(WIDTH, HEIGHT);
    event_queue = al_create_event_queue();
    if (display == NULL || event_queue == NULL) {
        show_err_msg(-1);
    }
    // Initialize Allegro settings
    al_set_window_position(display, 0, 0);
    al_set_window_title(display, title);
    al_init_primitives_addon();
    al_install_keyboard();
    al_install_audio();
    al_init_image_addon();
    al_init_acodec_addon();
    al_init_font_addon();
    al_init_ttf_addon();

    // Register event
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    srand((unsigned)time(NULL));
}

void game_begin() {
    // Load sound
    song = al_load_sample( "hello.wav" );
    if (!song){
        printf( "Audio clip sample not loaded!\n" );
        show_err_msg(-1);
    }
    // Loop the song until the display closes
    al_play_sample(song, 1.0, 0.0,1.0,ALLEGRO_PLAYMODE_LOOP,NULL);
    al_clear_to_color(al_map_rgb(0,0,0));
    // Load and draw text
    font = al_load_ttf_font("pirulen.ttf",12,0);
    title_font = al_load_ttf_font("pirulen.ttf",17,0);
    al_draw_text(font, al_map_rgb(150,150,150), WIDTH/2, HEIGHT/2+220 , ALLEGRO_ALIGN_CENTRE, "Press 'q' to quit");
    al_draw_rectangle(WIDTH/2-150, 500, WIDTH/2+150, 540, al_map_rgb(255, 255, 255), 0);
    al_draw_text(font, al_map_rgb(150,150,150), WIDTH/2, HEIGHT/2+170 , ALLEGRO_ALIGN_CENTRE, "Press 'a' to about");
    al_draw_rectangle(WIDTH/2-150, 450, WIDTH/2+150, 490, al_map_rgb(255, 255, 255), 0);
    al_draw_text(font, al_map_rgb(150,150,150), WIDTH/2, HEIGHT/2+120 , ALLEGRO_ALIGN_CENTRE, "Press 'Enter' to start");
    al_draw_rectangle(WIDTH/2-150, 400, WIDTH/2+150, 440, al_map_rgb(255, 255, 255), 0);
    al_draw_text(title_font, al_map_rgb(150,150,150), WIDTH/2, 50 , ALLEGRO_ALIGN_CENTRE, "Can you survive to win??");
    al_draw_bitmap(al_load_bitmap("airplane.png"),25,100,0);
    al_flip_display();
}

int process_event(){
    // Request the event

    //int rand_var = rand()%10;
    ALLEGRO_EVENT event;
    al_wait_for_event(event_queue, &event);

    // Our setting for controlling animation
    if(event.timer.source == timer){
        //printf("process\n");
        move_bullet();
        if(TRAIN_MODE){
            sa_train_mode();
        }
        else{
            sa_game_mode();
        }
    }
    // Keyboard
    if(event.type == ALLEGRO_EVENT_KEY_UP)
    {
        switch(event.keyboard.keycode)
        {            // Control
            case ALLEGRO_KEY_LEFT:
                //printf("hello\n");
                if(character1.x >=58){
                    character1.x -= 58;
                }
                break;
            case ALLEGRO_KEY_RIGHT:
                if(character1.x <348){
                    character1.x += 58;
                }
                break;
            case ALLEGRO_KEY_SPACE:
                bullet1_place[1] = character1.x / 58;
                //printf("shoot\n");
                break;
            // For Start Menu
            case ALLEGRO_KEY_ENTER:
                judge_next_window = true;
                break;
            case ALLEGRO_KEY_A:
                go_win3 = true;
                break;
        }
    }
    if(event.type == ALLEGRO_EVENT_KEY_UP && window == 1){
        switch(event.keyboard.keycode){
            case ALLEGRO_KEY_A:
                go_win3 = true;
                go_win1 = false;
                return 0;
                break;
            case ALLEGRO_KEY_Q:
                return GAME_TERMINATE;
            default :
                printf("key error\n");
        }
    }
    if(event.type == ALLEGRO_EVENT_KEY_UP && window == 3){
        switch(event.keyboard.keycode){
            case ALLEGRO_KEY_Q:
                game_begin();
                window = 1;
                go_win3 = false;
                return 0;
                break;
            default:
                printf("key error\n");
        }
    }
    if(event.type == ALLEGRO_EVENT_KEY_UP && window == 4){
        switch(event.keyboard.keycode){
            case ALLEGRO_KEY_Q:
                return GAME_TERMINATE;
                break;
            case ALLEGRO_KEY_ENTER:
                window = 2;
                break;
            case ALLEGRO_KEY_A:
                go_win3 = false;
                game_begin();
                window = 1;
                break;
            default:
                printf("key error\n");
        }
    }
    // Shutdown our program
    else if(event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
        return GAME_TERMINATE;

    return 0;
}

int game_run() {
    int error = 0;
    // First window(Menu)
    if(window == 1){
        if (!al_is_event_queue_empty(event_queue)) {
            error = process_event();
        }
        if(go_win3){
            window = 3;
        }
        else if(judge_next_window) {
            judge_next_window = false;
            window = 2;
            // Setting Character
            character1.x = 174;
            character1.y = 497;
            character2.x = 174;
            character2.y = 0;
            character1.image_path = al_load_bitmap("player1.png");
            character2.image_path = al_load_bitmap("player2.png");
            bullet1 = al_load_bitmap("bullet1.png");
            bullet2 = al_load_bitmap("bullet2.png");
            background = al_load_bitmap("gray.png");

            //Initialize Timer
            timer  = al_create_timer(1.0 / SPEED);
            al_register_event_source(event_queue, al_get_timer_event_source(timer)) ;
            al_start_timer(timer);
        }
    }
    // Second window(Main Game)
    if(window == 2){
        // Change Image for animation
        int i;
        al_draw_bitmap(background, 0,0, 0);
        al_draw_bitmap(character1.image_path, character1.x, character1.y, 0);
        al_draw_bitmap(character2.image_path, character2.x, character2.y, 0);
        al_draw_rectangle(character1.x, character1.y, character1.x + 6 * player1_life, character1.y +10,al_map_rgb(200, 0, 0), 2);
        al_draw_rectangle(character2.x, character2.y, character2.x + 6 * player2_life, character2.y +10,al_map_rgb(0, 0, 200), 2);
        //al_clear_to_color(al_map_rgb(0,0,0));
        for(i=1;i<=7;i++){
            if(bullet1_place[i] != -1){
                al_draw_bitmap(bullet1, bullet1_place[i] * 58, 497 - i * 71 , 0);
            }
            if(bullet2_place[i] != -1){
                al_draw_bitmap(bullet2, bullet2_place[i] *58, 71+i*71 , 0);
            }
        }
        // Listening for new event
        if (!al_is_event_queue_empty(event_queue)) {
            error = process_event();
        }
        al_flip_display();
    }
    if(window == 3){
        al_clear_to_color(al_map_rgb(0,0,0));
        al_draw_text(title_font, al_map_rgb(150,150,150), WIDTH/2, 50 , ALLEGRO_ALIGN_CENTRE, "Just a EZ game");
        al_draw_text(font, al_map_rgb(150,150,150), WIDTH/2, HEIGHT/2+220 , ALLEGRO_ALIGN_CENTRE, "Press 'q' to quit");
        al_flip_display();
        if (!al_is_event_queue_empty(event_queue)) {
            error = process_event();
        }
    }
    if(window == 4){
        if(player1_life == 0){
            al_draw_text(title_font, al_map_rgb(150,150,150), WIDTH/2, 50 , ALLEGRO_ALIGN_CENTRE, "YOU DEAD");
        }
        else{
            al_draw_text(title_font, al_map_rgb(150,150,150), WIDTH/2, 50 , ALLEGRO_ALIGN_CENTRE, "YOU SURVIVE??");
        }
        al_draw_text(font, al_map_rgb(150,150,150), WIDTH/2, HEIGHT/2+220 , ALLEGRO_ALIGN_CENTRE, "Press 'q' to quit");
        al_draw_rectangle(WIDTH/2-150, 500, WIDTH/2+150, 540, al_map_rgb(255, 255, 255), 0);
        al_draw_text(font, al_map_rgb(150,150,150), WIDTH/2, HEIGHT/2+170 , ALLEGRO_ALIGN_CENTRE, "Press 'a' to menu");
        al_draw_rectangle(WIDTH/2-150, 450, WIDTH/2+150, 490, al_map_rgb(255, 255, 255), 0);
        al_draw_text(font, al_map_rgb(150,150,150), WIDTH/2, HEIGHT/2+120 , ALLEGRO_ALIGN_CENTRE, "Press 'Enter' to start");
        al_draw_rectangle(WIDTH/2-150, 400, WIDTH/2+150, 440, al_map_rgb(255, 255, 255), 0);
        al_flip_display();
        if (!al_is_event_queue_empty(event_queue)) {
            error = process_event();
        }
    }
    return error;
}

void game_destroy() {
    // Make sure you destroy all things
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_timer(timer2);
    al_destroy_bitmap(image);
    al_destroy_sample(song);
}
void move_bullet(void){
    //printf("move_bullet\n");
    int i;
    for(i=7;i>=1;i--){
        bullet1_place[i] = bullet1_place[i-1];
        bullet2_place[i] = bullet2_place[i-1];
    }
    if(bullet1_place[7]*58 == character2.x){
        //printf("player win the game\n");
        end_game = 1;
        killed_player = 2;
    }

    else if(bullet2_place[7]*58 == character1.x){
        //printf("AI win the game\n");
        end_game = 1;
        killed_player = 1;
    }
}
void sa_form_setup(void){
    int a,b,c;
    for(a=0;a<=6;a++){
        for(b=0;b<=7;b++){
            for(c=0;c<=6;c++){
                fscanf(sa_form_r,"%f %f %f %f",
                &sa_form_array[a][b][c][0],
                &sa_form_array[a][b][c][1],
                &sa_form_array[a][b][c][2],
                &sa_form_array[a][b][c][3]);
                if(sa_form_array[a][b][c][0] != 0.000){
                    //printf("%f\n",sa_form_array[a][b][c][d][h][i][0]);
                    state_not_equal_zero ++;
                }
                if(sa_form_array[a][b][c][1] != 0.000){
                    //printf("%f\n",sa_form_array[a][b][c][d][h][i][1]);
                    state_not_equal_zero ++;
                }
                if(sa_form_array[a][b][c][2] != 0.000){
                    //printf("%f\n",sa_form_array[a][b][c][d][h][i][2]);
                    state_not_equal_zero ++;
                }
                if(sa_form_array[a][b][c][3] != 0.000){
                    //printf("%f\n",sa_form_array[a][b][c][d][h][i][3]);
                    state_not_equal_zero ++;
                }
            }
        }
        printf("loading...%.3f%\n",100.0 * (a+1) / 7 );
    }
}
void sa_form_save(void){
    int a,b,c;
    for(a=0;a<=6;a++){
        for(b=0;b<=7;b++){
            for(c=0;c<=6;c++){
                fprintf(sa_form_w,"%f %f %f %f\n",
                sa_form_array[a][b][c][0],
                sa_form_array[a][b][c][1],
                sa_form_array[a][b][c][2],
                sa_form_array[a][b][c][3]);
                if(sa_form_array[a][b][c][0] != 0){
                    //printf("%f\n",sa_form_array[a][b][c][d][h][i][0]);
                    state_not_equal_zero_after_exe++;
                }
                if(sa_form_array[a][b][c][1] != 0){
                    //printf("%f\n",sa_form_array[a][b][c][d][h][i][1]);
                    state_not_equal_zero_after_exe++;
                }
                if(sa_form_array[a][b][c][2] != 0){
                    //printf("%f\n",sa_form_array[a][b][c][d][h][i][2]);
                    state_not_equal_zero_after_exe++;
                }
                if(sa_form_array[a][b][c][3] != 0){
                    //printf("%f\n",sa_form_array[a][b][c][d][h][i][3]);
                    state_not_equal_zero_after_exe++;
                }
            }
        }
        printf("loading...%.3f%\n",100.0 * (a+1) / 7 );
    }
    //if(sa_form_w == NULL) printf("warring\n");
}
int random_zero_to_three(void){
    return rand()%4;
}
int random_zero_to_two(void){
    return rand()%3;
}
int random_act_deside(void){
    if(rand()%100 < 100 * EPS){
        return 0;
    }
    else {
        return 1;
    }
}
void sa_form_flash(int* status_new, int* status_old){
    //printf("sa_form_flash\n");
    float max_act = sa_form_max_act(status_new);
    //printf("sa_form_max_act done\n");
    float sa_old_value = sa_form_array[status_old[0]][status_old[1]][status_old[2]][status_old[3]];
    float flash_value = GAMMA*(reward + ALPHA*max_act - sa_old_value);
    if(sa_old_value>0 && flash_value>0){
        if(sa_old_value + flash_value <=0){
            return ;
        }
    }
    else if(sa_old_value<0 && flash_value<0){
        if(sa_old_value + flash_value >=0){
            return ;
        }
    }
    else{
        sa_form_array[status_old[0]][status_old[1]][status_old[2]][status_old[3]] += flash_value;
    }
}
float sa_form_max_act(int* status){
    //printf("sa_form_max_act\n");
    float max;
    float pt1 = sa_form_array[status[0]][status[1]][status[2]][0];
    float pt2 = sa_form_array[status[0]][status[1]][status[2]][1];
    float pt3 = sa_form_array[status[0]][status[1]][status[2]][2];
    float pt4 = sa_form_array[status[0]][status[1]][status[2]][3];
    max = pt1;
    if(pt2 > max) max = pt2;
    if(pt3 > max) max = pt3;
    if(pt4 > max) max = pt4;
    return max;
}
int sa_form_act(int* status){
    //printf("sa_form_act\n");
    int act;
    float max;
    float act0 = sa_form_array[status[0]][status[1]][status[2]][0];
    float act1 = sa_form_array[status[0]][status[1]][status[2]][1];
    float act2 = sa_form_array[status[0]][status[1]][status[2]][2];
    float act3 = sa_form_array[status[0]][status[1]][status[2]][3];
    max = act0;
    act = 0;
    if(random_act_deside()){
        return random_zero_to_three();
    }
    if(act1 > max){
        max = act1;
        act = 1;
    }
    if(act2 > max){
        max = act2;
        act = 2;
    }
    if(act3 > max){
        max = act3;
        act = 3;
    }
    return act;

}
void sa_train_mode(void){
    int player1_act;
    int player2_act;
    if(end_game){
        if(killed_player == 1){
            //printf("player1 win\n");
            reward = -1000;
            sa_form_flash(status_next, status_now);
            reward = 10;
            sa_form_flash(status_trainer_next, status_trainer_now);
            //printf("%d %d %d %d %d %d %d ",status_now[0],status_now[1],status_now[2],status_now[3],status_now[4],status_now[5],status_now[6]);
            //printf("%.3f\n",sa_form_array[status_now[0]][status_now[1]][status_now[2]][status_now[3]][status_now[4]][status_now[5]][status_now[6]]);
            //printf("%d %d %d %d %d %d %d ",status_trainer_now[0],status_trainer_now[1],status_trainer_now[2],status_trainer_now[3],status_trainer_now[4],status_trainer_now[5],status_trainer_now[6]);
            //printf("%.3f\n",sa_form_array[status_trainer_now[0]][status_trainer_now[1]][status_trainer_now[2]][status_trainer_now[3]][status_trainer_now[4]][status_trainer_now[5]][status_trainer_now[6]]);
        }
        if(killed_player == 2){
            //printf("player2 win\n");
            reward = 10;
            sa_form_flash(status_next, status_now);
            reward = -1000;
            sa_form_flash(status_trainer_next, status_trainer_now);
        }
    }else{
        reward = 0;
        sa_form_flash(status_next, status_now);
        sa_form_flash(status_trainer_next, status_trainer_now);
        steps++;
    }
   // printf("process 2\n");
    player1_act = sa_form_act(status_now);
    player2_act = sa_form_act(status_trainer_now);
    //printf("%d %d %d\n",steps, player1_act, player2_act);
    //printf("process 3\n");
    do_act(player1_act,1);
    do_act(player2_act,2);
    //printf("process 4\n");
    status_now[3] = player1_act;
    status_trainer_now[3] = player2_act;
    flash_train_mode_status();
}
void sa_game_mode(void){
    int player2_act;
    if(end_game){
        if(killed_player == 1){
            player1_life -= 1;
            if(player1_life != 0){
                end_game = 0;
            }
            else{
                window = 4;
            }
            //printf("AI win\n");
            reward = 10;
            sa_form_flash(status_trainer_next, status_trainer_now);
        }
        if(killed_player == 2){
            //printf("YOU win\n");
            player2_life -= 1;
            if(player2_life != 0){
                end_game = 0;
            }
            else{
                window = 4;
            }
            reward = -1000;
            sa_form_flash(status_trainer_next, status_trainer_now);
        }
    }else{
        reward = 0;
        sa_form_flash(status_trainer_next, status_trainer_now);
        steps++;
    }
   // printf("process 2\n");
    player2_act = sa_form_act(status_trainer_now);
    //printf("%d %d %d\n",steps, player1_act, player2_act);
    //printf("process 3\n");
    do_act(player2_act,2);
    //printf("process 4\n");
    status_trainer_now[3] = player2_act;
    flash_game_mode_status();

}
void do_act(int act,int player){
    //printf("do_act\n");
    if(player == 1){
        if(act == 0){
            return ;
        }
        else if(act == 1){
            bullet1_place[1] = character1.x/58;
            return ;
        }
        else if(act == 2 ){
            if(character1.x >= 58){
                character1.x -= 58;
                return ;
            }
            else{
                return ;
            }
        }
        else if(act == 3){
            if(character1.x <348){
                character1.x += 58;
                return ;
            }
            else{
                return ;
            }
        }
        else{
            printf("Something wrong with do_act function 1\n");
        }
    }
    else{
        if(act == 0){
            return ;
        }
        else if(act == 1){
            bullet2_place[1] = character2.x/58;
            return ;
        }
        else if(act == 2){
            if(character2.x >=58){
                character2.x -= 58;
                return ;
            }
            else{
                return ;
            }

        }
        else if(act == 3){
            if(character2.x <348){
                character2.x += 58;
                return ;
            }
            else{
                return ;
            }
        }
        else{
            printf("Something wrong with do_act function 2\n");
        }
    }
}
void flash_train_mode_status(void){
    //printf("flash_status\n");
    (bullet2_place[6] == -1 )? (status_now[1] = 7) :( status_now[1] = bullet2_place[6]);
    (bullet2_place[5] == -1 )? (status_next[1] = 7) : (status_next[1] = bullet2_place[5]);
    (bullet1_place[6] == -1 )? (status_trainer_now[1] = 7) : (status_trainer_now[1] = bullet1_place[6]);
    (bullet1_place[5] == -1 )? (status_trainer_next[1] = 7): (status_trainer_next[1] = bullet1_place[5]);

    status_now[2] = character2.x/58;
    status_trainer_now[2] = character1.x/58;
    status_now[0] = character1.x/58;
    status_trainer_now[0] = character2.x/58;

    status_next[2] = character2.x/58;
    status_trainer_next[2] = character1.x/58;
    status_next[3] = 0;
    status_trainer_next[3] = 0;

    if(status_now[3] == 0){
        status_next[0] = status_now[0];
        //status_next[1] = 0;
    }else if(status_now[3] == 1){
        status_next[0] = status_now[0];
    }
    else if(status_now[3] == 2 ){
        if(status_now[0]>=1){
            status_next[0] = status_now[0] -1;
        }else{
            status_next[0] = status_now[0];
        }
    }
    else if(status_now[3] == 3){
        if(status_now[0] < 6){
            status_next[0] = status_now[0] +1;
        }
        else{
            status_next[0] = status_now[0] ;
        }
    }
    if(status_trainer_now[3] == 0){
        status_trainer_next[0] = status_trainer_now[0];
    }
    else if(status_trainer_now[3] == 1){
        status_trainer_next[0] = status_trainer_now[0];
    }
    else if(status_trainer_now[3] == 2){
        if(status_trainer_now[0]>=1){
            status_trainer_next[0] = status_trainer_now[0] -1;
        }else{
            status_trainer_next[0] = status_trainer_now[0] ;
        }
    }
    else if(status_trainer_now[3] == 3){
        if(status_trainer_now[0] < 6){
            status_trainer_next[0] = status_trainer_now[0] +1;
        }else{
            status_trainer_next[0] = status_trainer_now[0] ;
        }
    }
    //printf("status_now[6] = %d\n",status_now[6]);
    //printf("status_trainer_now[6] = %d\n",status_trainer_now[6]);
    /*
    for(i=0;i<=9;i++){
        printf("status_now[i] = %d\n", status_now[i]);
    }
    for(i=0;i<=9;i++){
        printf("status_next[i] = %d\n", status_next[i]);
    }
    for(i=0;i<=9;i++){
        printf("status_trainer_now[i] = %d\n", status_trainer_now[i]);
    }
    for(i=0;i<=9;i++){
        printf("status_trainer_next[i] = %d\n", status_trainer_next[i]);
    }
    */
}
void flash_game_mode_status(void){

    (bullet1_place[6] == -1 )? (status_trainer_now[1] = 7) : (status_trainer_now[1] = bullet1_place[6]);
    (bullet1_place[5] == -1 )? (status_trainer_next[1] = 7): (status_trainer_next[1] = bullet1_place[5]);

    status_trainer_now[2] = character1.x/58;
    status_trainer_now[0] = character2.x/58;

    status_trainer_next[2] = character1.x/58;
    status_trainer_next[3] = 0;

    if(status_trainer_now[3] == 0){
        status_trainer_next[0] = status_trainer_now[0];
    }
    else if(status_trainer_now[3] == 1){
        status_trainer_next[0] = status_trainer_now[0];
    }
    else if(status_trainer_now[3] == 2){
        if(status_trainer_now[0]>=1){
            status_trainer_next[0] = status_trainer_now[0] -1;
        }else{
            status_trainer_next[0] = status_trainer_now[0] ;
        }
    }
    else if(status_trainer_now[3] == 3){
        if(status_trainer_now[0] < 6){
            status_trainer_next[0] = status_trainer_now[0] +1;
        }else{
            status_trainer_next[0] = status_trainer_now[0] ;
        }
    }
}
void clear(void){
    int i;
    game_round ++;
    steps = 0;
    reward = 0;
    end_game = 0;
    killed_player = 0;
    character1.x = 174;
    character1.y = 497;
    character2.x = 174;
    character2.y = 0;
    player1_life = 10;
    player2_life = 10;
    for(i=0;i<=7;i++){
        bullet1_place[i] = -1;
        bullet2_place[i] = -1;
    }
    status_now[0] = status_now[2] = 3;
    status_next[0] = status_next[2] = 3;
    status_trainer_now[0] = status_trainer_now[2] = 3;
    status_trainer_next[0] = status_trainer_next[2] = 3;
    status_now[1] = status_next[1] = 7;
    status_trainer_now[1] = status_trainer_next[1] = 7;
    status_now[3] = status_next[3] = 0;
    status_trainer_now[3] = status_trainer_next[3] = 0;
    //status_next = {3,7,7,7,7,3,0};
    //status_now = {3,7,7,7,7,3,0};

}
