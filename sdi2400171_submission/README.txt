K22 Programming Assignment 1

Q1.
A program with a high nivcsw count is heavily CPU-bound, performing continuous calculations without pausing. Because it rarely waits for external resources like disk or network, it consumes its entire allocated time slice, forcing the operating system's scheduler to preempt it and forcibly take the CPU away to let other processes run.

Q2. ✅ 
A program with a high nvcsw (voluntary context switch) count is a heavily I/O-bound or interactive program, that frequently pauses to wait for external events like network traffic, file access, or user input. Instead of using its full time slice, the program voluntarily "sends" the CPU back to the operating system every time it stops to wait for these resources to become available.

Q3. ✅ 
Many processes have a start time that matches the system boot time. These are not programs a user launched, they are kernel threads and system daemons launched automatically by the operating system. Kernel threads run purely in kernel space, while daemons are ordinary user-space programs that run on the system’s behalf. On the other hand, user programs show later, have scattered start times that match when the user started them after logging in.

Q4.
A non-null userpc means the task has a saved user-mode instruction pointer that can be reported as its return-to-user location. It does not necessarily mean that the task was executing in user mode at the instant of the snapshot.

Q5. The process list and task state can change between the two calls. A
process shown in the first output but not the second may have exited (or no
longer matched the PC condition) before the next snapshot. The swapper/0
entry is the idle kernel task and is expected to remain; its userpc and
kernelpc are null because it has no saved user-mode context and no saved
schedule return address in the reported state. The sample alone cannot prove
why cupsd disappeared, but process lifetime and scheduling state make this
kind of difference possible.


Grading assessment:
The submited patch does compile, passes all checkpatch linting ests without errors 
or warnings and the patched kernel does boot.

Correctness: ~50/60. The patch registers the syscall for x86-64 and ARM64,
defines the UAPI structure, and walks the task tree in depth-first order while
holding tasklist_lock. It handles invalid pointers, invalid capacities,
allocation failures, and copy-to-user errors. Runtime behavior should still be
tested with representative process trees and user/kernel tasks.

Robustness: ~20/30. The kernel uses a temporary buffer and reports insufficient
capacity, while the userspace program retries and checks errors. One area to
review is the x86 kernel-PC helper: checking task_curr() and reading
t->thread.sp do not by themselves guarantee that the task's saved stack stays
valid if it is scheduled concurrently.

Readability: ~8/10. The implementation is organized into small traversal and
record-filling helpers, and the userspace output is straightforward. Although 
comments could be mmore descriptive and formatting could be made more consistent 
with the kernel style.

Indicative total: 79/100, subject to the assignment's detailed syscall
specification and instructor evaluation.
