/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (C) 2020-2022 Oplus. All rights reserved.
 */

/*
this file is splited from the sa_common.h to adapt the OKI,
IS_ENABLED is not allowed in here, because the macro will not work in OKI.
*/
#ifndef _OPLUS_SA_COMMON_STRUCT_H_
#define _OPLUS_SA_COMMON_STRUCT_H_

#define MAX_CLUSTER            (4)

/*#if IS_ENABLED(CONFIG_OPLUS_FEATURE_CPU_JANKINFO)*/
/* hot-thread */
struct task_record {
#define RECOED_WINSIZE			(1 << 8)
#define RECOED_WINIDX_MASK		(RECOED_WINSIZE - 1)
	u8 winidx;
	u8 count;
};

#define MAX_TASK_COMM_LEN 256
struct uid_struct {
	uid_t uid;
	u64 uid_total_cycle;
	u64 uid_total_inst;
	spinlock_t lock;
	char leader_comm[TASK_COMM_LEN];
	char cmdline[MAX_TASK_COMM_LEN];
};

struct  amu_uid_entry {
	uid_t uid;
	struct uid_struct *uid_struct;
	struct hlist_node node;
};

/*#endif*/

/*#if IS_ENABLED(CONFIG_OPLUS_LOCKING_STRATEGY)*/
struct locking_info {
	u64 waittime_stamp;
	u64 holdtime_stamp;
	/* Used in torture acquire latency statistic.*/
	u64 acquire_stamp;
	/*
	 * mutex or rwsem optimistic spin start time. Because a task
	 * can't spin both on mutex and rwsem at one time, use one common
	 * threshold time is OK.
	 */
	u64 opt_spin_start_time;
	struct task_struct *holder;
	u32 waittype;
	bool ux_contrib;
	/*
	 * Whether task is ux when it's going to be added to mutex or
	 * rwsem waiter list. It helps us check whether there is ux
	 * task on mutex or rwsem waiter list. Also, a task can't be
	 * added to both mutex and rwsem at one time, so use one common
	 * field is OK.
	 */
	bool is_block_ux;
	u32 kill_flag;
	/* for cfs enqueue smoothly.*/
	struct list_head node;
	struct task_struct *owner;
	struct list_head lock_head;
	u64 clear_seq;
	atomic_t lock_depth;
};
/*#endif*/

/*#if IS_ENABLED(CONFIG_HMBIRD_SCHED_GKI)*/
#define RAVG_HIST_SIZE 	5
#define SCX_SLICE_DFL 	(1 * NSEC_PER_MSEC)
#define SCX_SLICE_INF	U64_MAX
#define DEFAULT_CGROUP_DL_IDX (8)
#define EXT_FLAG_RT_CHANGED  	(1 << 0)
#define EXT_FLAG_CFS_CHANGED 	(1 << 1)
struct scx_task_stats {
	u64	mark_start;
	u64	window_start;
	u32	sum;
	u32	sum_history[RAVG_HIST_SIZE];
	int	cidx;
	u32	demand;
	u16	demand_scaled;
	void	*sdsq;
};
/*
 * The following is embedded in task_struct and contains all fields necessary
 * for a task to be scheduled by SCX.
 */
struct scx_entity {
	struct scx_dispatch_q	*dsq;
	struct {
		struct list_head	fifo;	/* dispatch order */
		struct rb_node		priq;	/* p->scx.dsq_vtime order */
	} dsq_node;
	u32			flags;		/* protected by rq lock */
	u32			dsq_flags;	/* protected by dsq lock */
	s32			sticky_cpu;
	unsigned long		runnable_at;
	u64			slice;
	u64			dsq_vtime;
	int			gdsq_idx;
	int 			ext_flags;
	int 			prio_backup;
	unsigned long		sched_prop;
	struct scx_task_stats 	sts;
};
/*#endif*/

#if IS_ENABLED(CONFIG_OPLUS_FEATURE_POWERMODEL)

#define MAX_CPU_FREQ_STATE 32
#define MAX_CPU_CNT 8

#define STATE_IN_POOL 0
#define STATE_ACTIVE 1

struct powermodel_freq_task_state {
	u8 powermodel_enqueued:1;
	u8 powermodel_index:7;
};

struct powermodel_cpu_task_state {
	struct oplus_task_struct *ots;
	struct powermodel_freq_task_state powermodel_freq_task_states[MAX_CPU_FREQ_STATE];
	u64 powermodel_last_seq;
	u64 last_seq;
	struct list_head node;
	int state;
	int pool_id;
};

#endif


