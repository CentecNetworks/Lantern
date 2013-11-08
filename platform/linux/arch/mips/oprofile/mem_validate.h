/*
 * mem_validate.h - inline api to validate that memory is reasonable
 *
 * Copyright (c) 2008 Windriver Systems, Inc.
 *
 * Author: David Lerner <david.lerner@windriver.com>
 *         Wind River scopetools developers
 *
 * This code is closely adapted from the improved backtrace 'context' analysis
 * code that was developed to be shared by profiler and debugger
 * products, host and target based for various Wind River Systems, Inc products.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#ifndef _MEM_VALIDATE_H_
#define _MEM_VALIDATE_H_

/*
 * Return 1 if will  page fault, or 0 if it won't
 */
extern int will_page_fault(const void *const address);

/*
 * Context address types
 */
typedef enum rtiContextAddrType {
	CNTXT_ADDR_STACK_T,	/* stack aligned, within stack limits */
	CNTXT_ADDR_FRAME_T,	/* frame aligned, within stack limits */
	CNTXT_ADDR_TEXT_T,	/* width aligned, within task text limits */
	CNTXT_ADDR_UNKNOWN_T	/* memProbe candidate */
} RtiTools_ContextAddressType;

/*
 *  A simple range of target addresses.
 */
typedef struct {
	void *pStart;
	void *pEnd;
} ADRS_RANGE;

#define range_set(range, start, end)            \
	do {                                    \
		if (range != 0) {               \
			range->pStart = start;  \
			range->pEnd = end;      \
		}                               \
	} while (0);

/*
 *  The crawl_validation structure holds values which are used to expidite
 * validation of target addresses.  For example, the "tid" member can be used
 * to determine stack bounds, making validation of stack addresses trivial.
 * Task id (tid) is represented as an unsigned long, since Vx tids are
 * pointers, using the unsigned long type makes sure there's enough bitwidth
 * available for storage.  */
typedef struct {
	unsigned long tid;
	ADRS_RANGE txtRng;
} crawl_validation;

/*
 * Allocated by backtrace client and currently used to pass
 *	data between PCOk and instruction() to preserve original
 *	WindRiver backtrace API organization
 */
struct memory_access_data {
	void *pc_copied;
	void *data;
};

/*
 * PCOk - check for valid PC address
 *
 * Input Args:
 *  pc         - PC text address to validate
 *  membuf - a pointer to an internal buffer setup by client
 *
 * Returns:
 *  true if valid address, else, false
 */
static inline bool PCOk(const void const *pc, struct memory_access_data *membuf)
{
	if (will_page_fault(pc))
		return false;
	if (__copy_from_user_inatomic(&membuf->data, pc, sizeof(membuf->data)))
		return false;
	membuf->pc_copied = (void *)pc;
	return true;
}

/*
 *  The instruction() routine is used by the op_frame_crawl() function to
 * discover which instruction resides at the given pc.  For normal cross-
 * compilation, where the crawling code is running in the same memory context
 * as the instructions it's looking at, this is macro-ized into essentially a
 * no-op.  In the case we're compiling the crawl code for host-native use,
 * we simply provide the signature for this routine.  The client *must* provide
 * an implementation.
 *
 * The "pc" parameter is used by op_frame_crawl() to specify the target-
 * relative address of the instruction it's inspecting.  The "arg" parameter is
 * an opaque pointer to client-defined data; op_frame_crawl() will pass the
 * pointer given to it to all calls to instruction().
 *
 * instruction() should return a pointer which op_frame_crawl() can dereference
 * to read the instructions at the given target pc.  The instruction at that
 * address should be represented in the endianness of the machine executing
 * op_frame_crawl().
 */

static inline
    void *instruction(const void const *pc, struct memory_access_data *membuf)
{
	if (membuf->pc_copied != pc) {
		if (__copy_from_user_inatomic
		    (&membuf->data, pc, sizeof(membuf->data))) {
			/* copy failed, stuff bad instruction into the buffer */
			membuf->data = 0;
		}
	}
	return &membuf->data;
}

 /*
  * FPOk - check for a valid frame-pointer value
  *
  * Input Args:
  *  fp         - frame address to validate
  *  client_arg - ignored in this implementation, using page fault tests instead
  *
  * Returns:
  *  true if valid address, else, false
  */
#define FPOk SPOk

 /*
  * SPOk - check for a valid stack-pointer value
  *
  * Input Args:
  *  sp         - stack address to validate
  *  client_arg - ignored in this implementation, using page fault tests instead
  *
  * Returns:
  *  true if valid address, else, false
  */
static inline bool SPOk(const void *const sp, crawl_validation *validation)
{
	return !will_page_fault(sp);
}

#endif /* _MEM_VALIDATE_H_ */
