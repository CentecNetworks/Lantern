/*
 * context.h - api and structs for applying frame deltas from child to parent
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
#ifndef context_h
#define context_h

/*
 * struct op_context - this should be passed in and out of the
 *   Context package, but its members should not be accessed
 *   by external packages.  They are subject to change.
 */
struct op_context {
	void *pc;		/* Program counter for this context */
#if RTITOOLS_CONTEXT_USES_FP
	void *fp;		/* Frame pointer for this context */
#endif
#if RTITOOLS_CONTEXT_USES_SP
	void *sp;		/* Stack pointer for this context */
#endif

	unsigned long gpregs[BT_NUM_GPREGS];	/* general purpose registers
						   used for tracing.
						   These are only valid for
						   the first call frame. */

	bool leaf;		/* false if no possibility of being a leaf */

	bool firstInstruction;	/* == true if we are at the first
				 * instruction of a function */

	unsigned int patchDepth;	/* counts number of patched functions
					 * we've passed through to get to this
					 * call stack frame.
					 * Needed to accurately trace callstacks
					 * with one patched function recursively
					 * calling itself and another patched
					 * function.
					 * Trust me. */

	crawl_validation validation_parms;	/* currently valid PC text range
						   and task id for context */
};

/*
 *  Applies the results of a stack frame trace (as described by the given
 * op_context structure) to an instance of a stack frame (as given by the
 * "child" parameter) and stores the resulting "parent" stack frame description
 * in the "parent" parameter.  All of the parameters must point at fully valid
 * structures.
 *
 *  This routine validates pointers (e.g., sp, fp) before dreferencing them.
 * If it discovers an invalid value, it aborts and returns false.
 * Otherwise it returns true.
 */
extern bool apply_context_results(struct op_context *const child,
				  const struct frame_deltas *const resFrame,
				  struct op_context *parent);

#endif /* context_h */
