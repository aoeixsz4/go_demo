#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "board.h"

static struct stack *top;

void
board_dump ()
{
    int i, nr_strings = 0;
    struct string_data *string;
    for (i = 0; i < MAX_STRINGS; ++i)
        if (top->string[i].mark == 1)
            nr_strings++;
    fprintf(stderr, "%d strings: {\n", nr_strings);
    for (i = 0; i < MAX_STRINGS; ++i)
    {
        string = &top->string[i];
        if (string->mark == 1)
        {
            fprintf(stderr,
                    "\t%d: { stones: %d, liberties: %d, origin: (%d, %d), colour: %d }\n",
                    i, string->stones, string->liberties,
                    Px(string->origin), Py(string->origin), string->colour);
        }
    }
}

static void
end_turn (struct stack *our_stack)
{
    if (our_stack->to_play == BLACK)
        our_stack->to_play = WHITE;
    else if (our_stack->to_play == WHITE)
        our_stack->to_play = BLACK;
}

char
board_get_to_play ()
{
    return top->to_play;
}

int
board_get_size ()
{
    return top->size;
}

struct stack *
board_get_stack ()
{
    return top;
}

static struct stack *
stack_init (int size)
{
    int i, j;
    struct stack *new_stack = malloc(sizeof(*new_stack));
    memset(&new_stack->board, OUT_OF_BOUNDS, sizeof(new_stack->board));
    new_stack->size = size;
    new_stack->to_play = BLACK;
    new_stack->prev = NULL;
    for (i = 0; i < size; i++)
    {
        for (j = 0; j < size; j++)
        {
            new_stack->board[INDEX(i, j)] = EMPTY;
        }
    }
    for (i = 0; i < MAX_STRINGS; i++)
    {
        new_stack->string[i].mark = 0;
    }
    for (i = 0; i < BOARD_ARRAY_SIZE; i++)
    {
        new_stack->string_index[i] = -1;
    }
}

static void
_board_print (struct stack *current)
{
    int i, j;
    for (i = 0; i < BOARD_ARRAY_SIZE / ROWLENGTH; i++)
    {
        for (j = 0; j < ROWLENGTH; j++)
        {
            switch (current->board[i * ROWLENGTH + j])
            {
                case OUT_OF_BOUNDS:
                    fprintf (stderr, "#");
                    break;
                case EMPTY:
                    fprintf (stderr, ".");
                    break;
                case BLACK:
                    fprintf (stderr, "X");
                    break;
                case WHITE:
                    fprintf (stderr, "O");
                    break;
                default:
                    fprintf (stderr, "$");
            }
        }
        fprintf (stderr, "\n");
    }
}

static struct stack *
copy_stack (struct stack *src)
{
    struct stack *dest = malloc(sizeof(*dest));
    memcpy(dest, src, sizeof(*dest));
}

static void
merge (struct stack *our_stack,
        struct string_data *new, struct string_data *extant)
{
    int i, j, index;

    /* Update liberties */
    for (i = 0; i < extant->liberties; i++)
    {
        for (j = 0; j < new->liberties; j++)
        {
            if (extant->liberty[i] == new->liberty[j])
                break;
        }

        if (j == new->liberties)
            new->liberty[new->liberties++] = extant->liberty[i];
    }

    /* Fixup stack->string_index */
    index = extant->origin;
    for (i = 0; i < extant->stones; i++)
    {
        our_stack->string_index[index] = new->index;
        index = our_stack->next_stone[index];
    }

    /* Merge linked lists */
    index = our_stack->next_stone[new->origin];
    our_stack->next_stone[new->origin] = our_stack->next_stone[extant->origin];
    our_stack->next_stone[extant->origin] = index;
    new->stones += extant->stones;

    /* Update origin */
    if (new->origin > extant->origin)
        new->origin = extant->origin;

    /* Mark old string as vacant */
    extant->mark = -1;
}

static void
remove_liberty (struct string_data *string, int index)
{
    int i, j;
    for (i = 0; i < string->liberties; i++)
    {
        if (string->liberty[i] == index)
        {
            for (j = i + 1; j < string->liberties; j++)
            {
                string->liberty[j-1] = string->liberty[j];
            }
            string->liberties--;
            break;
        }
    }
}

