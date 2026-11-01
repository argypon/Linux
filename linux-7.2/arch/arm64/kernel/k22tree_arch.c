// SPDX-License-Identifier: GPL-2.0
#include <linux/sched.h>
#include <linux/k22tree.h>

unsigned long arch_k22_kernelpc(struct task_struct *t)
{
	if (task_curr(t))
		return 0;
	return t->thread.cpu_context.pc;
}
