#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "thread.h"

void scheduler_init(void);
void scheduler_add_thread(Thread* thread);
Thread* scheduler_get_current(void);
void scheduler_set_current(Thread* thread);
void scheduler_switch(void);
void scheduler_dump(void);

#endif
