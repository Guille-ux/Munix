#include "tasks.h"

multitask_clock_t clock_task;

void initClockTask(int nibtc) {
	clock_task.current_count = 0;
	clock_task.nibtc = nibtc;
	clock_task.is_enabled = false;
}
