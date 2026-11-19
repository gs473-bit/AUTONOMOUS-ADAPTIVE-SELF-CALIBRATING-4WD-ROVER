#include "config.h"
#include "hal_port/board.h"
/* Safe bootstrap; later modules can schedule logic but may never bypass the
 * compile-time actuation interlock while real wiring remains unverified. */
int main(void) {
    board_boot();
    for (;;) {
        board_idle();
    }
}
