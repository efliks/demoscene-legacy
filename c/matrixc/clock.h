#ifndef _CLOCK_H
#define _CLOCK_H

void clock_init();
void clock_shutdown();

void timer_start(unsigned long);
void timer_wait();

#endif // _CLOCK_H
