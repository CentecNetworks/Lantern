/*
 * Simple /proc interface to the Octeon Performance Counters
 *
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 2004-2007 Cavium Networks
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <asm/octeon/cvmx.h>
#include <asm/octeon/cvmx-core.h>
#include <asm/octeon/cvmx-l2c.h>
#include <asm/octeon/cvmx-l2c-defs.h>
#include <asm/octeon/cvmx-lmcx-defs.h>

/**
 * Module parameters used to control the counters. Can be
 * changed on the fly through sysfs.
 */
static char counter0[32] = "sissue";
static char counter1[32] = "dissue";
module_param_string(counter0, counter0, sizeof(counter0), 0644);
module_param_string(counter1, counter1, sizeof(counter1), 0644);

static char l2counter0[32] = "imiss";
static char l2counter1[32] = "ihit";
static char l2counter2[32] = "dmiss";
static char l2counter3[32] = "dhit";
module_param_string(l2counter0, l2counter0, sizeof(l2counter0), 0644);
module_param_string(l2counter1, l2counter1, sizeof(l2counter1), 0644);
module_param_string(l2counter2, l2counter2, sizeof(l2counter2), 0644);
module_param_string(l2counter3, l2counter3, sizeof(l2counter3), 0644);

static struct proc_dir_entry *proc_perf_entry;
static uint64_t proc_perf_counter_control[2];
static uint64_t proc_perf_counter_data[NR_CPUS][2];
static uint64_t proc_perf_l2counter_control[4];
static uint64_t proc_perf_l2counter_data[4];
static const char *proc_perf_label[CVMX_CORE_PERF_MAX];
static const char *proc_perf_l2label[CVMX_L2C_EVENT_MAX];
static uint64_t proc_perf_dram_clocks;
static uint64_t proc_perf_dram_operations;
static int proc_perf_in_use;
static uint64_t start_cycle, end_cycle;
static struct proc_perf_l2tad_label
{
	/* type of the event */
	cvmx_l2c_tad_event_t type;
	/* unique name of each event */
	const char *name;
	/* 
	 * Based on the type of the event, print the counter value 
	 * differently 
	 */
	int info;                    
} proc_perf_l2tad_label[] = { 
	{ CVMX_L2C_TAD_EVENT_NONE, "none", 0 },
	{ CVMX_L2C_TAD_EVENT_TAG_HIT, "hit", 0 },
	{ CVMX_L2C_TAD_EVENT_TAG_MISS, "miss", 0 },
	{ CVMX_L2C_TAD_EVENT_TAG_NOALLOC, "no-alloc", 0 },
	{ CVMX_L2C_TAD_EVENT_TAG_VICTIM, "victim", 0 },
	{ CVMX_L2C_TAD_EVENT_SC_FAIL, "sc-fail", 0 },
	{ CVMX_L2C_TAD_EVENT_SC_PASS, "sc-pass", 0 },
	{ CVMX_L2C_TAD_EVENT_LFB_VALID, "lfb-valid", 1 },
	{ CVMX_L2C_TAD_EVENT_LFB_WAIT_LFB, "lfb-wait-lfb", 1 },
	{ CVMX_L2C_TAD_EVENT_LFB_WAIT_VAB, "lfb-wait-vab", 1 },
	{ CVMX_L2C_TAD_EVENT_QUAD0_INDEX, "quad0-index", 2 },
	{ CVMX_L2C_TAD_EVENT_QUAD0_READ, "quad0-read", 2 },
	{ CVMX_L2C_TAD_EVENT_QUAD0_BANK, "quad0-bank", 1 },
	{ CVMX_L2C_TAD_EVENT_QUAD0_WDAT, "quad0-wdat", 2 },
	{ CVMX_L2C_TAD_EVENT_QUAD1_INDEX, "quad1-index", 2 },
	{ CVMX_L2C_TAD_EVENT_QUAD1_READ, "quad1-read", 2 },
	{ CVMX_L2C_TAD_EVENT_QUAD1_BANK, "quad1-bank", 1 },
	{ CVMX_L2C_TAD_EVENT_QUAD1_WDAT, "quad1-wdat", 2 },
	{ CVMX_L2C_TAD_EVENT_QUAD2_INDEX, "quad2-index", 2 },
	{ CVMX_L2C_TAD_EVENT_QUAD2_READ, "quad2-read", 2 },
	{ CVMX_L2C_TAD_EVENT_QUAD2_BANK, "quad2-bank", 1 },
	{ CVMX_L2C_TAD_EVENT_QUAD2_WDAT, "quad2-wdat", 2 },
	{ CVMX_L2C_TAD_EVENT_QUAD3_INDEX, "quad3-index", 2 },
	{ CVMX_L2C_TAD_EVENT_QUAD3_READ, "quad3-read", 2 },
	{ CVMX_L2C_TAD_EVENT_QUAD3_BANK, "quad3-bank", 1 },
	{ CVMX_L2C_TAD_EVENT_QUAD3_WDAT, "quad3-wdat", 2 },
	{ CVMX_L2C_TAD_EVENT_MAX, NULL, 0}
};

