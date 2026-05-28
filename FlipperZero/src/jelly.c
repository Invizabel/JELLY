#include "dolphin/dolphin.h"
#include <furi.h>
#include <furi_hal_rtc.h>
#include <gui/gui.h>
#include <stdlib.h>

int SCORE = 0;
char score_str[16];

int snacks[13][5];
bool moving_right = false;
bool moving_left = false;

int jellyfish_x_y[][2] = {{60,54},{60,54},{60,54},{60,54}};

int player_x = 6;
int player_y = 4;

// coordinates for drawing
int jellyfish[][2] = {{3,2},{4,2},{5,2},{6,2},{7,2},{8,2},{2,3},{9,3},{2,4},{9,4},{3,5},{4,5},{5,5},{6,5},{7,5},{8,5},{3,6},{6,6},{8,6},{4,7},{6,7},{9,7},{2,8},{4,8},{7,8},{3,9}};
int players[][2] = {{7,4},{8,4},{3,5},{4,5},{6,5},{9,5},{3,6},{5,6},{10,6},{3,7},{5,7},{10,7},{3,8},{4,8},{6,8},{9,8},{7,9},{8,9}};


void collide_rect()
{
    
    int player_left = player_x;
    int player_top = player_y - 1;
    int player_right = player_x + 8;
    int player_bottom = player_y + 10;

    for (int i = 0; i < 4; i++)
    {
        int jellyfish_left = jellyfish_x_y[i][0];
        int jellyfish_top = jellyfish_x_y[i][1] - 1;
        int jellyfish_right = jellyfish_x_y[i][0] + 8;
        int jellyfish_bottom = jellyfish_x_y[i][1] + 10;

        bool jellyfish_collision = player_left <= jellyfish_right && player_right >= jellyfish_left && player_top <= jellyfish_bottom && player_bottom >= jellyfish_top;

        if (jellyfish_collision)
        {
            SCORE = 0;
            // pass for now
        }
    }
}

void draw_player(Canvas* canvas)
{
    int array_size = sizeof(players) / sizeof(players[0]);
    for(int i = 0; i < array_size; i++)
    {
        int x = players[i][0];
        int y = players[i][1];
        if(x != 0 && y != 0)
        {
            canvas_draw_dot(canvas, x + player_x, y + player_y);
        }
    }
}

void draw_jellyfish(Canvas* canvas)
{   
    int array_size = sizeof(jellyfish) / sizeof(jellyfish[0]);
    for (int i = 0; i < 4; i++)
    {
        for(int j = 0; j < array_size;j++)
        {
            int x = jellyfish[j][0];
            int y = jellyfish[j][1];
            if(x != 0 && y != 0)
            {
                canvas_draw_dot(canvas, x + jellyfish_x_y[i][0], y + jellyfish_x_y[i][1]);
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
    draw_jellyfish(canvas);

    
    if (moving_right && player_x < 114)
    {
        player_x += 4;
    }

    if (moving_left && player_x > 6)
    {
        player_x -= 4;
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
