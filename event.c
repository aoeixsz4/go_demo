#include <stdio.h>
#include "SDL.h"
#include "board.h"
#include "display.h"

/* Try to place a stone */
static void
_leftmouseup (int mouse_x, int mouse_y)
{
    int colour;
    int x, y;
    int square_size;
    int size;
    colour = board_get_to_play();
    size = board_get_size();
    square_size = SCREEN_SIZE / size;
    x = mouse_x / square_size;
    y = mouse_y / square_size;
    board_do_move(colour, x, y);
    display_update();
}

static void
_debug ()
{
    board_dump();
}

void
event_poll ()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_MOUSEBUTTONUP:
                switch (event.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        _leftmouseup(event.button.x, event.button.y);
                        break;
                    case SDL_BUTTON_RIGHT:
                        board_rewind();
                        display_update();
                }
                break;
            case SDL_QUIT:
                exit(0);
                break;
            case SDL_KEYDOWN:
                _debug();
                break;
        }
    }
}

void
event_wait ()
{
    while (1)
    {
        event_poll();
        SDL_Delay(200);
    }
}