/**
 * Setup the core counters. Called on each core
 *
 * @param arg
 */
static void proc_perf_setup_counters(void *arg)
{
	cvmx_core_perf_control_t control;
	uint64_t cvmctl;

	if (proc_perf_in_use) {
		/*
		 * Disable the issue and exec conditional clock support so we 
		 * get better results
		 */
		cvmctl = __read_64bit_c0_register($9, 7);
		cvmctl |= 3 << 16;
		__write_64bit_c0_register($9, 7, cvmctl);
	}

	control.u32 = 0;
	control.s.event = proc_perf_counter_control[0];
	control.s.u = 1;
	control.s.s = 1;
	control.s.k = 1;
	control.s.ex = 1;
	__write_32bit_c0_register($25, 0, control.u32);

	control.s.event = proc_perf_counter_control[1];
	__write_32bit_c0_register($25, 2, control.u32);

	__write_32bit_c0_register($25, 1, 0);
	__write_32bit_c0_register($25, 3, 0);
}


/**
 * Update the counters for each core.
 *
 * @param arg
 */
static void proc_perf_update_counters(void *arg)
{
	int cpu = smp_processor_id();

	proc_perf_counter_data[cpu][0] = __read_64bit_c0_register($25, 1);
	proc_perf_counter_data[cpu][1] = __read_64bit_c0_register($25, 3);
	mb();
}


/**
 * Cleanup the input of sysfs
 *
 * @param str
 * @param len
 */
static inline void clean_string(char *str, int len)
{
	int i;
	for (i = 0; i < len; i++)
		if (str[i] <= 32)
			str[i] = 0;
}


/**
 * Setup the counters using the current config
 */
