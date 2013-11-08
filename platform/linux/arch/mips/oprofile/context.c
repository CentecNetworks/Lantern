/*
 * Manages register deltas created by the op_frame_crawl api and
 * provides the api to apply these delta on the child frame to
 * the parent frame.  The term 'context' refers to the callstack
 * context.
 *
 * Copyright (c) 2001-2008 Windriver Systems, Inc.
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

#include <linux/uaccess.h>
#include "mips_context.h"
#include "stack_crawl.h"
#include "mem_validate.h"
#include "context.h"

/*
 * Use the given struct new_value_delta specification to calculate the required
 * value based on currently accessible memory and the register values stored in
 * the "child" parameter.  The "stack_range" parameter is used to cache the
 * currently validated range of accessible stack for the execution context
 * denoted by "child".
 *
 * Returns true if the value was successfully calculated.  In this case,
 * "result" is populated with the result of the computation.
 */

bool apply_delta(const struct new_value_delta *const delta,
		 const struct op_context *const child,
		 crawl_validation *stack_range, void **result)
{
	/* Determine value using the current context and the calculated delta */
	if (LOCATION_REG_OFFSET == delta->location) {
#if RTITOOLS_CONTEXT_USES_SP
		if (BT_REG_SP == delta->reg) {
			*result =
			    (void *)((unsigned long)child->sp + delta->offset);
		} else
#endif
#if RTITOOLS_CONTEXT_USES_FP
		if (BT_REG_FP == delta->reg) {
			*result =
			    (void *)((unsigned long)child->fp + delta->offset);
		} else
#endif
		{
			/* picked up from one of the gpregs, these are only
			 * valid for leaf frames
			  */
			if (!child->leaf) {
				d1printf
				    ("value in register in non-leaf - aborting\n");
				return false;
			}
			*result = (void *)
			    ((unsigned long)child->gpregs[delta->reg] +
			     delta->offset);
		}
	}

	else if (LOCATION_REG_INDIRECT == delta->location) {
		void **target;
#if RTITOOLS_CONTEXT_USES_SP
		if (BT_REG_SP == delta->reg) {
			target =
			    (void *)((unsigned long)child->sp + delta->offset);
			if (!SPOk(target, stack_range)) {
				d1printf
				    ("Invalid address in child sp (%p + 0x%lx)- aborting\n",
				     child->sp, delta->offset);
				return false;
			}
		} else
#endif
#if RTITOOLS_CONTEXT_USES_FP
		if (BT_REG_FP == delta->reg) {
			target =
			    (void **)((unsigned long)child->fp + delta->offset);
			if (!SPOk(target, stack_range)) {
				d1printf
				    ("Invalid address in child fp (%p + 0x%lx)- aborting\n",
				     child->fp, delta->offset);
				return false;
			}
		} else
#endif
		{
			/* picked up from one of the gpregs */
			if (!child->leaf) {
				d1printf
				    ("value in register in non-leaf - aborting\n");
				return false;
			}

			target =
			    (void *)((unsigned long)child->gpregs[delta->reg] +
				     delta->offset);
		}
		if (will_page_fault(target)) {
			d1printf
			    ("Invalid address %p picked up from gpregs+%d\n",
			     target, delta->reg);
			return false;
		}
		*result = *target;
	}

	else if (LOCATION_ABSOLUTE == delta->location) {
		*result = (void **)delta->offset;
		if (!SPOk(*result, stack_range)) {
			d1printf("Invalid absolute address: %p\n", *result);
			return false;
		}

	} else if (LOCATION_UNKNOWN == delta->location) {
		d1printf("Unknown SP location - aborting\n");
		return false;
	} else {
		d0printf("Unknown location value: %d\n", delta->location);
		return false;
	}

	return true;
}

/*
 *  Applies the results of a stack frame trace (as described by the given
 * ContextFrame structure) to an instance of a stack frame (as given by the
 * "child" parameter) and stores the resulting "parent" stack frame description
 * in the "parent" parameter.  All of the parameters must point at fully valid
 * structures.
 *
 *  This routine validates pointers (e.g., sp, fp) before dreferencing them.
 * If it discovers an invalid value, it aborts and returns false.
 * Otherwise it returns true.
 */
bool apply_context_results(struct op_context *const child,
			   const struct frame_deltas *const resFrame,
			   struct op_context *parent)
{
	/* NOTE: this logic used to use spOk and fpOk booleans to track whether
	 * the stack and frame pointer addresses had been checked already, so
	 * that future dereferences didn't need to be checked.  However, in
	 * practice, this could lead to unchecked dereferences since the first
	 * check would be on (reg + offset), and a future check against
	 * (reg + new_offset) would be skipped if the first succeeded.
	 * This practice was replaced with using a valid address range tracking
	 * var, stack_range.  This has a few advantages, including avoiding the
	 * above issue, can be faster for archs which use both stack and frame
	 * pointers since the valid stack range is shared. */
	crawl_validation stack_range;
	stack_range.tid = child->validation_parms.tid;
	stack_range.txtRng.pStart = 0;
	stack_range.txtRng.pEnd = 0;

#if RTITOOLS_CONTEXT_USES_SP
	/* Determine SP value using current context and calculated delta */
	if (LOCATION_UNUSED == resFrame->spLocation.location) {
		parent->sp = child->sp;
	} else {
		if (!apply_delta(&resFrame->spLocation, child, &stack_range,
				 &parent->sp)) {
			d1printf("Error applying SP delta\n");
			return false;
		}
	}
#endif /* RTITOOLS_CONTEXT_USES_SP */

#if RTITOOLS_CONTEXT_USES_FP
	/* Determine FP value using current context and calculated delta */
	if (LOCATION_UNUSED == resFrame->fpLocation.location)
		parent->fp = child->fp;
#  if RTITOOLS_CONTEXT_USES_SP
	/* Note that only the SH uses this flag for fp location now.  We should
	   examine the target-specific code to see if it's really necessary
	   and/or appropriate. */
	else if (LOCATION_NEW_SP == resFrame->fpLocation.location) {
		void **target =
		    (void **)((unsigned long)parent->sp +
			      resFrame->fpLocation.offset);
		if (!(SPOk(target, &stack_range))) {
			d1printf
			    ("Invalid address in parent sp (%p) while locating fp - aborting\n",
			     target);
			return false;
		}
		parent->fp = *target;
	}
#  endif /* RTITOOLS_CONTEXT_USES_SP */
	else {
		if (!apply_delta(&resFrame->fpLocation, child, &stack_range,
				 &parent->fp)) {
			d1printf("Error applying FP delta\n");
			return false;
		}
	}
#endif /* RTITOOLS_CONTEXT_USES_FP */

	/* Determine PC value using current context and calculated delta */
#if RTITOOLS_CONTEXT_USES_SP
	if (LOCATION_NEW_SP == resFrame->pcLocation.location) {
		void **target =
		    (void **)((unsigned long)parent->sp +
			      resFrame->pcLocation.offset);
		if (!(SPOk(target, &stack_range))) {
			d1printf
			    ("Invalid address in parent sp (%p) - aborting\n",
			     target);
			return false;
		}
		parent->pc = *target;
	}
#endif /* USES_SP */

	else {
		if (!apply_delta(&resFrame->pcLocation, child, &stack_range,
				 &parent->pc)) {
			d1printf("Error applying PC delta\n");
			return false;
		}
	}

	/* no way of failing now, copy over the rest of the known values */
	return true;
}
