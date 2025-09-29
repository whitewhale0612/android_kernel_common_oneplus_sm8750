/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2024 Oplus. All rights reserved.
 */
#ifndef _OPLUS_SCHED_EXT_H
#define _OPLUS_SCHED_EXT_H
#include "sa_common.h"

#define SCHED_PROP_TOP_THREAD_SHIFT (8)
#define SCHED_PROP_TOP_THREAD_MASK  (0xf << SCHED_PROP_TOP_THREAD_SHIFT)
#define SCHED_PROP_DEADLINE_MASK (0xFF) /* deadline for ext sched class */
#define SCHED_PROP_DEADLINE_LEVEL1 (1)  /* 1ms for user-aware audio tasks */
#define SCHED_PROP_DEADLINE_LEVEL2 (2)  /* 2ms for user-aware touch tasks */
#define SCHED_PROP_DEADLINE_LEVEL3 (3)  /* 4ms for user aware dispaly tasks */
#define SCHED_PROP_DEADLINE_LEVEL4 (4)  /* 6ms */
#define SCHED_PROP_DEADLINE_LEVEL5 (5)  /* 8ms */
#define SCHED_PROP_DEADLINE_LEVEL6 (6)  /* 16ms */
#define SCHED_PROP_DEADLINE_LEVEL7 (7)  /* 32ms */
#define SCHED_PROP_DEADLINE_LEVEL8 (8)  /* 64ms */
#define SCHED_PROP_DEADLINE_LEVEL9 (9)  /* 128ms */

static inline int sched_prop_get_top_thread_id(struct task_struct *p)
{
	struct oplus_task_struct *ots = get_oplus_task_struct(p);

	if (!ots) {
		return -EPERM;
	}

	return ((ots->scx.sched_prop & SCHED_PROP_TOP_THREAD_MASK) >> SCHED_PROP_TOP_THREAD_SHIFT);
}

static inline int sched_set_sched_prop(struct task_struct *p, unsigned long sp)
{
	struct oplus_task_struct *ots = get_oplus_task_struct(p);

	if (!ots) {
		pr_err("scx_sched_ext: sched_set_sched_prop failed! fn=%s\n", __func__);
		return -EPERM;
	}

	ots->scx.sched_prop = sp;
	return 0;
}

static inline unsigned long sched_get_sched_prop(struct task_struct *p)
{
	struct oplus_task_struct *ots = get_oplus_task_struct(p);

	if (!ots) {
		pr_err("scx_sched_ext: sched_get_sched_prop failed! fn=%s\n", __func__);
		return (unsigned long)-1;
	}
	return ots->scx.sched_prop;
}

#endif /*_OPLUS_SCHED_EXT_H */