static void proc_perf_setup(void)
{
	int i;

	proc_perf_counter_control[0] = 0;
	proc_perf_counter_control[1] = 0;
	proc_perf_l2counter_control[0] = 0;
	proc_perf_l2counter_control[1] = 0;
	proc_perf_l2counter_control[2] = 0;
	proc_perf_l2counter_control[3] = 0;

	/* Cleanup junk on end of param strings */
	clean_string(counter0, sizeof(counter0));
	clean_string(counter1, sizeof(counter1));
	clean_string(l2counter0, sizeof(l2counter0));
	clean_string(l2counter1, sizeof(l2counter1));
	clean_string(l2counter2, sizeof(l2counter2));
	clean_string(l2counter3, sizeof(l2counter3));

	/* Set the core counters to match the string parameters */
	for (i = 0; i < CVMX_CORE_PERF_MAX; i++) {
		if (proc_perf_label[i]) {
			if (strcmp(proc_perf_label[i], counter0) == 0)
				proc_perf_counter_control[0] = i;
			if (strcmp(proc_perf_label[i], counter1) == 0)
				proc_perf_counter_control[1] = i;
		}
	}
	if (OCTEON_IS_MODEL(OCTEON_CN5XXX) || OCTEON_IS_MODEL(OCTEON_CN3XXX)) {
		if (proc_perf_counter_control[0] & ~0x3f) {
			cvmx_dprintf("WARNING: Invalid Core Performance "
			    "Counter0 Event(%s), resetting to 'none'.\n", 
			    proc_perf_label[proc_perf_counter_control[0]]);
			proc_perf_counter_control[0] = 0;
		} 
		if (proc_perf_counter_control[1] & ~0x3f) {
			cvmx_dprintf("WARNING: Invalid Core Performance " 
			    "Counter1 Event(%s), resetting to 'none'.\n", 
			    proc_perf_label[proc_perf_counter_control[1]]);
			proc_perf_counter_control[1] = 0;
		}
	}

	/* Set the L2 counters to match the string parameters */
	if (OCTEON_IS_MODEL(OCTEON_CN5XXX) || OCTEON_IS_MODEL(OCTEON_CN3XXX)) {
		for (i = 0; i < CVMX_L2C_EVENT_MAX; i++) {
			if (proc_perf_l2label[i]) {
				if (strcmp(proc_perf_l2label[i], l2counter0) 
					== 0)
					proc_perf_l2counter_control[0] = i;
				if (strcmp(proc_perf_l2label[i], l2counter1) 
					== 0)
					proc_perf_l2counter_control[1] = i;
				if (strcmp(proc_perf_l2label[i], l2counter2) 
					== 0)
					proc_perf_l2counter_control[2] = i;
				if (strcmp(proc_perf_l2label[i], l2counter3) 
					== 0)
					proc_perf_l2counter_control[3] = i;
			}
		}
	} else {
		for (i = 0; proc_perf_l2tad_label[i].name; i++) {
			if (strcmp(proc_perf_l2tad_label[i].name, l2counter0) 
					== 0)	
				proc_perf_l2counter_control[0] = i;
			if (strcmp(proc_perf_l2tad_label[i].name, l2counter1) 
					== 0)	
				proc_perf_l2counter_control[1] = i;
			if (strcmp(proc_perf_l2tad_label[i].name, l2counter2) 
					== 0)	
				proc_perf_l2counter_control[2] = i;
			if (strcmp(proc_perf_l2tad_label[i].name, l2counter3) 
					== 0)	
				proc_perf_l2counter_control[3] = i;
		}
	}

	/* Update strings to match final config */
	strcpy(counter0, proc_perf_label[proc_perf_counter_control[0]]);
	strcpy(counter1, proc_perf_label[proc_perf_counter_control[1]]);

	if (OCTEON_IS_MODEL(OCTEON_CN5XXX) || OCTEON_IS_MODEL(OCTEON_CN3XXX)) {
		strcpy(l2counter0, 
		       proc_perf_l2label[proc_perf_l2counter_control[0]]);
		strcpy(l2counter1, 
		       proc_perf_l2label[proc_perf_l2counter_control[1]]);
		strcpy(l2counter2, 
		       proc_perf_l2label[proc_perf_l2counter_control[2]]);
		strcpy(l2counter3, 
		       proc_perf_l2label[proc_perf_l2counter_control[3]]);
	} else {
		strcpy(l2counter0, 
		       proc_perf_l2tad_label[proc_perf_l2counter_control[0]].name);
		strcpy(l2counter1, 
		       proc_perf_l2tad_label[proc_perf_l2counter_control[1]].name);
		strcpy(l2counter2, 
		       proc_perf_l2tad_label[proc_perf_l2counter_control[2]].name);
		strcpy(l2counter3, 
		       proc_perf_l2tad_label[proc_perf_l2counter_control[3]].name);
	}

	on_each_cpu(proc_perf_setup_counters, NULL, 1);

	if (OCTEON_IS_MODEL(OCTEON_CN5XXX) || OCTEON_IS_MODEL(OCTEON_CN3XXX)) {
		cvmx_l2c_pfctl_t l2control;

		l2control.u64 = 0;
		l2control.s.cnt3ena = 1;
		l2control.s.cnt3clr = 1;
		l2control.s.cnt3sel = proc_perf_l2counter_control[3];
		l2control.s.cnt2ena = 1;
		l2control.s.cnt2clr = 1;
		l2control.s.cnt2sel = proc_perf_l2counter_control[2];
		l2control.s.cnt1ena = 1;
		l2control.s.cnt1clr = 1;
		l2control.s.cnt1sel = proc_perf_l2counter_control[1];
		l2control.s.cnt0ena = 1;
		l2control.s.cnt0clr = 1;
		l2control.s.cnt0sel = proc_perf_l2counter_control[0];

		cvmx_write_csr(CVMX_L2C_PFCTL, l2control.u64);
	} else {
		cvmx_l2c_tadx_prf_t l2c_tadx_prf;
		int tad;

		l2c_tadx_prf.u64 = 0;
		l2c_tadx_prf.s.cnt3sel = proc_perf_l2counter_control[3];
		l2c_tadx_prf.s.cnt2sel = proc_perf_l2counter_control[2];
		l2c_tadx_prf.s.cnt1sel = proc_perf_l2counter_control[1];
		l2c_tadx_prf.s.cnt0sel = proc_perf_l2counter_control[0];

		for (tad = 0; tad < CVMX_L2C_TADS; tad++)
			cvmx_write_csr(CVMX_L2C_TADX_PRF(tad), 
					   l2c_tadx_prf.u64);

		start_cycle = cvmx_read_csr(CVMX_IPD_CLK_COUNT);
	}
}


