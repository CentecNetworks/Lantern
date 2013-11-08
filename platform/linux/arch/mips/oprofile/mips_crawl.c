/*
 * mips dependent frame crawling logic
 *
 * Copyright (c) 2007-2008 Windriver Systems, Inc.
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

/*
DESCRIPTION
Architecture specific part of context tools for MIPS32/64.

The MIPS n32 ABI was developed by Silicon Graphics Incorporated (SGI) for
ease in porting 32-bit operating systems and applications to 64-bit MIPS
processors.  This entails the following issues.

+ The n32 ABI supports 32-bit addresses and integers, sign extending them
to 64 bits before being placed in the 64-bit registers.  The original ABI
from MIPS is designated o32 and has no support for 64-bit registers,
restricts the number of floating point (FP) registers to 16 instead of 32,
and makes inefficient use of the FP registers.  Unlike o32 which allows
passing of up to 4 values in general purpose (GP) and FP registers
(r4-r7, f12-f15), n32 supports 8 registers each (r4-r11, f12-f19).

+ The 64-bit internal representation of data necessitates stack region
alignment to quad-words (16 bytes) and all stack entries must be aligned to
double-word (8-byte) addresses, even values occupying 32 bits or less.

+ Within each 64-bit stack slot non-FP values requiring less than 64 bits are
right justified, i.e., occupy the highest order bytes, in big-endian systems.
(MIPS is a native little-endian architecture that supports big-endian
operation.  So, for example, given a stack entry address of 0x40080, the
starting address of a byte, short, int, and double are as follows for the
different endian systems:

Type    Little      Big
------  --------    --------
byte    0x400080    0x400087
short   0x400080    0x400086
int32   0x400080    0x400084
float   0x400080    0x400084
double  0x400080    0x400080

+ Integers of 32 bits are always sign-extended before being passed in a
register, regardless of whether the integer is signed or not.

+ Quad-precision (128-bit) FP values (C long double) are passed in 16-byte
aligned stack slots and even-odd pairs of FP registers, even if doing so
skips a 64-bit stack slot or a register assignment.

+ FP values are passed in FP registers whenever possible, irrespective of any
integer values preceding them in an argument list, unlike o32.

    - Variable (varargs) arguments are an exception to this rule.  Any FP
    values in a varargs part of the argument list are passed in GP registers.
    NOTE: functions accepting variable numbers of FP arguments MUST be
    prototyped!

+ C99 complex types are passed in multiple FP registers, as available:

Type                    Passed as
-------------------     --------------
float complex           2 floats
double complex          2 doubles
long double complex     2 long doubles

This has an impact on stack slots.  Because the size of a float complex is
8 bytes (2 4-byte floats) but two float argument registers take 16 bytes
of stack space, the float complex stack entry is padded with an extra 8 bytes.
For example, if a float complex argument is passed in f14 and f15 and the next
argument in f16, the corresponding stack locations for a slot at 0x400080
would be the first float at 0x400080 and the second float at 0x400084, but
the following argument would be in a slot starting at 0x400090.

+ Structures, unions or other composite types are regarded as a series of
64-bit values and are passed into functions as a number of GP registers and/or
FP registers as they fit and any excess passed on the stack according to the
composition of the type and the stack alignment rules.  This means:

    - Regardless of the structure layout, it is treated by the compiler as a
    series of 64-bit parts.  If a part is a double float and not part of a
    union, it is passed in a FP register while all other parts as passed in
    GP registers.

    - All unions are treated as a series of integer double-words as far as
    GP register assignment is concerned.  NOTE: no attempt is made to
    differentiate FP members from integer for passing in FP registers!

    - Array members of structures are passed like unions while array arguments
    are passed by reference.  (Applies to C and C++, but others may differ.)

    - Despite right-justifying small scalar values in their stack slots,
    structure parameters are always left-justified, which applies to
    structures smaller than 64-bits as well as any remaing parts of a
    structure not passed in registers, whether or not it is an integral number
    of 64-bit parts.  In other words, the remaining part is stored on the
    stack exactly as laid out in regular memory.

+ Return values from functions are passed back in r2 (and r3 if needed) or
f0 (and f2 if needed), as specified by the type.  Note that the f0/f2 rule
allows for even/odd pairing.  Composite return types (structure, union, etc.)
have strange rules that say they must be returned as follows:

    - A structure with only one or two FP members returns them with f0 holding
    the first and and f2 the second.

    - Any other structure or union members of at most 128 bits are returned
    with r2 holding the first 64 bits, r3 holding the second 64 bits.

    - Composite return types larger than 128 bits are passed back transparently
    in a memory location reserved by the caller.  A pointer to the area is
    passed implicitly as the first argument to the function.

+ The callee function saves any non-volatile registers it uses, ranging from
GP registers r24 to r31 and the even FP registers from f20 to f30.

+ Unlike o32, routines are NOT restricted to a single exit block (epilogue).

+ Unlike o32, the gp (global pointer registr r28) is saved by the callee,
rather than the caller.  This gives the following saving scheme:

Reg#        Name    Use                     Saver
---------   -----   ----------------------  -------------
$0          zero    HW zero                 N/A
$1          at      assembler temporary     CALLER
$2-$3       v0-v1   function results        CALLER
$4-$11      a0-a7   function input args     CALLER
$12-$15     t4-t7   temporaries             CALLER
$16-$23     s0-s7   always saved if used    callee
$24-$25     t8-t9   temporaries             CALLER
$26-$27     k0-k1   reserved for kernel     N/A
$28         gp      global pointer          callee
$29         sp      stack pointer           callee
$30         s8/fp   frame pointer, if used  callee
$31         ra      return address          CALLER
hi,lo               mult/div special regs   CALLER
$f0,$f2             FP results              CALLER
$f1,$f3             FP temporaries          CALLER
$f4-$f11            FP temporaries          CALLER
$f12-$f19           FP input args           CALLER
$f20-$f23 (32-bit)  FP temporaries          CALLER
$f24-$f31 (64-bit)  FP temporaries          callee
$f20-$f31 (even, n32) FP temporaries        callee
$f20-$f31 (odd, n32)  FP temporaries        CALLER

+ At most 8 registers are used for passing input arguments, whether all FP,
all GP, or a combination thereof.  Any remaining arguments are passed on the
stack.  The function passing the arguments (the caller) is not required to
allocate space on the stack for the registers nor is the called function (the
callee), unlike o32.  In the following examples of register/stack assignment,
's' is a single precision FP, 'd' is a double precision FP, and 'n' is an
integer.

Argument List               N32 Register and Stack Assignments
--------------------------  ----------------------------------------------
n1,n2                       r4,r5
s1,s2                       f12,f13
d1,d2                       f12,f13
s1,d1                       f12,f13
d1,s1                       f12,f13
n1,d1                       r4,f13
d1,n1,d2                    f12,r5,f14
n1,n2,d1                    r4,r5,f14
d1,n1,n2                    f12,r5,r6
s1,n1,n2                    f12,r5,r6
d1,s1,s2                    f12,f13,f14
s1,s2,d1                    f12,f13,f14
n1,n2,n3,n4,n5,n6,n7,n8     r4,r5,r6,r7,r8,r9,r10,r11
n1,n2,n3,d1                 r4,r5,r6,f15
n1,n2,n3,s1                 r4,r5,r6,f15
s1,s2,s3,s4,s5,s6,s7,s8     f12,f13,f14,f15,f16,f17,f18,f19
s1,n1,s2,n2                 f12,r5,f14,r7
n1,s1,n2,s2                 r4,f13,r6,f15
n1,s1,n2,n3                 r4,f13,r6,r7
d1,d2,d3,d4,d5,d6,d7,d8     f12,f13,f14,f15,f16,f17,f18,f19
d1,d2,d3,d4,d5,s1,s2,s3,s4  f12,f13,f14,f15,f16,f17,f18,f19,stack
d1,d2,d3,s1,s2,s3,n1,n2,n3  f12,f13,f14,f15,f16,f17,r10,r11,stack

+ The bit sizes for o32, n32, and 64-bit model C types:

C Type          032 and n32         64-bit
--------------  ------------------  ------
char             8                   8
short int       16                  16
int             32                  32
long int        32                  64
long long int   64                  64
pointer         32                  64
float           32                  32
double          64                  64
long double     64 (128 in n32)     128
*/

