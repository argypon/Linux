// SPDX-License-Identifier: GPL-2.0
#include <linux/sched.h>
#include <linux/k22tree.h>
#include <asm/switch_to.h>

unsigned long arch_k22_kernelpc(struct task_struct *t)
{
	struct inactive_task_frame *frame;

	if (task_curr(t))
		return 0;
	frame = (struct inactive_task_frame *)READ_ONCE(t->thread.sp);
	return frame ? frame->ret_addr : 0;
}
