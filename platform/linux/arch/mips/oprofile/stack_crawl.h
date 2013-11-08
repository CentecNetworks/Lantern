/*
 * stack_crawl.h
 * Contains the Arch-independent api to the arch-dependent code. To be included
 * by clients of the <arch>_crawl functionality.
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

#ifndef stack_crawl_h
#define stack_crawl_h

/*
 * This symbol can also be set to '1' by the developer from the make
 * command line to enable using development features and testing code:
 * make <target> "RTI_TESTING_FLAG = '-DOP_CONTEXT_DEV_MODE=1'"
 */
#if defined(OP_CONTEXT_DEV_MODE)
#undef OP_CONTEXT_DEV_MODE
#endif

/* Development mode - 0 for prod */
#define OP_CONTEXT_DEV_MODE       (1)

/*
 *  A context package-wide verbosity setting.  Should be be set and reside in
 * client's code.
*/
extern int op_context_debug_output;

/*
 * some logging operations for debugging
 */
#include <linux/kernel.h>
#if OP_CONTEXT_DEV_MODE
#define v0printf(fmt, args...) printk(fmt, ##args)
#  define v1printf        if (op_context_debug_output >= 1) \
				v0printf
#  define v2printf        if (op_context_debug_output >= 2) \
				v0printf
#  define v3printf        if (op_context_debug_output >= 3) \
				v0printf
#  define d0printf(fmt, args...)     v0printf(fmt, ##args)
#  define d1printf(fmt, args...)     v1printf(fmt, ##args)
#  define d2printf(fmt, args...)     v2printf(fmt, ##args)
#  define d3printf(fmt, args...)     v3printf(fmt, ##args)
#else /* if OP_CONTEXT_DEV_MODE */
#  define d0printf(fmt, args...)
#  define d1printf(fmt, args...)
#  define d2printf(fmt, args...)
#  define d3printf(fmt, args...)
#endif /* if OP_CONTEXT_DEV_MODE */

/*
 * Location operations, specifying where the associated value can be found.
 */
enum value_location {
	LOCATION_UNKNOWN = 0,	/* uh-oh */
	LOCATION_UNUSED,	/* If this location is unused by this arch */
	LOCATION_INDETERMINATE,	/* the value in question no longer exists.
				   E.g., a non-returning function chooses not
				   to save its LR.  Tracing from a child of this
				   function, there's no way of determining the
				   non-returning routine's parent. */
	LOCATION_REG_OFFSET,	/* value is (reg) + (offset) */
	LOCATION_REG_INDIRECT,	/* value is *(reg + byte offset) */
	LOCATION_ABSOLUTE,	/* treat the offset as the actual value */
	LOCATION_NEW_SP,	/* [pc only] update sp and THEN update pc */
	LOCATION_ERROR		/* not to ever be returned! Only exists as a
				   dynamic marker on the bounds of this enum */
};

/*
 *  A structure specifying how a value can be calculated, given access to a
 * set of registers, and target memory.  May specify a new absolute value,
 * indicate memory is to be dereferenced, etc.
 */
struct new_value_delta {
	enum value_location location;
	enum register_name reg;
	long offset;
};

/*
 *  struct frame_deltas describes the location of the arch specific values
 * for a particular stack call frame (typically the frame which "called" a
 * given frame.  Note, this structure describes the location and not the
 * actual value of these values, allowing client code to calculate the values
 * in implementation-specific ways.
 */
struct frame_deltas {
	struct new_value_delta pcLocation;
#if RTITOOLS_CONTEXT_USES_FP
	struct new_value_delta fpLocation;
#endif
#if RTITOOLS_CONTEXT_USES_SP
	struct new_value_delta spLocation;
#endif
	/* TD: is this only used as a debugging aid?
	 *  If so, ifdef out for production
	 */
	unsigned long steps;	/* #steps to locate parent function */
};

/*
 * op_frame_crawl - given a target pc value, locate the calling stack frame
 *
 * DESCRIPTION
 *  This function analyzes instruction flow around the given program counter
 * and returns when it has either figured out the locations of the calling
 * stack frame, or determined it's been confused and cannot figure out where
 * the calling context is located.  The function containing "startpc" should
 * conform to the c/c++ abi, otherwise this function may be less accurate.
 * "gprs_available" should be set to true if the caller has access to valid
 * contents of the general purpose register values corresponding to the
 * "child's" stack frame.  When set to false, FrameCrawl() will do extra work
 * to locate these values on the stack.  Note that all non-pc, non-sp, and
 * non-fp registers are considered "general purpose," including the LR.
 *
 * The "client_arg" parameter value is not directly used by the FrameCrawl()
 * routine at all.  However it is passed to all instruction() and PCOk() calls,
 * and is used to pass client-specific data that may be necessary for the
 * client-specific implementations of these functions.
 *
 * RETURNS
 *   true            - context found
 *   false         - context not found
 */
/* TD: should this routine ever conceptually return "TOP"? */
extern bool op_frame_crawl(void *startpc,
			   void *client_arg,
			   bool gprs_available, struct frame_deltas *resFrame);

/*
 * A convenience function to quickly denote a register offset type delta.
 */
static inline
    void location_set_reg_off(struct new_value_delta *delta,
			      const enum register_name reg, const long offset)
{
	delta->location = LOCATION_REG_OFFSET;
	delta->reg = reg;
	delta->offset = offset;
}

/*
 * A convenience function to quickly denote a register indirect type delta.
 */
static inline
    void location_set_reg_indirect(struct new_value_delta *delta,
				   const enum register_name reg,
				   const long offset)
{
	delta->location = LOCATION_REG_INDIRECT;
	delta->reg = reg;
	delta->offset = offset;
}

/*
 *  A convenience function to quickly reset/initialize a ContextFrame structure
 * to a known starting state.
 */
static inline void init_ContextFrame(struct frame_deltas *resFrame)
{
	resFrame->pcLocation.location = LOCATION_UNKNOWN;
	resFrame->pcLocation.reg = 0;
	resFrame->pcLocation.offset = 0;

#if RTITOOLS_CONTEXT_USES_SP
	resFrame->spLocation.location = LOCATION_UNKNOWN;
	resFrame->spLocation.reg = 0;
	resFrame->spLocation.offset = 0;
#endif

#if RTITOOLS_CONTEXT_USES_FP
	resFrame->fpLocation.location = LOCATION_UNKNOWN;
	resFrame->fpLocation.reg = 0;
	resFrame->fpLocation.offset = 0;
#endif
}

/*
 *  A convenience function to quickly determine if a delta is unknown or unused.
 */
static inline bool is_location_unknown_unused(struct new_value_delta *delta)
{
	return delta->location == LOCATION_UNKNOWN ||
	    delta->location == LOCATION_UNUSED;
}

#if OP_CONTEXT_DEV_MODE
/*
 *  A debugging utility function providing a convenient method for dumping the
 * stack around a given value.
 */
static inline void dumpStack(const unsigned long *sp, unsigned int width)
{
	unsigned int i;
	const unsigned long *trav = sp - width / 2;

	for (i = 0; i < width; ++i) {
		if ((trav + i) != sp) {
			d0printf("\t%p: 0x%lx\n", trav + i, *(trav + i));
		} else {
			d0printf("\t%p: 0x%lx  <------\n", trav + i,
				 *(trav + i));
		}
	}
}
#endif /* if OP_CONTEXT_DEV_MODE */

#endif /* stack_crawl_h */
