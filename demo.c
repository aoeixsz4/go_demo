#include "board.h"
#include "display.h"
#include "event.h"

int
main ()
{
    board_stack_init(19);
    display_init();
    display_update();
    event_wait();
    return 0;
}
