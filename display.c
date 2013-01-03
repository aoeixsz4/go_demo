/* display.c - Deals with the SDL display of the go board */
#include "SDL.h"
#include "SDL_draw.h"
#include "board.h"
#include "display.h"

SDL_Surface *screen = NULL;

static void
_draw_board_background ()
{
    int board_size = board_get_size();
    int screen_size = SCREEN_SIZE;
    int square_size = screen_size / board_size;
    int i;
    
    if (!screen)
    {
        /* print error */
        return;
    }

    /* draw BG colour */
    Draw_FillRect(screen, 0, 0, screen_size, screen_size, BACKGROUND_COLOUR);

    for (i = 0; i < board_size; i++)
    {
        Draw_Line(screen, square_size/2 + square_size*i, square_size/2,
                square_size/2 + square_size*i, square_size/2 + square_size*(board_size - 1),
                0);
    }

    for (i = 0; i < board_size; i++)
    {
        Draw_Line(screen, square_size/2, square_size/2 + square_size*i,
                square_size/2 + square_size*(board_size - 1), square_size/2 + square_size*i,
                0);
    }
}

static void
_draw_board_stones ()
{
    struct stack *our_stack = board_get_stack();
    int x, y, index;
    int square_size;
    int board_size = our_stack->size;

    square_size = SCREEN_SIZE / board_size;
    for (x = 0; x < board_size; x++)
    {
        for (y = 0; y < board_size; y++)
        {
            index = INDEX(x, y);
            if (our_stack->board[index] == BLACK)
                Draw_FillCircle(screen, square_size/2 + x*square_size,
                        square_size/2 + y*square_size, square_size / 2, 0);
            else if (our_stack->board[index] == WHITE)
                Draw_FillCircle(screen, square_size/2 + x*square_size,
                        square_size/2 + y*square_size, square_size / 2, 0xffffff);
        }
    }
}

void
display_init ()
{
    SDL_Init(SDL_INIT_EVERYTHING);
    screen = SDL_SetVideoMode(SCREEN_SIZE, SCREEN_SIZE, COLOUR_DEPTH, SDL_SWSURFACE);
}

void
display_update ()
{
    _draw_board_background();
    _draw_board_stones();
    SDL_Flip(screen);
}
