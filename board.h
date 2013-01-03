#ifndef _BOARD_H_
#define _BOARD_H_
#define MAX_BOARD 19
#define MAX_BOARD_1D (MAX_BOARD * MAX_BOARD)
#define MAX_STRINGS MAX_BOARD_1D
#define KO -2
#define OUT_OF_BOUNDS -1
#define EMPTY 0
#define BLACK 1
#define WHITE 2
#define PAD 2
#define ROWLENGTH (2 * PAD + MAX_BOARD)
#define INDEX(x, y) (PAD + x + (PAD + y) * ROWLENGTH)
#define Px(index) ((index % ROWLENGTH) - PAD)
#define Py(index) ((index / ROWLENGTH) - PAD)
#define TOTAL_PAD (ROWLENGTH * 2 * PAD + MAX_BOARD * 2 * PAD)
#define BOARD_ARRAY_SIZE (MAX_BOARD_1D + TOTAL_PAD)
#define NORTH(index) (index - ROWLENGTH)
#define EAST(index) (index + 1)
#define SOUTH(index) (index + ROWLENGTH)
#define WEST(index) (index - 1)
#define DNORTH (-ROWLENGTH)
#define DEAST (1)
#define DSOUTH (ROWLENGTH)
#define DWEST (-1)
#define OPPOSITE(colour) ((colour - 1) ? WHITE : BLACK)
struct string_data {
    int mark; /* -1 means removed, 0 means empty, 1 means active */
    int liberties;
    int liberty[MAX_STRINGS];
    int origin;
    int stones;
    char colour;
    int index;
};
struct stack {
    char board[BOARD_ARRAY_SIZE];
    char to_play;
    int size;
    struct string_data string[MAX_STRINGS];
    int string_index[BOARD_ARRAY_SIZE]; /* -1 means no string */
    int next_stone[BOARD_ARRAY_SIZE]; /* -1 if no stone at index */
    void *prev;
};
void board_stack_init(int size);
void board_do_move(char, int, int);
void board_dump(void);
char board_get_to_play(void);
struct stack *board_get_stack(void);
int board_get_size(void);
void board_rewind(void);
#endif /* _BOARD_H_ */
