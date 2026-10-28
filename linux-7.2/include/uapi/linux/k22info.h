/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _UAPI_LINUX_K22INFO_H
#define _UAPI_LINUX_K22INFO_H

#include <linux/types.h>

struct k22info {
	char comm[64];
	__kernel_pid_t pid;
	__kernel_pid_t parent_pid;
	__kernel_pid_t first_child_pid;
	__kernel_pid_t next_sibling_pid;
	unsigned long nvcsw;
	unsigned long nivcsw;
	unsigned long start_time;
	unsigned long userpc;
	unsigned long kernelpc;
};

#endif
