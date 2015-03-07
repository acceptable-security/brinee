#include <system.h>

int timer_ticks = 0;

void timer_handler(struct regs *r) {
    timer_ticks++;

    // Every 18.222 cycles is one second.
    if (timer_ticks % 18 == 0) {
        puts("One second has passed\n");
    }
}

void timer_wait(int ticks) {
    unsigned long eticks;

    eticks = timer_ticks + ticks;
    while(timer_ticks < eticks);
}