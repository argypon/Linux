// SPDX-License-Identifier: GPL-2.0
/*
 * hello system call implementation
 */

#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define HELLO_MSG "Hello "

/**
 *
 * hello: Reads a string <s> and prepends "Hello " to it
 * -
 * @input:   The input buffer to read the input string from
 * @in_size: The size of the input buffer, excluding the null-terminator
 *
 * @output:   The output buffer to place the new string at
 * @out_size: The size of the output buffer
 *
 * This syscall returns an error if the generated string does not fit in the
 * output buffer, or if (in_size + 1) including the null terminator does not
 * fit within 2 pages, or if the output buffer does not fit within 2 pages;
 * otherwise it returns the length of the new string on success.
 *
 * NOTE: This function calls kmalloc with the GFP_KERNEL flag on, which means it
 * could sleep. Therefore, the caller of this function should probably not invoke
 * it while holding a spinlock...
 *
 */
static int do_k22_hello(const char *input, size_t in_size, char *output,
			size_t out_size)
{
	int return_code;
	int buf_in_size;
	int actual_in_size;
	int expected_out_size;
	char *input_kbuf;
	char *output_kbuf;

	pr_info("I miss Alexis Delis! But I managed to invoke a syscall...\n");

	if (!input || !output || in_size == 0 || in_size >= 2 * PAGE_SIZE ||
	    out_size == 0) {
		return_code = -EINVAL;
		goto out;
	}

	/* Allocate buffer to store user-provided string */
	buf_in_size = in_size + 1;
	input_kbuf = kmalloc(buf_in_size, GFP_KERNEL);
	if (!input_kbuf) {
		return_code = -ENOMEM;
		goto out;
	}

	input_kbuf[in_size] = '\0';

	/* Safely copy user-provided string from user space to kernel space */
	if (copy_from_user(input_kbuf, input, in_size)) {
		return_code = -EFAULT;
		goto free_input;
	}

	actual_in_size = strlen(input_kbuf);
	expected_out_size = strlen(HELLO_MSG) + actual_in_size + 1;

	/* Make sure output fits into the output buffer */
	if (expected_out_size > out_size || expected_out_size > 2 * PAGE_SIZE) {
		return_code = -EINVAL;
		goto free_input;
	}

	/* Allocate the output buffer */
	output_kbuf = kmalloc(expected_out_size, GFP_KERNEL);
	if (!output_kbuf) {
		return_code = -ENOMEM;
		goto free_input;
	}

	/* Format desired string into the temporary output buffer */
	snprintf(output_kbuf, expected_out_size, HELLO_MSG "%s", input_kbuf);

	/* Write resulting string back to user */
	if (copy_to_user(output, output_kbuf, expected_out_size)) {
		return_code = -EFAULT;
		goto free_output;
	}

	return_code = expected_out_size;

free_output:
	kfree(output_kbuf);

free_input:
	kfree(input_kbuf);
out:
	return return_code;
}

SYSCALL_DEFINE4(k22_hello, const char __user *, input, size_t, in_size,
		char __user *, output, size_t, out_size)
{
	return do_k22_hello(input, in_size, output, out_size);
}
