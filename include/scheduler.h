#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "thread.h"

void scheduler_init();
void scheduler_add_thread(Thread* thread);
void scheduler_switch();
Thread* scheduler_get_current();
void scheduler_set_current(Thread* thread);

#endif
