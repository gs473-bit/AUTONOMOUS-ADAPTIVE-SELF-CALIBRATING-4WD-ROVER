#include "hal_port/board.h"
#include "app/controller.h"
/* No motor GPIO/PWM configuration in the unverified build. */
int main(void) {
    board_boot();
    controller_init();
    for (;;) {
        controller_tick(board_millis());
        board_idle();
    }
}