static void proc_perf_update(void)
{
	on_each_cpu(proc_perf_update_counters, NULL, 1);
	mb();
	
	if (OCTEON_IS_MODEL(OCTEON_CN5XXX) || OCTEON_IS_MODEL(OCTEON_CN3XXX)) {
		proc_perf_l2counter_data[0] = cvmx_read_csr(CVMX_L2C_PFC0);
		proc_perf_l2counter_data[1] = cvmx_read_csr(CVMX_L2C_PFC1);
		proc_perf_l2counter_data[2] = cvmx_read_csr(CVMX_L2C_PFC2);
		proc_perf_l2counter_data[3] = cvmx_read_csr(CVMX_L2C_PFC3);
	} else {
		int tad;
		proc_perf_l2counter_data[0] = 0;
		proc_perf_l2counter_data[1] = 0;
		proc_perf_l2counter_data[2] = 0;
		proc_perf_l2counter_data[3] = 0;
		for (tad = 0; tad < CVMX_L2C_TADS; tad++) {
			proc_perf_l2counter_data[0] += 
				cvmx_read_csr(CVMX_L2C_TADX_PFC0(tad));
			cvmx_write_csr(CVMX_L2C_TADX_PFC0(tad), 0);
			proc_perf_l2counter_data[1] += 
				cvmx_read_csr(CVMX_L2C_TADX_PFC1(tad));
			cvmx_write_csr(CVMX_L2C_TADX_PFC1(tad), 0);
			proc_perf_l2counter_data[2] += 
				cvmx_read_csr(CVMX_L2C_TADX_PFC2(tad));
			cvmx_write_csr(CVMX_L2C_TADX_PFC2(tad), 0);
			proc_perf_l2counter_data[3] += 
				cvmx_read_csr(CVMX_L2C_TADX_PFC3(tad));
			cvmx_write_csr(CVMX_L2C_TADX_PFC3(tad), 0);
		}
		end_cycle = cvmx_read_csr(CVMX_IPD_CLK_COUNT);
	}
}


/**
 * Show the counters to the user
 *
 * @param m
 * @param v
 * @return
 */
