// SPDX-License-Identifier: GPL-2.0
/*
 *Implementation of k22tree: copy the process tree to user space in DFS order
 */
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/sched/stat.h>
#include <linux/sched/task.h>
#include <linux/sched/task_stack.h>
#include <linux/ptrace.h>
#include <linux/k22info.h>
#include <linux/k22tree.h>

/*Return the kernel instruction pointer for a task(x86 and arm64 override it) */
unsigned long __weak arch_k22_kernelpc(struct task_struct *t)
{
	return 0;
}

/*Check if a task should be included in the tree*/
static bool match(struct task_struct *t, bool alive_only)
{
	return thread_group_leader(t) && (!alive_only || !t->exit_state);
}

static struct task_struct *next_child(struct task_struct *p,
				      struct task_struct *prev, bool alive_only)
{
	struct task_struct *th, *c;
	bool seen = !prev;

	for_each_thread(p, th) {
		list_for_each_entry(c, &th->children, sibling) {
			if (!seen) {
				seen = (c == prev);
				continue;
			}
			if (match(c, alive_only))
				return c;
		}
	}
	return NULL;
}

static struct task_struct *first_child(struct task_struct *p, bool alive_only)
{
	return next_child(p, NULL, alive_only);
}

static struct task_struct *next_sibling(struct task_struct *t, bool alive_only)
{
	if (t == &init_task)	
		return NULL;
	return next_child(t->real_parent, t, alive_only);
}

static void fill(struct k22info *i, struct task_struct *t)
{
	struct task_struct *c = first_child(t, true);
	struct task_struct *s = next_sibling(t, true);

	memset(i, 0, sizeof(*i));
	get_task_comm(i->comm, t);
	i->pid = task_pid_nr(t);
	i->parent_pid = (t == &init_task) ? 0 : task_tgid_nr(t->real_parent);
	i->first_child_pid = c ? task_pid_nr(c) : 0;
	i->next_sibling_pid = s ? task_pid_nr(s) : 0;
	i->nvcsw = t->nvcsw;
	i->nivcsw = t->nivcsw;
	i->start_time = t->start_time;

	if (t->exit_state)
		return;

	if (!(t->flags & (PF_KTHREAD | PF_IO_WORKER)))
		i->userpc = instruction_pointer(task_pt_regs(t));
	i->kernelpc = arch_k22_kernelpc(t);
}

SYSCALL_DEFINE2(k22tree, struct k22info __user *, buf, int __user *, ne)
{
	struct task_struct *p, *next;
	struct k22info *kbuf;
	int max, n = 0, ret = 0;

	if (!buf || !ne)
		return -EFAULT;
	if (get_user(max, ne))
		return -EFAULT;
	if (max <= 0)
		return -EINVAL;

	/* Don't allocate (much) more than the number of tasks that exist. */
	if (max > nr_threads + 32)
		max = nr_threads + 32;

	/* Allocate a temporary kernel buffer for the entries */
	kbuf = kvmalloc_array(max, sizeof(*kbuf), GFP_KERNEL | __GFP_NOWARN);
	if (!kbuf)
		return -ENOMEM;

	read_lock(&tasklist_lock);
	p = &init_task;
	while (p) {
		if (n == max) {
			ret = -ENOSPC;
			break;
		}
		fill(&kbuf[n++], p);

		/* DFS: first child, else next sibling, else climb up */
		next = first_child(p, false);
		while (!next && p != &init_task) {
			next = next_sibling(p, false);
			if (!next)
				p = p->real_parent->group_leader;
		}
		p = next;
	}
	read_unlock(&tasklist_lock);

	/* Copy the entries to the caller's buffer*/
	if (copy_to_user(buf, kbuf, n * sizeof(*kbuf)))
		ret = -EFAULT;
	else if (!ret && put_user(n, ne))
		ret = -EFAULT;

	/* Free the temporary buffer*/
	kvfree(kbuf);
	return ret;
}
