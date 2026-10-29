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

Submission assessment:
Part 1 (90%): the patch implements the syscall, UAPI structure, process-tree
traversal, and syscall-number registration for x86-64 and the generic syscall
table used by ARM64. Part 2 (10%): the userspace program retries with larger
buffers, prints the returned process tree with indentation, and reports errors.
The userspace program builds with the supplied Makefile. Runtime validation of
the syscall still requires booting a kernel built with the patch, so this
assessment is based on implementation and build checks rather than a live
syscall run.

Build the userspace program:
  make -C userspace

If the header was installed into a kernel source tree's usr/include directory:
  make -C userspace KERNEL_HEADERS=/path/to/linux-7.2/usr/include

Run on the patched kernel:
  ./userspace/k22test