static int proc_perf_show(struct seq_file *m, void *v)
{
	int cpu;
	int i;
	uint64_t dram_clocks;
	uint64_t dram_operations;
	cvmx_core_perf_control_t control0;
	cvmx_core_perf_control_t control1;

	proc_perf_update();

	control0.u32 = __read_32bit_c0_register($25, 0);
	control1.u32 = __read_32bit_c0_register($25, 2);
	seq_printf(m, "       %16s %16s\n",
		   proc_perf_label[control0.s.event],
		   proc_perf_label[control1.s.event]);
	for (cpu = 0; cpu < NR_CPUS; cpu++) {
		if (cpu_online(cpu))
			seq_printf(m, "CPU%2d: %16llu %16llu\n", cpu,
				   (unsigned long long)
				   proc_perf_counter_data[cpu][0],
				   (unsigned long long)
				   proc_perf_counter_data[cpu][1]);
	}

	seq_printf(m, "\n");
	if (OCTEON_IS_MODEL(OCTEON_CN5XXX) || OCTEON_IS_MODEL(OCTEON_CN3XXX)) {
		for (i = 0; i < 4; i++)
			seq_printf(m, "%s: %llu\n",
			   proc_perf_l2label[proc_perf_l2counter_control[i]],
			   (unsigned long long) proc_perf_l2counter_data[i]);
	} else {
		uint64_t cycles_used = end_cycle - start_cycle;
		for (i = 0; i < 4; i++) {
			if (proc_perf_l2tad_label
					[proc_perf_l2counter_control[i]].info 
			     == 1)
				seq_printf(m, "%s: %llu, average: %6lu\n",
				   proc_perf_l2tad_label
					[proc_perf_l2counter_control[i]].name,
				   (unsigned long long) proc_perf_l2counter_data[i],
				   (long int)(proc_perf_l2counter_data[i]/ 
					(cycles_used * CVMX_L2C_TADS)));
			else if (proc_perf_l2tad_label
					[proc_perf_l2counter_control[i]].info 
				== 2)
				seq_printf(m, "%s bus utilization: %4lu%%\n",
				   proc_perf_l2tad_label
					[proc_perf_l2counter_control[i]].name,
				   (long int)((proc_perf_l2counter_data[i]*100)/
					(cycles_used * CVMX_L2C_TADS)));
			else
				seq_printf(m, "%s: %llu\n",
				   proc_perf_l2tad_label
					[proc_perf_l2counter_control[i]].name,
			  (unsigned long long) proc_perf_l2counter_data[i]);
		}
	}

	/* Compute DRAM utilization */
	if (OCTEON_IS_MODEL(OCTEON_CN5XXX) || OCTEON_IS_MODEL(OCTEON_CN3XXX)) {
		dram_operations =
			(cvmx_read_csr(CVMX_LMCX_OPS_CNT_HI(0)) << 32) |
			cvmx_read_csr(CVMX_LMCX_OPS_CNT_LO(0));
		dram_clocks =
			(cvmx_read_csr(CVMX_LMCX_DCLK_CNT_HI(0)) << 32) |
			cvmx_read_csr(CVMX_LMCX_DCLK_CNT_LO(0));
	} else {
		int tad;
		dram_operations = 0;
		dram_clocks = 0;
		for (tad=0; tad<CVMX_L2C_TADS; tad++) {
			dram_operations += cvmx_read_csr
					(CVMX_LMCX_OPS_CNT(tad));
			dram_clocks += cvmx_read_csr
					(CVMX_LMCX_DCLK_CNT(tad));
		}
	}
#ifndef _ABIO32
	if (dram_clocks > proc_perf_dram_clocks) {
		uint64_t delta_clocks = dram_clocks - proc_perf_dram_clocks;
		uint64_t delta_operations =
			dram_operations - proc_perf_dram_operations;
		uint64_t percent_x100 = 10000 * delta_operations / delta_clocks;
		seq_printf(m,
			   "\nDRAM ops count: %lu, dclk count: %lu, " 
			   "utilization: %lu.%02lu%%\n",
			   (long int)delta_operations, (long int)delta_clocks, 
			   (long int)percent_x100 / 100,
			   (long int)percent_x100 % 100);
	}
#endif
	proc_perf_dram_operations = dram_operations;
	proc_perf_dram_clocks = dram_clocks;

	seq_printf(m,
		   "\n"
		   "Configuration of the performance counters is controlled " 
		   "by writing \none of the following values to:\n"
		   "    /sys/module/perf_counters/parameters/counter{0,1}\n"
		   "    /sys/module/perf_counters/parameters/l2counter{0-3}\n"
		   "\n" "Possible CPU counters:");
	for (i = 0; i < CVMX_CORE_PERF_MAX; i++) {
		if ((i & 7) == 0)
			seq_printf(m, "\n    ");
		if (proc_perf_label[i])
			seq_printf(m, "%s ", proc_perf_label[i]);
	}

	seq_printf(m, "\nPossible L2 counters:");
	if (OCTEON_IS_MODEL(OCTEON_CN5XXX) || OCTEON_IS_MODEL(OCTEON_CN3XXX)) {
		for (i = 0; i < CVMX_L2C_EVENT_MAX; i++) {
			if ((i & 3) == 0)
				seq_printf(m, "\n    ");
			if (proc_perf_l2label[i])
				seq_printf(m, "%s ", proc_perf_l2label[i]);
		}
	} else {
		for (i = 0; proc_perf_l2tad_label[i].name; i++) {
			if ((i & 3) == 0)
				seq_printf(m, "\n    ");
			seq_printf(m, "%s ", proc_perf_l2tad_label[i].name);
		}
	}
		
	seq_printf(m,
		   "\nWarning: Counter configuration doesn't update till you " 
		    "access /proc/octeon_perf.\n");

	proc_perf_setup();
	return 0;
}