#include <linux/types.h>
#include <linux/uaccess.h>
#include <linux/ptrace.h>

#include "mem_validate.h"
#include "mips_context.h"
#include "stack_crawl.h"

/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
 * Debugging/programming DEFINEs
 *-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
int op_context_debug_output; /* = 0 */

#if defined(CONFIG_64BIT)

/*
 * Make a page fault or else prove we won't.  Tool used by PCOk and SPOk
 * Return 0 if no page fault, 1 if page fault happened
 *
 * NOTE: The probe_kernel_read() routine does not catch unaligned
 * exceptions which occur in the upper address space for MIPS64.
 */
noinline int will_page_fault(const void *const address)
{
	register long ret asm("$2") = 0;

	if (((unsigned long)address & XKSEG) == XKPHYS)
		return 1;
	/* dereference the given address.  If this causes a page fault, our
	 * installed handlers should intercept this and set the "ret" register
	 * to 1.
	 * This instruction has an artificial dependency on the "ret" variable
	 * as an input parameter, which ensures "ret" is not forcefully set to
	 * true after our page fault handling mechanism may have set it false.
	 */
	asm volatile (
		".set push\n\t"
		".set noreorder\n\t"
		"op_will_page_fault_bottom:"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"lw   $3, %1\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"op_will_page_fault_top:"
		".set pop"
		: "+X" (ret) : "m" (*(long *)address) : "$3" );

	return ret;
}

extern void op_will_page_fault_bottom(void);
extern void op_will_page_fault_top(void);

/*
 * callback for do_page_fault and do_ade
 * return 1 to abort the do_page_fault
 * return 0 if fault not caused by our backtrace
 */
unsigned int op_page_fault_filter(struct pt_regs *regs)
{
	unsigned long pc = regs->cp0_epc;

	if ((pc >= (unsigned long) op_will_page_fault_bottom) &&
		(pc <= (unsigned long) op_will_page_fault_top)) {
		/* page corresponding to pc isn't in memory, set return
		 * value to true
		 */
		regs->regs[REG_V0] = 1;

		/* change pc to right instruction (*/
		regs->cp0_epc = pc + sizeof(tInst);

		return 1;
	}

	/* return 0, since this isn't our page fault */
	return 0;
}

#else

/*
 * Make a page fault or else prove we won't.  Tool used by PCOk and SPOk
 * Return 0 if no page fault, 1 if page fault happened
 */
int will_page_fault(const void *const address)
{
	long dst;
	if (probe_kernel_read(&dst, (void *)address, sizeof(long)) != 0)
		return 1;	/* page fault or address error */
	return 0;
}

#endif


/**
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
 * "child's" stack frame.  When set to false, op_frame_crawl() will do extra
 * work to locate these values on the stack.  Note that all non-pc, non-sp,
 * and non-fp registers are considered "general purpose," including the LR.
 *
 * The "client_arg" parameter value is not directly used by the op_frame_crawl()
 * routine at all.  However it is passed to all instruction() and PCOk() calls,
 * and is used to pass client-specific data that may be necessary for the
 * client-specific implementations of these functions.
 *
 * RETURNS
 *   true            - context found
 *   false         - context not found
 */
