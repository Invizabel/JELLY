#include "dolphin/dolphin.h"
#include <furi.h>
#include <furi_hal_rtc.h>
#include <gui/gui.h>
#include <stdlib.h>

int SCORE = 0;
int HIGH_SCORE = 10;
char score_str[16];

int snacks[13][5];
bool moving_right = false;
bool moving_left = false;

int player_x = 60;
int player_y = 54;

int fish_x = 6;
int fish_y = 4;

int ball_x = 64;
int ball_y = 46;

// player coordinates for drawing
int player[][2] = {{3,2},{4,2},{5,2},{6,2},{7,2},{8,2},{2,3},{9,3},{2,4},{9,4},{3,5},{4,5},{5,5},{6,5},{7,5},{8,5},{3,6},{6,6},{8,6},{4,7},{6,7},{9,7},{2,8},{4,8},{7,8},{3,9}};
int fish[][2] = {{7,4},{8,4},{3,5},{4,5},{6,5},{9,5},{3,6},{5,6},{10,6},{3,7},{5,7},{10,7},{3,8},{4,8},{6,8},{9,8},{7,9},{8,9}};


void collide_rect()
{
    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            int ball_left = ball_x - 4;
            int ball_top = ball_y - 4;
            int ball_right = ball_x + 4;
            int ball_bottom = ball_y + 4;
            
            int fish_left = fish_x - 8;
            int fish_top = fish_y - 8;
            int fish_right = fish_x + 8;
            int fish_bottom = fish_y + 8;
            
            bool fish_collision = fish_left <= ball_right && fish_right >= ball_left && fish_top <= ball_bottom && fish_bottom >= ball_top;

            if (fish_collision)
            {
                snacks[x][y] = 0;
            }
        }
    }
}


void draw_player(Canvas* canvas)
{   
    int array_size = sizeof(player) / sizeof(player[0]);
    for(int i = 0; i < array_size; i++)
    {
        int x = player[i][0];
        int y = player[i][1];
        if(x != 0 && y != 0)
        {
            canvas_draw_dot(canvas, x + player_x, y + player_y);
        }
    }
}

void draw_fish(Canvas* canvas)
{
    for (int x = 0; x < 13; x++)
    {
        for (int y = 0; y < 5; y++)
        {
            if (snacks[x][y] == 1)
            {
                int array_size = sizeof(fish) / sizeof(fish[0]);
                for(int i = 0; i < array_size; i++)
                {
                    if(x != 0 && y != 0)
                    {
                        int xer = fish[i][0];
                        int yer = fish[i][1];
                        canvas_draw_dot(canvas,(((128 - (8 * fish_x + 7 * 4)) / 2) + x * (fish_x + 4) + xer) - 30, (4 + y * (fish_y + 4) + yer) - 10);
                    }
                }
            }
        }
    }
}

static void input_callback(InputEvent* event, void* context)
{
    FuriMessageQueue* queue = (FuriMessageQueue*)context;
    if(event->type == InputTypeShort || event->type == InputTypeRepeat || event->type == InputTypePress)
    {
        if (event->key == InputKeyLeft)
        {
            moving_right = false;
            moving_left = true;
        }

        if (event->key == InputKeyRight)
        {
            moving_right = true;
            moving_left = false;
        }
    }
    
    
    furi_message_queue_put(queue, event, FuriWaitForever);
}


static void draw_callback(Canvas* canvas, void* context)
{
    UNUSED(context);
    canvas_clear(canvas);
    collide_rect();
    draw_player(canvas);
    draw_fish(canvas);

    
    if (moving_right && player_x < 114)
    {
        player_x += 4;
    }

    if (moving_left && player_x > 6)
    {
        player_x -= 4;
    }

    if (ball_y == 2)
    {
        HIGH_SCORE = 10;

        SCORE = 0;
    }

    snprintf(score_str, sizeof(score_str), "%d", SCORE);
    canvas_draw_str(canvas,2,8,score_str);

    canvas_commit(canvas);
}

int main()
{
    for (int i = 0; i < 13; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            snacks[i][j] = 1;
        }
    }
    
    DateTime dt;
    furi_hal_rtc_get_datetime(&dt);
    unsigned int seed = dt.hour * 3600 + dt.minute * 60 + dt.second;
    srand(seed);

    FuriMessageQueue* queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, NULL);
    view_port_input_callback_set(view_port, input_callback, queue);
    Gui* gui = (Gui*)furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    dolphin_deed(DolphinDeedPluginGameStart);
    InputEvent event;
    bool running = true;
    while(running)
    {
        if(furi_message_queue_get(queue, &event, FuriWaitForever) == FuriStatusOk)
        {
            if(event.type == InputTypeShort && event.key == InputKeyBack)
            {
                running = false;
            }
        }
        view_port_update(view_port);
    }

    view_port_enabled_set(view_port, false);
    furi_message_queue_free(queue);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    return 0;
}