/**
 * /proc/octeon_perf was openned. Use the single_open iterator
 *
 * @param inode
 * @param file
 * @return
 */
static int proc_perf_open(struct inode *inode, struct file *file)
{
	proc_perf_in_use = 1;
	return single_open(file, proc_perf_show, NULL);
}


static struct file_operations proc_perf_operations = {
	.open = proc_perf_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};


/**
 * Module initialization
 *
 * @return
 */
static int __init proc_perf_init(void)
{
	pr_notice("/proc/octeon_perf: Octeon performace counter interface "
		  "loaded\n");

	memset(proc_perf_label, 0, sizeof(proc_perf_label));
	memset(proc_perf_l2label, 0, sizeof(proc_perf_l2label));

	proc_perf_label[CVMX_CORE_PERF_NONE] = "none";
	proc_perf_label[CVMX_CORE_PERF_CLK] = "clk";
	proc_perf_label[CVMX_CORE_PERF_ISSUE] = "issue";
	proc_perf_label[CVMX_CORE_PERF_RET] = "ret";
	proc_perf_label[CVMX_CORE_PERF_NISSUE] = "nissue";
	proc_perf_label[CVMX_CORE_PERF_SISSUE] = "sissue";
	proc_perf_label[CVMX_CORE_PERF_DISSUE] = "dissue";
	proc_perf_label[CVMX_CORE_PERF_IFI] = "ifi";
	proc_perf_label[CVMX_CORE_PERF_BR] = "br";
	proc_perf_label[CVMX_CORE_PERF_BRMIS] = "brmis";
	proc_perf_label[CVMX_CORE_PERF_J] = "j";
	proc_perf_label[CVMX_CORE_PERF_JMIS] = "jmis";
	proc_perf_label[CVMX_CORE_PERF_REPLAY] = "replay";
	proc_perf_label[CVMX_CORE_PERF_IUNA] = "iuna";
	proc_perf_label[CVMX_CORE_PERF_TRAP] = "trap";
	proc_perf_label[CVMX_CORE_PERF_UULOAD] = "uuload";
	proc_perf_label[CVMX_CORE_PERF_UUSTORE] = "uustore";
	proc_perf_label[CVMX_CORE_PERF_ULOAD] = "uload";
	proc_perf_label[CVMX_CORE_PERF_USTORE] = "ustore";
	proc_perf_label[CVMX_CORE_PERF_EC] = "ec";
	proc_perf_label[CVMX_CORE_PERF_MC] = "mc";
	proc_perf_label[CVMX_CORE_PERF_CC] = "cc";
	proc_perf_label[CVMX_CORE_PERF_CSRC] = "csrc";
	proc_perf_label[CVMX_CORE_PERF_CFETCH] = "cfetch";
	proc_perf_label[CVMX_CORE_PERF_CPREF] = "cpref";
	proc_perf_label[CVMX_CORE_PERF_ICA] = "ica";
	proc_perf_label[CVMX_CORE_PERF_II] = "ii";
	proc_perf_label[CVMX_CORE_PERF_IP] = "ip";
	proc_perf_label[CVMX_CORE_PERF_CIMISS] = "cimiss";
	proc_perf_label[CVMX_CORE_PERF_WBUF] = "wbuf";
	proc_perf_label[CVMX_CORE_PERF_WDAT] = "wdat";
	proc_perf_label[CVMX_CORE_PERF_WBUFLD] = "wbufld";
	proc_perf_label[CVMX_CORE_PERF_WBUFFL] = "wbuffl";
	proc_perf_label[CVMX_CORE_PERF_WBUFTR] = "wbuftr";
	proc_perf_label[CVMX_CORE_PERF_BADD] = "badd";
	proc_perf_label[CVMX_CORE_PERF_BADDL2] = "baddl2";
	proc_perf_label[CVMX_CORE_PERF_BFILL] = "bfill";
	proc_perf_label[CVMX_CORE_PERF_DDIDS] = "ddids";
	proc_perf_label[CVMX_CORE_PERF_IDIDS] = "idids";
	proc_perf_label[CVMX_CORE_PERF_DIDNA] = "didna";
	proc_perf_label[CVMX_CORE_PERF_LDS] = "lds";
	proc_perf_label[CVMX_CORE_PERF_LMLDS] = "lmlds";
	proc_perf_label[CVMX_CORE_PERF_IOLDS] = "iolds";
	proc_perf_label[CVMX_CORE_PERF_DMLDS] = "dmlds";
	proc_perf_label[CVMX_CORE_PERF_STS] = "sts";
	proc_perf_label[CVMX_CORE_PERF_LMSTS] = "lmsts";
	proc_perf_label[CVMX_CORE_PERF_IOSTS] = "iosts";
	proc_perf_label[CVMX_CORE_PERF_IOBDMA] = "iobdma";
	proc_perf_label[CVMX_CORE_PERF_DTLB] = "dtlb";
	proc_perf_label[CVMX_CORE_PERF_DTLBAD] = "dtlbad";
	proc_perf_label[CVMX_CORE_PERF_ITLB] = "itlb";
	proc_perf_label[CVMX_CORE_PERF_SYNC] = "sync";
	proc_perf_label[CVMX_CORE_PERF_SYNCIOB] = "synciob";
	proc_perf_label[CVMX_CORE_PERF_SYNCW] = "syncw";
	if (!OCTEON_IS_MODEL(OCTEON_CN5XXX) && !OCTEON_IS_MODEL(OCTEON_CN3XXX)) {
		proc_perf_label[CVMX_CORE_PERF_ERETMIS] = "eretmis";
		proc_perf_label[CVMX_CORE_PERF_LIKMIS] = "likmis";
		proc_perf_label[CVMX_CORE_PERF_HAZTR] = "hazard-trap";
	}

	if (OCTEON_IS_MODEL(OCTEON_CN5XXX) || OCTEON_IS_MODEL(OCTEON_CN3XXX))
	{
		proc_perf_l2label[CVMX_L2C_EVENT_CYCLES] = "cycles";
		proc_perf_l2label[CVMX_L2C_EVENT_INSTRUCTION_MISS] = "imiss";
		proc_perf_l2label[CVMX_L2C_EVENT_INSTRUCTION_HIT] = "ihit";
		proc_perf_l2label[CVMX_L2C_EVENT_DATA_MISS] = "dmiss";
		proc_perf_l2label[CVMX_L2C_EVENT_DATA_HIT] = "dhit";
		proc_perf_l2label[CVMX_L2C_EVENT_MISS] = "miss";
		proc_perf_l2label[CVMX_L2C_EVENT_HIT] = "hit";
		proc_perf_l2label[CVMX_L2C_EVENT_VICTIM_HIT] = 
			"victim-buffer-hit";
		proc_perf_l2label[CVMX_L2C_EVENT_INDEX_CONFLICT] =
			"lfb-nq-index-conflict";
		proc_perf_l2label[CVMX_L2C_EVENT_TAG_PROBE] = "tag-probe";
		proc_perf_l2label[CVMX_L2C_EVENT_TAG_UPDATE] = "tag-update";
		proc_perf_l2label[CVMX_L2C_EVENT_TAG_COMPLETE] =
			"tag-probe-completed";
		proc_perf_l2label[CVMX_L2C_EVENT_TAG_DIRTY] = 
			"tag-dirty-victim";
		proc_perf_l2label[CVMX_L2C_EVENT_DATA_STORE_NOP] = 
			"data-store-nop";
		proc_perf_l2label[CVMX_L2C_EVENT_DATA_STORE_READ] = 
			"data-store-read";
		proc_perf_l2label[CVMX_L2C_EVENT_DATA_STORE_WRITE] = 
			"data-store-write";
		proc_perf_l2label[CVMX_L2C_EVENT_FILL_DATA_VALID] =
			"memory-fill-data-valid";
		proc_perf_l2label[CVMX_L2C_EVENT_WRITE_REQUEST] =
			"memory-write-request";
		proc_perf_l2label[CVMX_L2C_EVENT_READ_REQUEST] =
			"memory-read-request";
		proc_perf_l2label[CVMX_L2C_EVENT_WRITE_DATA_VALID] =
			"memory-write-data-valid";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_NOP] = "xmc-nop";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_LDT] = "xmc-ldt";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_LDI] = "xmc-ldi";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_LDD] = "xmc-ldd";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_STF] = "xmc-stf";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_STT] = "xmc-stt";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_STP] = "xmc-stp";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_STC] = "xmc-stc";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_DWB] = "xmc-dwb";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_PL2] = "xmc-pl2";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_PSL1] = "xmc-psl1";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_IOBLD] = "xmc-iobld";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_IOBST] = "xmc-iobst";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_IOBDMA] = "xmc-iobdma";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_IOBRSP] = "xmc-iobrsp";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_BUS_VALID] = 
			"xmd-bus-valid";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_MEM_DATA] =
			"xmd-bus-valid-dst-l2c";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_REFL_DATA] =
			"xmd-bus-valid-dst-iob";
		proc_perf_l2label[CVMX_L2C_EVENT_XMC_IOBRSP_DATA] =
			"xmd-bus-valid-dst-pp";
		proc_perf_l2label[CVMX_L2C_EVENT_RSC_NOP] = "rsc-nop";
		proc_perf_l2label[CVMX_L2C_EVENT_RSC_STDN] = "rsc-stdn";
		proc_perf_l2label[CVMX_L2C_EVENT_RSC_FILL] = "rsc-fill";
		proc_perf_l2label[CVMX_L2C_EVENT_RSC_REFL] = "rsc-refl";
		proc_perf_l2label[CVMX_L2C_EVENT_RSC_STIN] = "rsc-stin";
		proc_perf_l2label[CVMX_L2C_EVENT_RSC_SCIN] = "rsc-scin";
		proc_perf_l2label[CVMX_L2C_EVENT_RSC_SCFL] = "rsc-scfl";
		proc_perf_l2label[CVMX_L2C_EVENT_RSC_SCDN] = "rsc-scdn";
		proc_perf_l2label[CVMX_L2C_EVENT_RSC_DATA_VALID] = 
			"rsd-data-valid";
		proc_perf_l2label[CVMX_L2C_EVENT_RSC_VALID_FILL] =
			"rsd-data-valid-fill";
		proc_perf_l2label[CVMX_L2C_EVENT_RSC_VALID_STRSP] =
			"rsd-data-valid-strsp";
		proc_perf_l2label[CVMX_L2C_EVENT_RSC_VALID_REFL] =
			"rsd-data-valid-refl";
		proc_perf_l2label[CVMX_L2C_EVENT_LRF_REQ] = "lrf-req";
		proc_perf_l2label[CVMX_L2C_EVENT_DT_RD_ALLOC] = "dt-rd-alloc";
		proc_perf_l2label[CVMX_L2C_EVENT_DT_WR_INVAL] = "dt-wr-inva";
	}

	proc_perf_entry = create_proc_entry("octeon_perf", 0, NULL);
	if (proc_perf_entry)
		proc_perf_entry->proc_fops = &proc_perf_operations;

	/* Octeon2 has different L2C performance counters */
        if (!(OCTEON_IS_MODEL(OCTEON_CN5XXX) || 
	      OCTEON_IS_MODEL(OCTEON_CN3XXX))) {
		strcpy(l2counter0, "hit");	
		strcpy(l2counter1, "miss");	
		strcpy(l2counter2, "lfb-wait-lfb");	
		strcpy(l2counter3, "lfb-wait-vab");	
	}

	proc_perf_setup();
	return 0;
}


/**
 * Module cleanup
 *
 * @return
 */
static void __exit proc_perf_cleanup(void)
{
	if (proc_perf_entry)
		remove_proc_entry("octeon_perf", NULL);
}


MODULE_LICENSE("GPL");
MODULE_AUTHOR("Cavium Networks <support@caviumnetworks.com>");
MODULE_DESCRIPTION("Cavium Networks Octeon performance counter interface.");
module_init(proc_perf_init);
module_exit(proc_perf_cleanup);