/* Please add your own members of task_struct here :) */
struct oplus_task_struct {
	/* CONFIG_OPLUS_FEATURE_SCHED_ASSIST */
	struct rb_node ux_entry;
	struct rb_node exec_time_node;
	struct task_struct *task;
	atomic64_t inherit_ux;
	u64 enqueue_time;
	u64 inherit_ux_start;
	/* u64 sum_exec_baseline; */
	u64 total_exec;
	u64 vruntime;
	u64 preset_vruntime;
	/* contains ux state
	 1. if static and inherited ux both exist, static ux stores in ux_state, inherited ux in sub_ux_state.
	 2. if only static ux exists, static ux stores in ux_state.
	 2. if only inherited ux exists, inherited ux stores in ux_state */
	int ux_state;
	int sub_ux_state;
	u8 ux_depth;
	s8 ux_priority;
	s8 ux_nice;
	pid_t affinity_pid;
	pid_t affinity_tgid;
	unsigned long state;
	unsigned long im_flag;
	atomic_t is_vip_mvp;

/* #if IS_ENABLED(CONFIG_OPLUS_FEATURE_SCHED_DDL) */
	u64 ddl;
	u64 ddl_active_ts;
	u64 runnable_ts;
	struct rb_node ddl_node;
/* #endif */
/*#if IS_ENABLED(CONFIG_OPLUS_FEATURE_ABNORMAL_FLAG)*/
	int abnormal_flag;
/*#endif*/
	/* CONFIG_OPLUS_FEATURE_SCHED_SPREAD */
	int lb_state;
	int ld_flag:1;
	/* CONFIG_OPLUS_FEATURE_TASK_LOAD */
	int is_update_runtime:1;
	int target_process;
	u64 wake_tid;
	u64 running_start_time;
	bool update_running_start_time;
	u64 exec_calc_runtime;
/*#if IS_ENABLED(CONFIG_OPLUS_FEATURE_CPU_JANKINFO)*/
	struct task_record record[MAX_CLUSTER];	/* 2*u64 */
	u64 block_start_time;
/*#endif*/
	/* CONFIG_OPLUS_FEATURE_FRAME_BOOST */
	struct list_head fbg_list;
	raw_spinlock_t fbg_list_entry_lock;
	bool fbg_running; /* task belongs to a group, and in running */
	u16 fbg_state;
	s8 preferred_cluster_id;
	s8 fbg_depth;
	u64 last_wake_ts;
	int fbg_cur_group;
/*#ifdef CONFIG_LOCKING_PROTECT*/
	unsigned long locking_start_time;
	unsigned long last_jiffies;
	struct list_head locking_entry;
	int locking_depth;
	int lk_tick_hit;
/*#endif*/
/*#if IS_ENABLED(CONFIG_OPLUS_LOCKING_STRATEGY)*/
	struct locking_info lkinfo;
/*#endif*/
/*#if IS_ENABLED(CONFIG_HMBIRD_SCHED_GKI)*/
	struct scx_entity scx;
/*#endif*/

/*#if IS_ENABLED(CONFIG_OPLUS_FEATURE_FDLEAK_CHECK)*/
	u8 fdleak_flag;
/*#endif*/

/*#if IS_ENABLED(CONFIG_OPLUS_FEATURE_LOADBALANCE)*/
	/* for loadbalance */
	struct plist_node rtb;		/* rt boost task */

	/*
	 * The following variables are used to calculate the time
	 * a task spends in the running/runnable state.
	 */
	u64 snap_run_delay;
	unsigned long snap_pcount;
/*#endif*/
#if IS_ENABLED(CONFIG_OPLUS_SCHED_TUNE)
	int stune_idx;
#endif
/*#if IS_ENABLED(CONFIG_OPLUS_FEATURE_PIPELINE)*/
	atomic_t pipeline_cpu;
/*#endif*/

	/* for oplus secure guard */
	int sg_flag;
	int sg_scno;
	uid_t sg_uid;
	uid_t sg_euid;
	gid_t sg_gid;
	gid_t sg_egid;
/*#if IS_ENABLED(CONFIG_ARM64_AMU_EXTN) && IS_ENABLED(CONFIG_OPLUS_FEATURE_CPU_JANKINFO)*/
	struct uid_struct *uid_struct;
	u64 amu_instruct;
	u64 amu_cycle;
/*#endif*/
	/* for binder ux */
	int binder_async_ux_enable;
	bool binder_async_ux_sts;
	int binder_thread_mode;
	struct binder_node *binder_thread_node;

/* for powermodel */
#if IS_ENABLED(CONFIG_OPLUS_FEATURE_POWERMODEL)
	struct powermodel_cpu_task_state *powermodel_cpu_task_states[MAX_CPU_CNT];
	u64 exec_runtime;
#endif

#if IS_ENABLED(CONFIG_OPLUS_FEATURE_SCHED_CFBT)
	int cfbt_cur_group;
	bool cfbt_running;
#endif /* CONFIG_OPLUS_FEATURE_SCHED_CFBT */
} ____cacheline_aligned;

/*#if IS_ENABLED(CONFIG_OPLUS_FEATURE_LOADBALANCE)*/
#define INVALID_PID						(-1)
struct oplus_lb {
	/* used for active_balance to record the running task. */
	pid_t pid;
};
/*#endif*/

#endif /* _OPLUS_SA_COMMON_STRUCT_H_ */