bool op_frame_crawl(void *startpc,
		    void *client_arg,
		    bool gprs_available, struct frame_deltas *res_frame)
{
	/*
	 * MIPS has three categories of functions [see "See Mips Run", p278]
	 * 1.  Leaf functions - ra stays in register, sp may be used
	 * 2.  Non-leaf functions - ra saved on stack, sp is allocated at
	 *     function prologue and stays that way during the function
	 * 3.  Complex functions - ra saved, sp is allocated at prologue but may
	 *     change during execution.  In these functions, a frame pointer is
	 *     used to store the low water mark for the stack pointer.
	 *
	 * For our purposes, these decompose into the following cases:
	 *   1.  ra is never saved or is unsaved when we start looking
	 *   2.  sp is never altered or is unaltered when we start looking
	 *   3.  ra is saved on the stack, referenced with sp
	 *   4.  ra is saved on the stack, referenced with fp
	 */

	bool reverseOperation = false;
	int i, j;
	const tInst *pcCur = 0;
	const tInst *pcCurStart = startpc;
	const tInst *branchArray[MAX_ARB_BRANCHES];
	tInst curInst;		/* instruction pointed to by pcCur */
	int branchCount = 0;
	long offset;
	bool fpBeingUsed = false;

	/* most clauses don't use the frame pointer, so by default we mark it as
	   untracked for the current frame. */

	res_frame->fpLocation.location = LOCATION_UNUSED;

	/*
	 * walk forward from the vicinity of the PC to determine
	 * the current stack frame
	 */
	for (i = 0; i < RTITOOLS_CONTEXT_MAX_FN_LENGTH; ++i) {
resumeFwdSearch:
		pcCur = pcCurStart + i;
		if (!PCOk(pcCur, client_arg)) {
			d1printf(" %p: invalid PC!  Loop #%u; reversing...\n",
				 pcCur, i);
			reverseOperation = true;
			break;
		}
		curInst = *(tInst *) instruction(pcCur, client_arg);

		d3printf("%p: op code 0x%08x\n", pcCur, curInst);

		/*----
		 * First, find instructions that indicate we're in the epilogue:
		 * ... [body] ...
		 * lw    ra,offset(sp)  [32-bit regs]
		 *    OR
		 * ld    ra,offset(sp)  [64-bit regs]
		 * ... [restore other registers]
		 * addiu sp,sp,positiveamount
		 * jr    ra
		 * ... [possibly addiu sp,sp,positiveamount] ...
		 */

		/*--- lw  ra,offset(sp)
		 *--- ld  ra,offset(sp)
		 * If we encounter this one first, we need to find out what
		 * local stack space is,
		 * i.e., continue searching down for addiu sp,sp,+xx.
		 */
		if (((curInst & MASK_SWLW_OPCODE) == INST_RESTORE_RA_32) ||
		    ((curInst & MASK_SWLW_OPCODE) == INST_RESTORE_RA_64)) {
			uint32_t nextInst, nextNextInst;
			offset = curInst & MASK_SWLW_OFFSET;
			location_set_reg_indirect(&res_frame->pcLocation,
						  BT_REG_SP, offset);

			/* PC location is affected by register size */
			if ((curInst & MASK_SWLW_OPCODE) == INST_RESTORE_RA_32) {
				d2printf("  %p: lw ra,0x%lx(sp)\n", pcCur,
					 offset);
			}
			/* MIPS64 BE has this -- we need to offset by a 32-bit
			 * register width to get the least significant 32 bits
			 * of the address because VxWorks currently supports
			 * only 32-bit addresses.
			 */
			if ((curInst & MASK_SWLW_OPCODE) == INST_RESTORE_RA_64) {
				res_frame->pcLocation.offset += REGISTER_OFFSET;
				d2printf("  %p: ld ra,0x%lx(sp)\n", pcCur,
					 res_frame->pcLocation.offset);
			}
			/* We must check for a GCC optimizer trick here to
			 * shorten our search for a return point.
			 * Some routines are optimized to be a special form of
			 * tail-continued with this epilogue:
			 *
			 *  ...
			 *  ld  ra,offset(sp)
			 *  j   <anotherRoutine>
			 *  addiu sp,sp,posConst
			 *
			 * In this case, we need only find the SP offset and
			 * we're done.
			 */
			if (!PCOk(pcCur + 2, client_arg)) {
				d1printf(" %p: bogus PC! [0]; reversing...\n",
					 pcCur + 2);
				reverseOperation = true;
				break;
			}

			nextInst =
			    *(uint32_t *) instruction(pcCur + 1, client_arg);
			nextNextInst =
			    *(uint32_t *) instruction(pcCur + 2, client_arg);

			if (((nextInst & MASK_JUMP_OPCODE) == INST_J) &&
			    ((nextNextInst & MASK_OPCODE) == INST_ADDIU)) {
				location_set_reg_off(&res_frame->spLocation,
						     BT_REG_SP,
						     nextNextInst &
						     MASK_ADD_OFFSET);
				d2printf("    %p: j <routine>\n", pcCur + 1);
				d2printf("    %p: addiu sp,sp,0x%lx(sp)\n",
					 pcCur + 2,
					 res_frame->spLocation.offset);

				return true;
			}
			continue;
		}

		/*--- move sp,fp
		 * restoring intermediate sp from fp
		 */
		if (curInst == INST_FP_TO_SP) {
			fpBeingUsed = true;
			location_set_reg_off(&res_frame->spLocation, BT_REG_FP,
					     0);
			d2printf("  %p: move sp,fp; fp = 0x%lx\n", pcCur,
				 res_frame->spLocation.offset);
			continue;
		}

		/*--- lw  fp,+d(sp)
		 * restoration of stack frame pointer register from stack
		 */
		if ((curInst & MASK_SWLW_FP_SP) == INST_LW_FP_SP) {
			/* we may already have seen this */
			if (is_location_unknown_unused(&res_frame->fpLocation)) {
				location_set_reg_indirect(&res_frame->
							  fpLocation, BT_REG_SP,
							  curInst &
							  MASK_SWLW_OFFSET);
				d2printf("  %p: lw fp,%ld(sp)\n", pcCur,
					 res_frame->fpLocation.offset);
			}
			continue;
		}

		/*--- jr ra
		 * End of routine.
		 * Search for stack restoration instruction either preceding
		 * this instruction or immediately following it in the branch
		 * delay slot.
		 *
		 * ...
		 * jr ra               / end of preceding routine [stop search]
		 * addiu sp,sp,-xx     / our prolog               [stop search]
		 * ...
		 * lw/ld ra,offset(sp) / look for this if not already known
		 * addiu sp,sp,+xx     / looking for this..
		 * ...
		 * jr ra        <-- we're here
		 * addiu sp,sp,+xx     / .. or this
		 */
		if (curInst == INST_RTS) {
			tInst nextInst;

			d2printf("  %p: jr ra\n", pcCur);
			if (!PCOk(pcCur + 1, client_arg)) {
				d1printf(" %p: bogus PC! [0]; reversing...\n",
					 pcCur + 1);
				reverseOperation = true;
				break;
			}

			nextInst =
			    *(tInst *) instruction(pcCur + 1, client_arg);
			d3printf("    %p: op code 0x%08x\n", pcCur + 1,
				 nextInst);

			/* check branch delay slot first */
			if ((nextInst & MASK_ADD_OPCODE) == INST_ADD_TO_SP) {
				offset = IMM_TO_INT(nextInst);

				/* want only offset >= 0; if < 0, it's a prolog */
				if (offset >= 0) {
					if (!fpBeingUsed) {
						res_frame->spLocation.location =
						    LOCATION_REG_OFFSET;
						res_frame->spLocation.reg =
						    BT_REG_SP;
					}
					res_frame->spLocation.offset += offset;
					d2printf
					    ("    %p: addiu sp,sp,0x%lx follows jr ra\n",
					     pcCur + 1, offset);
				} else {
					break;
				}
			}

			/* if we have not found PC or SP location yet,
			 * search back a few instructions
			 */
			if ((res_frame->pcLocation.location == LOCATION_UNKNOWN)
			    || (res_frame->spLocation.location ==
				LOCATION_UNKNOWN)) {
				/* Look backwards, but avoid the case where we
				 * might detect somebody else's stack restore:
				 *
				 * jr ra
				 * addiu sp, sp, -32
				 * our_function_label
				 * jr ra
				 * other_delay_slot
				 */

				/* j <= i so that we don't go beyond starting point */
				for (j = 1; j < MAX_BACKWARD_SEARCH && j <= i;
				     j++) {
					tInst travInst;

					if (!PCOk(pcCur - j, client_arg)) {
						d1printf
						    (" %p: bogus PC! [1]; reversing...\n",
						     pcCur - j);
						reverseOperation = true;
						break;
					}

					travInst =
					    *(tInst *) instruction(pcCur - j,
								   client_arg);

					/* search ends at preceding routine's end */
					if (travInst == INST_RTS) {
						d2printf
						    ("    jr ra @ %p while searching for "
						     "addiu sp,sp,+xx; stop search\n",
						     pcCur - j);
						break;
					}

					/*--- lw  fp,+d(sp)
					 * restoration of stack frame pointer
					 * register from stack
					 */
					if ((travInst & MASK_SWLW_FP_SP) ==
					    INST_LW_FP_SP) {
						/* we may already have seen this */
						if (is_location_unknown_unused
						    (&res_frame->fpLocation)) {
							location_set_reg_indirect
							    (&res_frame->
							     fpLocation,
							     BT_REG_SP,
							     travInst &
							     MASK_SWLW_OFFSET);
							d2printf
							    ("    %p: lw fp,%ld(sp)\n",
							     pcCur - j,
							     res_frame->
							     fpLocation.offset);
						}
						continue;
					}

					/* find PC if not already known */
					if ((res_frame->pcLocation.location ==
					     LOCATION_UNKNOWN)
					    &&
					    (((travInst & MASK_SWLW_OPCODE) ==
					      INST_RESTORE_RA_32)
					     || ((travInst & MASK_SWLW_OPCODE)
						 == INST_RESTORE_RA_64))) {
						offset =
						    travInst & MASK_SWLW_OFFSET;
						location_set_reg_indirect
						    (&res_frame->pcLocation,
						     BT_REG_SP, offset);

						/* PC location is affected by register size */
						if ((travInst &
						     MASK_SWLW_OPCODE) ==
						    INST_RESTORE_RA_32) {
							d2printf
							    ("    %p: lw ra,0x%lx(sp)\n",
							     pcCur, offset);
						} else {
							/* MIPS64: we need to offset by a 32-bit register
							 * width to get the least significant 32 bits of
							 * the address because VxWorks currently supports
							 * only 32-bit addresses.
							 */
							res_frame->pcLocation.offset +=
							    REGISTER_OFFSET;
							d2printf
							    ("    %p: ld ra,0x%lx(sp)\n",
							     pcCur,
							     res_frame->
							     pcLocation.offset);
						}
					}

					/* addiu sp,sp,offset: Found what we're searching for.
					 * If positive offset, we've found SP offset.
					 * If negative offset, we've found our own prolog.
					 */
					else if ((res_frame->spLocation.
						  location == LOCATION_UNKNOWN)
						 &&
						 ((travInst & MASK_ADD_OPCODE)
						  == INST_ADD_TO_SP)) {
						tInst prevTravInst;

						if (!PCOk
						    (pcCur - j - 1,
						     client_arg)) {
							d1printf
							    (" %p: bogus PC! [2]; reversing...\n",
							     pcCur - j - 1);
							reverseOperation = true;
							break;
						}

						prevTravInst =
						    *(tInst *) instruction(pcCur - j - 1,
									   client_arg);

						if ((prevTravInst != INST_RTS)) {
							if (!fpBeingUsed) {
								res_frame->
								    spLocation.
								    location =
								    LOCATION_REG_OFFSET;
								res_frame->
								    spLocation.
								    reg =
								    BT_REG_SP;
							}
							res_frame->spLocation.
							    offset +=
							    IMM_TO_INT
							    (travInst);
							d2printf
							    ("    %p: Found restore stack: %ld\n",
							     pcCur - j - 1,
							     res_frame->
							     spLocation.offset);
							/* break; */
						}
					}

					else if ((res_frame->pcLocation.
						  location != LOCATION_UNKNOWN)
						 && (res_frame->spLocation.
						     location !=
						     LOCATION_UNKNOWN)) {
						break;
					}
				}

				if (reverseOperation == true)
					break;

			}

			/* if "next instr == stack pop" */
			/* if we have not found PC location yet, assume it's
			 * in register
			 */
			if (res_frame->pcLocation.location == LOCATION_UNKNOWN) {
				d2printf("    assuming pc in reg\n");
				location_set_reg_off(&res_frame->pcLocation,
						     BT_REG_LR, 0);

				/* same assumption for SP */
				if (res_frame->spLocation.location ==
				    LOCATION_UNKNOWN) {
					d2printf("    assuming sp in reg\n");
					location_set_reg_off(&res_frame->
							     spLocation,
							     BT_REG_SP, 0);
				}
			}

			return true;
		}

		/*----
		 * Next, look for instructions that indicate we're in the
		 * prologue of either this or some other function:
		 *   addiu sp,sp,[negative amount] <-- already got this one
		 *   sw    ra,offset(sp)
		 */
		if ((curInst & MASK_SWLW_OPCODE) == INST_STORE_RA_32 ||
		    (curInst & MASK_SWLW_OPCODE) == INST_STORE_RA_64) {
			if (i < MAX_PROLOGUE_LENGTH) {
				offset = curInst & MASK_SWLW_OFFSET;
				if ((offset & MASK_NEG_OFFSET) != 0)
					offset |= ~MASK_ADD_OFFSET;
				location_set_reg_off(&res_frame->pcLocation,
						     BT_REG_LR, 0);

				/* MIPS64 can use both, and we want to make sure
				 * that we find the right one
				 */
				if ((curInst & MASK_SWLW_OPCODE) ==
				    INST_STORE_RA_64) {
					d2printf("  %p: sd ra,%ld(sp)\n", pcCur,
						 offset);
				} else {
					d2printf("  %p: sw ra,%ld(sp)\n", pcCur,
						 offset);
				}

				/* Search upward for the 'addiu sp, sp, offset'
				 * stack creation operation
				 */
				for (j = 1; j <= MAX_PROLOGUE_LENGTH; j++) {
					tInst travInst;

					if (!PCOk(pcCur - j, client_arg)) {
						d1printf
						    (" %p: bogus PC! [3]; reversing...\n",
						     pcCur - j);
						reverseOperation = true;
						break;
					}

					travInst =
					    *(tInst *) instruction(pcCur - j,
								   client_arg);

					/*--- move fp,sp  [move s8,sp]
					 * This signals that the fp is being used in this routine
					 * and we must use it also, but only if we didn't start
					 * on this instruction.  If this instruction has not yet
					 * executed, the SP and PC offsets and locations are as
					 * we find them.  But, if the instruction has executed,
					 * the SP will have an absolute location and its offset
					 * will be adjusted by any addiu value.
					 */
					if ((travInst == INST_SP_TO_FP)
					    && ((i - j) != 0)) {
						fpBeingUsed = true;
						location_set_reg_off
						    (&res_frame->spLocation,
						     BT_REG_FP, 0);
						d2printf
						    ("    %p: move fp,sp;  fp being used\n",
						     pcCur - j);
					}

					/*--- sw  fp,+d(sp)
					 * Storing of stack frame pointer register on stack.
					 * If we started here, fp is not yet on stack.
					 */
					if (((travInst & MASK_SWLW_FP_SP) ==
					     INST_SW_FP_SP) && ((i - j) != 0)) {
						/* we may already have seen this */
						if (is_location_unknown_unused
						    (&res_frame->fpLocation)) {
							location_set_reg_indirect
							    (&res_frame->
							     fpLocation,
							     BT_REG_SP,
							     travInst &
							     MASK_SWLW_OFFSET);
							d2printf
							    ("    %p: sw fp,%ld(sp)\n",
							     pcCur - j,
							     res_frame->
							     fpLocation.offset);
						}
						continue;
					}

					if ((travInst & MASK_ADD_OPCODE) ==
					    INST_ADD_TO_SP) {
						/* A positive addition to the stack pointer is popping
						 * something off of the stack and is not a part of the
						 * prologue which is setting the stack up.
						 * Stop scanning backwards and try finding the epilogue */
						offset = IMM_TO_INT(travInst);
						if (!
						    (travInst &
						     MASK_NEG_OFFSET)) {
							d2printf
							    ("    %p: addiu sp,sp,+%ld; not in prologue\n",
							     pcCur - j, offset);
							break;
						}

						if (!fpBeingUsed) {
							res_frame->spLocation.
							    location =
							    LOCATION_REG_OFFSET;
							res_frame->spLocation.
							    reg = BT_REG_SP;
						}

						/* Negative offset now is just adding later */
						res_frame->spLocation.offset +=
						    -offset;
						d2printf
						    ("    %p: addiu sp,sp,-%ld; done[1]\n",
						     pcCur - j, -offset);

						return true;
					}
				}

				/* couldn't find the stack pointer; try reverse processing */
				d2printf
				    ("    can't find addiu sp,sp,offset; reversing...\n");
				reverseOperation = true;
				break;
			}
		}

		/*--- addiu sp,sp,offset
		 */
		if ((curInst & MASK_ADD_OPCODE) == INST_ADD_TO_SP) {
			tInst prevInst, nextInst;

			offset = IMM_TO_INT(curInst);

			/* if the immediate value is negative */
			if (offset < 0) {
				offset = -offset;
				d2printf("  %p: addiu sp,sp,-%ld\n", pcCur,
					 offset);
				if (i < MAX_PROLOGUE_LENGTH) {
					d2printf("    epilogue entry @ %p\n",
						 pcCur);
					location_set_reg_off(&res_frame->
							     pcLocation,
							     BT_REG_LR, 0);
					location_set_reg_off(&res_frame->
							     spLocation,
							     BT_REG_SP, 0);
				}

				return true;
			}

			d2printf("  %p: addiu sp,sp,+%ld\n", pcCur, offset);

			/*
			 * Test for a tail continued function; one that restores
			 * the stack, then jumps directly to another function,
			 * without linking, instead of returning to the caller.
			 * If we find this, then all stack info is known and we
			 * don't have to follow the jump/branch.
			 *
			 *  ...
			 *   b              0xc008216c  [unconditional branch]
			 *   addiu          sp,sp,48    [stack restore in branch delay slot]
			 *
			 * In some cases, the compiler will not optimize the
			 * stack pop into the tail-continued functions jump
			 * delay-slot,
			 *
			 *   lw         ra,20(sp)
			 *          (...)
			 *   addiu      sp,sp,48
			 *   j          intUnlock
			 *   nop
			 *
			 * so we should check the next instruction for a jump
			 * as well.
			 */
			if (!PCOk(pcCur - 1, client_arg)) {
				d1printf
				    (" %p: bogus address [5]!  Reversing...\n",
				     pcCur - 1);
				reverseOperation = true;
				break;
			}
			if (!PCOk(pcCur + 1, client_arg)) {
				d1printf
				    (" %p: bogus address [6]!  Reversing...\n",
				     pcCur + 1);
				reverseOperation = true;
				break;
			}

			prevInst =
			    *(tInst *) instruction(pcCur - 1, client_arg);
			nextInst =
			    *(tInst *) instruction(pcCur + 1, client_arg);

			if (((prevInst & MASK_JUMP_OPCODE) == INST_J) ||
			    ((nextInst & MASK_JUMP_OPCODE) == INST_J) ||
			    ((prevInst & MASK_BRANCH_OPCODE) == INST_B) ||
			    ((nextInst & MASK_BRANCH_OPCODE) == INST_B)) {
				const tInst *pcTrav;
				tInst travInst;

				d2printf("  tail-continue @ %p\n", pcCur);
				if (!fpBeingUsed) {
					res_frame->spLocation.location =
					    LOCATION_REG_OFFSET;
					res_frame->spLocation.reg = BT_REG_SP;
				}
				res_frame->spLocation.offset += offset;

				/* Now we need to locate the return address.
				 * Scan backwards looking for a load from stack,
				 * otherwise assume its in a register.
				 */
				for (j = 0; j < MAX_EPILOGUE_LENGTH; ++j) {
					pcTrav = pcCur - 1 - j;
					if (!PCOk(pcTrav, client_arg)) {
						d1printf
						    (" %p: bogus PC! [7]; reversing...\n",
						     pcTrav);
						reverseOperation = true;
						break;
					}

					travInst =
					    *(tInst *) instruction(pcTrav,
								   client_arg);

					if ((travInst & MASK_SWLW_OPCODE) ==
					    INST_RESTORE_RA_32
					    || (travInst & MASK_SWLW_OPCODE) ==
					    INST_RESTORE_RA_64) {
						offset = IMM_TO_INT(travInst);

						/*
						 * MIPS64 can use both, and we want to make sure
						 * that we find the right one
						 */
						if ((travInst &
						     MASK_SWLW_OPCODE) ==
						    INST_STORE_RA_64) {
							res_frame->pcLocation.
							    offset =
							    REGISTER_OFFSET +
							    offset;
							d2printf
							    ("    %p: sd ra,%ld(sp)\n",
							     pcTrav,
							     res_frame->
							     pcLocation.offset);
						} else {
							res_frame->pcLocation.
							    offset = offset;
							d2printf
							    ("    %p: sw ra,%ld(sp)\n",
							     pcTrav, offset);
						}
						res_frame->pcLocation.location =
						    LOCATION_REG_INDIRECT;
						res_frame->pcLocation.reg =
						    BT_REG_SP;

						return true;
					}

					/* Some tail-continued functions are actually shorter than
					 * other functions' epilogues.  This can cause us to
					 * inadvertantly walk back into the previous function's
					 * epilogue, and mistakenly pick up its link register
					 * save instruction (see _d_sub on malta24kbe/32 bit/Vx6.1
					 * bsp for an example).  To solve this problem, we look
					 * for a stack reservation that will signal one of two
					 * things: either the prologue of the current function,
					 * or that we've exited the epilogue of the current
					 * function (i.e., the return address is _definately_ in
					 * ra at this point).
					 */
					if (((travInst & MASK_ADD_OPCODE) ==
					     INST_ADD_TO_SP)
					    && ((travInst & MASK_NEG_OFFSET) !=
						0)) {
						d2printf
						    ("    left epilogue @ %p\n",
						     pcTrav);
						break;
					}
				}	/* for (1 to epilogue_length) */

				if (reverseOperation == true)
					break;

				/* didn't find ra on the stack,
				 * assume it's in the register */
				location_set_reg_off(&res_frame->pcLocation,
						     BT_REG_LR, 0);

				return true;
			}
			/* if "previous or next instruction is a jump" */
			if (offset >= 0) {
				if (!fpBeingUsed) {
					res_frame->spLocation.location =
					    LOCATION_REG_OFFSET;
					res_frame->spLocation.reg = BT_REG_SP;
				}
				res_frame->spLocation.offset += offset;
				d2printf("    adjusting sp offset by 0x%lx\n",
					 offset);
			}
		}

		/*----
		 * Lastly, look for branch statements
		 * The B[FT]S versions have delays, but we skip them because
		 * we assume they are never interesting.  We were wrong.
		 * For example, strncmp puts the FP_TO_SP instruction in the
		 * delay slot. Fix this by not taking a branch until the
		 * instruction after it has been analyzed.
		 */

		/* -1 ensures that we check the delay slot first - this can't
		 * be used if we haven't processed at least one instruction!
		 */
		if (i >= 1) {
			tInst prevInst;

			if (!PCOk(pcCur - 1, client_arg)) {
				d1printf(" %p: bogus PC! [%u]; reversing...\n",
					 pcCur - 1, __LINE__);
				reverseOperation = true;
				break;
			}

			prevInst =
			    *(tInst *) instruction(pcCur - 1, client_arg);

			/* Test for a branch to an arbitrary register.  We can
			 * track only a certain type.  If it isn't that case,
			 * we should give up.
			 * The case we look for is a computed go-to:
			 *
			 *  ...
			 *  lui     t9,0xc00c    [load upper 16 bits of address]
			 *  addiu   t9,t9,18488  [load lower 16 bits of address]
			 *  addu    v1,t9,v1     [add offset into list]
			 *  jr      v1           [go to computed address]
			 *  nop
			 *  b       func1        [list[0]]  follow this branch
			 *  nop
			 *  b       func2        [list[1]]
			 *  nop
			 *  ...
			 *
			 * In this case we use a default offset of zero,
			 * selecting the first address in the list.
			 */
			if ((prevInst & MASK_JR_REG) == INST_JR_REG) {
				tInst nextInst[2];	/* pcCur +1, pcCur +2 */
				tInst localPrevInst[3];	/* pcCur [-2, -3, -4] */

				if (PCOk(pcCur - 4, client_arg) ||
				    PCOk(pcCur + 2, client_arg)) {
					/* reverse operation */
					d2printf
					    ("  %p: discovered jump to register - reverse op\n",
					     pcCur - 1);
					reverseOperation = true;
					break;
				}

				nextInst[0] =
				    *(tInst *) instruction(pcCur + 1,
							   client_arg);
				nextInst[1] =
				    *(tInst *) instruction(pcCur + 2,
							   client_arg);

				localPrevInst[0] =
				    *(tInst *) instruction(pcCur - 2,
							   client_arg);
				localPrevInst[1] =
				    *(tInst *) instruction(pcCur - 3,
							   client_arg);
				localPrevInst[2] =
				    *(tInst *) instruction(pcCur - 4,
							   client_arg);

				if (((localPrevInst[0] & MASK_BASIC_OPCODE) ==
				     INST_ADDU)
				    && ((localPrevInst[1] & MASK_OPCODE) ==
					INST_ADDIU)
				    && ((localPrevInst[2] & MASK_OPCODE) ==
					INST_LUI) && (curInst == INST_NOP)
				    && ((nextInst[0] & MASK_BRANCH_OPCODE) ==
					INST_B) && (nextInst[1] == INST_NOP)) {
					const tInst *pc = pcCur + 2;
					const rOffset offset =
					    nextInst[0] & MASK_BRANCH_OFFSET;
					d2printf
					    ("  %p: computed go-to; following to %p\n",
					     pcCur - 1, pc + offset);

					pcCurStart = pc + offset - 1;
					i = 0;
					continue;
				}
			}

			if ((prevInst & MASK_BRANCH_OPCODE) == INST_BEQ ||
			    (prevInst & MASK_BRANCH_OPCODE) == INST_BNE ||
			    (prevInst & MASK_BRANCH_OPCODE) == INST_BLEZ ||
			    (prevInst & MASK_BRANCH_OPCODE) == INST_BGTZ ||
			    (prevInst & MASK_B_OPCODE) == INST_BPOSGE32) {
				const rOffset offset =
				    *(tInst *) instruction(pcCur - 1,
							   client_arg)
				    & MASK_BRANCH_OFFSET;

				/* Ignore negative test branches */
				if (!(offset & 0x8000) && offset != 0) {
					/* const tInst * pc = pcCur + 1; */
					uint32_t nextInst;
					d2printf
					    ("  %p: forward branch [delay] by 0x%04x to %p\n",
					     pcCur - 1, offset, pcCur + offset);

					if (!PCOk(pcCur + 1, client_arg))
						goto follow_branch;

					nextInst =
					    *(tInst *) instruction(pcCur + 1,
								   client_arg);

					/* trick caught in point8(): check next
					 * inst for possible epilogue inst
					 * and follow if found
					 */
					if ((nextInst == INST_RTS) ||
					    ((nextInst & MASK_SWLW_OPCODE) ==
					     INST_RESTORE_RA_32)
					    || ((nextInst & MASK_SWLW_OPCODE) ==
						INST_RESTORE_RA_64)
					    ||
					    (((nextInst & MASK_ADD_OPCODE) ==
					      INST_ADD_TO_SP)
					     &&
					     (!((nextInst & MASK_ADD_OFFSET) &
						0x8000)))) {
						d2printf
						    ("    ignoring branch, continuing\n");
						continue;
					}

follow_branch:
					/* no epilogue, so follow branch */
					pcCurStart = pcCur + offset - 1;
					i = 0;
					continue;
				}
			}

			/* Take all arbitrary branches - check delay slot */
			if ((prevInst & MASK_B_OPCODE) == INST_BEQ) {
				offset = IMM_TO_INT(prevInst);

				d2printf
				    ("  %p: arbitrary branch [delay] by %ld to %p\n",
				     pcCur - 1, offset, pcCur + offset);

				pcCurStart = pcCur + offset;
				i = 0;

				/* we limit the number of arbitrary branches taken */
				if (branchCount >= MAX_ARB_BRANCHES) {
					d1printf
					    ("    Too many branches![1] Reversing...\n");
					reverseOperation = true;
				} else {
					/* check for infinite loop */
					for (j = 0; j < branchCount; j++) {
						if (branchArray[j] ==
						    (pcCur - 1)) {
							d1printf
							    ("    Infinite loop![1] Reversing...\n");
							reverseOperation = true;
							break;
						}
					}
					branchArray[branchCount++] =
					    (pcCur - 1);
				}

				if (reverseOperation == true) {
					d2printf
					    ("  %p: branch loop - reverse operation\n",
					     pcCur - 1);
					break;
				}

				continue;
			}

			if ((prevInst & MASK_JUMP_OPCODE) == INST_J) {
				/* Offset is actually a `target': it is
				 * bit-shifted two left to align on a byte
				 * boundary [28 bits] and then the upper
				 * 4/36 bits are taken from the top of the
				 * current 32-/64-bit address.
				 */
				tAddr offset =
				    (tAddr) (prevInst & MASK_JUMP_OFFSET);
				offset <<= 2;
				offset |= ((tAddr) (pcCur - 1) & 0xF0000000);

				if (offset != 0) {
					d2printf
					    ("  %p: arbitrary branch to 0x%lx\n",
					     pcCur - 1, offset);
					/* Set for arbitrary branch */
					i = 0;
					pcCurStart = (tInst *) offset - 1;

					/* we limit the number of arbitrary branches taken */
					if (branchCount >= MAX_ARB_BRANCHES) {
						d1printf
						    ("    Too many branches![2] Reversing...\n");
						reverseOperation = true;
					} else {
						/* check for infinite loop */
						for (j = 0; j < branchCount;
						     j++) {
							if (branchArray[j] ==
							    (pcCur - 1)) {
								d1printf
								    ("    Infinite loop![2] Reversing...\n");
								reverseOperation
								    = true;
								break;
							}
						}
						branchArray[branchCount++] =
						    (pcCur - 1);
					}
					if (reverseOperation == true) {
						d2printf
						    ("  branch loop - reverse operation\n");
						break;
					} else {
						continue;
					}
				}
			}
		}
		/* if >= 1 instruction */
	}			/* ...for ( < RTITOOLS_CONTEXT_MAX_FN_LENGTH) */

	/*
	 * Reset search to start from the original PC and search backwards,
	 * looking for the prolog and assuming nothing is known.
	 */

	if (reverseOperation) {
		init_ContextFrame(res_frame);
		/* most clauses don't use the frame pointer, so by default we
		 * mark it as untracked for the current frame.
		 */
		res_frame->fpLocation.location = LOCATION_UNUSED;

		pcCurStart = startpc;
		d2printf("< %p: Begin reverse search\n", pcCurStart);

		for (i = 0; i < RTITOOLS_CONTEXT_MAX_FN_LENGTH; i++) {
			pcCur = pcCurStart - i;
			if (!PCOk(pcCur, client_arg)) {
				d1printf(" %p: invalid PC!  REV2 loop #%u\n",
					 pcCur, i);
				return false;
			}

			curInst = *(tInst *) instruction(pcCur, client_arg);

			d3printf(" < %p: examining instruction 0x%08x\n", pcCur,
				 curInst);

			/*----
			 * Look for a function prologue [remember, we're moving
			 * backwards]:
			 *
			 * [some other function's jr ra]
			 * addiu sp,sp,-offset
			 * sw    ra,+d(sp)
			 * sw    fp,+d(sp)      [optional fp support]
			 * move  fp,sp          [   "     "     "   ]
			 * ...
			 */

			/*--- move fp,sp  [move s8,sp]
			 * This signals that the fp is being used in this
			 * routine and we must use it also, but only if we
			 * didn't start on this instruction.
			 * If this instruction has not yet executed, the SP
			 * and PC offsets and locations are as we find them.
			 * But, if the instruction has executed, the SP will
			 * have an absolute location and its offset will be
			 * adjusted by any addiu value.
			 */
			if ((curInst == INST_SP_TO_FP) && (i > 0)) {
				fpBeingUsed = true;
				location_set_reg_off(&res_frame->spLocation,
						     BT_REG_FP, 0);
				d2printf("  < %p: move fp,sp;  fp being used\n",
					 pcCur);
			}

			/*--- sw  fp,+d(sp)
			 * Storing of stack frame pointer register on stack.
			 * If we started here, fp is not yet on stack.
			 */
			if (((curInst & MASK_SWLW_FP_SP) == INST_SW_FP_SP)
			    && (i > 0)) {
				/* we may already have seen this */
				if (is_location_unknown_unused
				    (&res_frame->fpLocation)) {
					location_set_reg_indirect(&res_frame->
								  fpLocation,
								  BT_REG_SP,
								  curInst &
								  MASK_SWLW_OFFSET);
					d2printf("  < %p: sw fp,%ld(sp)\n",
						 pcCur,
						 res_frame->fpLocation.offset);
				}
				continue;
			}

			/*--- SW  ra,offset(sp)
			 *    SD  ra,offset(sp)
			 */
			if ((curInst & MASK_SWLW_OPCODE) == INST_STORE_RA_32 ||
			    (curInst & MASK_SWLW_OPCODE) == INST_STORE_RA_64) {
				res_frame->pcLocation.location =
				    LOCATION_REG_INDIRECT;
				res_frame->pcLocation.reg = BT_REG_SP;

				/* MIPS64 can use both, and we want to make sure
				 * that we find the right one
				 */
				offset = curInst & MASK_SWLW_OFFSET;
				if ((curInst & MASK_SWLW_OPCODE) ==
				    INST_STORE_RA_64) {
					d2printf("  < %p: sd ra,%ld(sp)\n",
						 pcCur, offset);
					res_frame->pcLocation.offset =
					    REGISTER_OFFSET + offset;
				} else {
					d2printf("  < %p: sw ra,%ld(sp)\n",
						 pcCur, offset);
					res_frame->pcLocation.offset = offset;
				}

				if (res_frame->spLocation.location ==
				    LOCATION_UNKNOWN)
					res_frame->spLocation.offset = 0;

				/* Search backward to learn about sp restoration
				 * and start of function */
				for (j = 1; j < MAX_EPILOGUE_LENGTH; j++) {
					tInst travInst;

					if (!PCOk(pcCur - j, client_arg)) {
						d1printf
						    (" < %p: bogus PC! [1]\n",
						     pcCur - j);
						return false;
					}

					travInst =
					    *(tInst *) instruction(pcCur - j,
								   client_arg);
					/*--- sw  fp,+d(sp)
					 * Storing of stack frame pointer register on stack.
					 * If we started here, fp is not yet on stack.
					 */
					if ((travInst & MASK_SWLW_FP_SP) ==
					    INST_SW_FP_SP) {
						/* we may already have seen this */
						if (is_location_unknown_unused
						    (&res_frame->fpLocation)) {
							location_set_reg_indirect
							    (&res_frame->
							     fpLocation,
							     BT_REG_SP,
							     travInst &
							     MASK_SWLW_OFFSET);
							d2printf
							    ("    < %p: sw fp,%ld(sp)\n",
							     pcCur - j,
							     res_frame->
							     fpLocation.offset);
						}
						continue;
					}

					if ((travInst & MASK_ADD_OPCODE) ==
					    INST_ADD_TO_SP) {
						offset = IMM_TO_INT(travInst);
						if (offset > 0) {
							/* If we encounter an ADD_TO_SP > 0 opcode, we
							 * are in the epilogue of some other function
							 * -- oops
							 */
							d2printf
							    ("    < %p: addiu sp,sp,+%ld: underran "
							     "function - ending\n",
							     pcCur - j, offset);
						} else {
							offset = -offset;

							/* sp location depends on fp use */
							if (!fpBeingUsed) {
								res_frame->
								    spLocation.
								    location =
								    LOCATION_REG_OFFSET;
								res_frame->
								    spLocation.
								    reg =
								    BT_REG_SP;
							}

							res_frame->spLocation.
							    offset += offset;
							d2printf
							    ("    < %p: addiu sp,sp,-%ld; done\n",
							     pcCur - j, offset);
						}

						break;
					}
				}

				return true;
			}

			/* if STORE_RA */
			/*--- ADDIU sp,sp,imm
			 * If we encounter this one first, the ra, sp, and fp
			 * are still in their registers, if the fp is being
			 * used at all [should only happen if we start in the
			 * prologue]
			 */
			if ((curInst & MASK_ADD_OPCODE) == INST_ADD_TO_SP) {
				offset = IMM_TO_INT(curInst);

				if (offset > 0) {
					/* found a stack pop instead of a
					 * push (positive modification)
					 */
					d2printf("  < %p: addiu sp,sp,+%ld\n",
						 pcCur, offset);
					if (!gprs_available) {
						d2printf
						    ("    underran function - ending\n");
						return false;
					}

					/* Forward and backward search has failed in leaf
					 * function, so it is likely that we are in spaghetti
					 * code like that written for VxWorks semaphore support
					 * and the regs still have the values we seek.
					 */
					if (res_frame->spLocation.location ==
					    LOCATION_UNKNOWN)
						location_set_reg_off
						    (&res_frame->spLocation,
						     BT_REG_SP, 0);
					return true;
				}

				/* Done: found a stack push;
				 * offset is negative, so negate. */
				offset = -offset;

				/* sp location depends on fp use */
				if (!fpBeingUsed) {
					res_frame->spLocation.location =
					    LOCATION_REG_OFFSET;
					res_frame->spLocation.reg = BT_REG_SP;
				}

				res_frame->spLocation.offset += offset;
				d2printf("  < %p: addiu sp,sp,-%ld\n", pcCur,
					 offset);

				if (gprs_available
				    && (res_frame->pcLocation.location ==
					LOCATION_UNKNOWN)) {
					location_set_reg_off(&res_frame->
							     pcLocation,
							     BT_REG_LR, 0);
				}

				return true;
			}

			/*----
			 * We're going backwards so, if we see any of these
			 * things, we have overrun our current function:
			 *   MOVE   sp,fp           :fp to sp
			 *   LW/LD  ra,offset(sp)   :restore ra
			 *   JR     ra              :rts
			 *   [restore fp] <-- we should never end up at a delay
			 *                    slot..that would be weird
			 */
			if ((curInst & MASK_SWLW_OPCODE) == INST_RESTORE_RA_32
			    || (curInst & MASK_SWLW_OPCODE) ==
			    INST_RESTORE_RA_64 || (curInst == INST_RTS)) {
				tInst brInst, prevInst;

				if ((curInst & MASK_SWLW_OPCODE) ==
				    INST_RESTORE_RA_32) {
					d2printf("  < %p: lw ra,%d(sp) !\n",
						 pcCur,
						 curInst & MASK_SWLW_OFFSET);
				} else if ((curInst & MASK_SWLW_OPCODE) ==
					   INST_RESTORE_RA_64) {
					d2printf("  < %p: ld ra,%d(sp) !\n",
						 pcCur,
						 curInst & MASK_SWLW_OFFSET);
				} else {
					d2printf("  < %p: jr ra !\n", pcCur);
				}

				/* Special case:
				 * lw/ld ra,x(sp) in branch delay slot.
				 * As seen in the C line editor loop (private
				 * routine following viLedLibInit), taking this
				 * branch will take us to the epilogue.
				 */
				if (!PCOk(pcCur - 1, client_arg)) {
					d1printf
					    (" %p: REV bogus address [3]!\n",
					     pcCur - 1);
					return false;
				}

				prevInst =
				    *(tInst *) instruction(pcCur - 1,
							   client_arg);
				brInst = prevInst & MASK_BRANCH_OPCODE;

				if ((((prevInst & MASK_B_OPCODE) == INST_BEQ) ||
				     (brInst == INST_BEQ) ||
				     (brInst == INST_BNE) ||
				     (brInst == INST_BLEZ) ||
				     (brInst == INST_BGTZ) ||
				     (brInst == INST_BPOSGE32)) &&
				    (((curInst & MASK_SWLW_OPCODE) ==
				      INST_RESTORE_RA_32)
				     || ((curInst & MASK_SWLW_OPCODE) ==
					 INST_RESTORE_RA_64))) {
					offset = IMM_TO_INT(prevInst);

					reverseOperation = false;
					pcCurStart = pcCur + offset;
					i = 0;
					offset = curInst & MASK_SWLW_OFFSET;
					location_set_reg_indirect(&res_frame->
								  pcLocation,
								  BT_REG_SP,
								  offset);

					if ((curInst & MASK_SWLW_OPCODE) ==
					    INST_RESTORE_RA_64) {
						res_frame->pcLocation.offset += 4;	/* only 32-bit address */
						d2printf
						    ("    %p: branch to %p followed by ld ra,%ld"
						     "(sp)\n", pcCur - 1,
						     pcCurStart, offset);
					} else {
						d2printf
						    ("    %p: branch to %p followed by lw ra,%ld"
						     "(sp)\n", pcCur - 1,
						     pcCurStart, offset);
					}
					d2printf
					    ("      follow branch, resume forward search\n");
					goto resumeFwdSearch;
				}

				/* Forward and backward search has failed in
				 * NON-LEAF function, so we quit here.
				 */
				if (!gprs_available) {
					d2printf
					    ("    %p: underran non-leaf function - ending\n",
					     pcCur);
				}

				/* Forward and backward search has failed in
				 * LEAF function, so it is likely that we are
				 * in spaghetti code like that written for
				 * VxWorks semaphore support and the regs still
				 * have the values we seek.
				 */
				else {
					if (res_frame->spLocation.location ==
					    LOCATION_UNKNOWN)
						location_set_reg_off
						    (&res_frame->spLocation,
						     BT_REG_SP, 0);
					if (res_frame->pcLocation.location ==
					    LOCATION_UNKNOWN)
						location_set_reg_off
						    (&res_frame->pcLocation,
						     BT_REG_PC, 0);
					d2printf
					    ("    %p: underran leaf function - ending\n",
					     pcCur);
				}

				return true;
			}
		}		/* ...for loop */
	}
	/* reverse search */
	d2printf("  ** %p: Bottomed out; i = %u **\n", pcCur, i);
	/* Still here?  Something's wrong.. give up. */

	res_frame->spLocation.location = LOCATION_UNKNOWN;
	res_frame->pcLocation.location = LOCATION_UNKNOWN;

	return false;
}