static struct string_data **
get_neighbours (struct stack *our_stack, int index, int *n)
{
    struct string_data **neighbour;
    int i, j, delta[] = {DNORTH, DEAST, DSOUTH, DWEST}, string_index[4];

    /* Get immediate neighbours */
    for (i = 0; i < 4; i++)
    {
        string_index[i] = our_stack->string_index[index + delta[i]];
        if (string_index[i] != -1)
        {
            /* Ensure no index pointing to self
             * (i.e. our_stack->string_index[index]) */
            if (string_index[i] == our_stack->string_index[index])
                string_index[i] = -1;

            /* Remove duplicates */
            for (j = 0; j < i; j++)
            {
                if (string_index[j] == string_index[i])
                    string_index[i] = -1;
            }
        }
    }

    /* Package pointers into a new array */
    neighbour = malloc(4 * sizeof(*neighbour));
    *n = 0;
    for (i = 0; i < 4; i++)
    {
        if (string_index[i] != -1)
            neighbour[(*n)++] = &our_stack->string[string_index[i]];
    }
    return neighbour;
}

static struct string_data *
create_string (struct stack *our_stack, char colour, int index)
{
    struct string_data *new_string;
    int i, delta[] = {DNORTH, DEAST, DSOUTH, DWEST};

    /* Find a space for our new string data */
    for (i = 0; i < MAX_STRINGS; i++)
    {
        if (our_stack->string[i].mark != 1)
        {
            new_string = &our_stack->string[i];
            break;
        }

    }
    new_string->index = i;

    /* Add immediate liberties to the new string */
    new_string->liberties = 0;
    for (i = 0; i < 4; i++)
    {
        if (our_stack->board[index + delta[i]] == EMPTY)
            new_string->liberty[new_string->liberties++] = index + delta[i];
    }

    /* Set basic string data */
    new_string->mark = 1;
    new_string->origin = index;
    new_string->stones = 1;
    new_string->colour = colour;

    /* Fixup index hook */
    our_stack->string_index[index] = new_string->index;

    /* Initialise (cyclic) linked index list */
    our_stack->next_stone[index] = index;

    /* Mark on board */
    our_stack->board[index] = colour;
    return new_string;
}

static void
free_liberty (struct stack *our_stack, struct string_data *string, int index)
{
    int i;

    /* prevent adding a liberty twice */
    for (i = 0; i < string->liberties; i++)
    {
        if (string->liberty[i] == index)
            break;
    }

    if (i == string->liberties)
        string->liberty[string->liberties++] = index;
}

static void
unlink (struct stack *our_stack, int index)
{
    struct string_data **neighbour;
    int i, n;

    our_stack->board[index] = EMPTY;
    our_stack->next_stone[index] = -1;
    our_stack->string_index[index] = -1;
    neighbour = get_neighbours(our_stack, index, &n);
    for (i = 0; i < n; i++)
        free_liberty(our_stack, neighbour[i], index);
    free(neighbour);
}

static void
capture (struct stack *our_stack, struct string_data *our_string)
{
    int i, index, next;

    index = our_string->origin;
    for (i = 0; i < our_string->stones; i++)
    {
        next = our_stack->next_stone[index];
        unlink(our_stack, index);
        index = next;
    }

    our_string->mark = 0;
}

static struct stack *
try_move (char colour, int index)
{
    struct stack *new_stack;
    struct string_data *my_string, **neighbour;
    int i, n;

    if (top->board[index] != EMPTY)
        return NULL;

    new_stack = copy_stack(top);
    my_string = create_string(new_stack, colour, index);
    neighbour = get_neighbours(new_stack, index, &n);

    /* For each neighbour, remove the liberty at index,
     * then merge or capture if necessary */
    for (i = 0; i < n; i++)
    {
        remove_liberty(neighbour[i], index);

        if (neighbour[i]->colour == colour)
            merge(new_stack, my_string, neighbour[i]);
        else if (neighbour[i]->liberties == 0)
            capture(new_stack, neighbour[i]);
    }
    free(neighbour);

    /* Return the new stack provided we didn't just self-capture */
    if (my_string->liberties > 0)
        return new_stack;
    else
        return NULL;
}

void
board_do_move (char colour, int x, int y)
{
    int index = INDEX(x, y);
    struct stack *new_stack = try_move(colour, index);
    if (new_stack)
    {
        new_stack->prev = top;
        top = new_stack;
        end_turn(top);
    }
}

void
board_stack_init (int size)
{
    top = stack_init(size);
}

void
board_rewind ()
{
    struct stack *old_stack;
    if (top->prev)
    {
        old_stack = top->prev;
        free(top);
        top = old_stack;
        _board_print(top);
    }
}