/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(C) 2020 Marvell International Ltd.
 */

#ifndef __EAL_TRACE_H
#define __EAL_TRACE_H

#include <rte_cycles.h>
#include <rte_log.h>
#include <rte_malloc.h>
#include <rte_spinlock.h>
#include <rte_trace.h>
#include <rte_trace_point.h>
#include <rte_uuid.h>

#include "eal_private.h"
#include "eal_thread.h"

#define trace_err(fmt, args...) \
	RTE_LOG(ERR, EAL, "%s():%u " fmt "\n", __func__, __LINE__, ## args)

#define trace_crit(fmt, args...) \
	RTE_LOG(CRIT, EAL, "%s():%u " fmt "\n", __func__, __LINE__, ## args)

#define TRACE_PREFIX_LEN 12
#define TRACE_DIR_STR_LEN (sizeof("YYYY-mm-dd-AM-HH-MM-SS") + TRACE_PREFIX_LEN)
#define TRACE_POINT_NAME_SIZE 64
#define TRACE_CTF_MAGIC 0xC1FC1FC1
#define TRACE_MAX_ARGS	32

struct trace_point {
	STAILQ_ENTRY(trace_point) next;
	rte_trace_point_t *handle;
	char name[TRACE_POINT_NAME_SIZE];
	char *ctf_field;
};

enum trace_area_e {
	TRACE_AREA_HEAP,
	TRACE_AREA_HUGEPAGE,
};

struct thread_mem_meta {
	void *mem;
	enum trace_area_e area;
};

struct trace_arg {
	STAILQ_ENTRY(trace_arg) next;
	char *val;
};

/*
char dir[PATH_MAX]: 存储跟踪文件的目录路径，用于指定事件跟踪数据的输出位置。

int dir_offset: 跟踪文件目录路径的偏移量。

int register_errno: 存储注册错误的编号，表示在注册过程中是否发生了错误。

bool status: 表示跟踪状态的布尔值，可能用于表示跟踪是否启用或正在运行。

enum rte_trace_mode mode: 枚举类型，表示事件跟踪的模式，可能有不同的跟踪模式选项。

rte_uuid_t uuid: 用于存储唯一标识符的数据类型，可能在某些情况下用于标识事件跟踪实例。

uint32_t buff_len: 缓冲区长度，用于存储事件跟踪数据的缓冲区大小。

STAILQ_HEAD(, trace_arg) args: 一个单向链表头，可能用于存储与事件跟踪相关的参数列表。

uint32_t nb_trace_points: 跟踪点数量，表示注册的事件跟踪点的数量。

uint32_t nb_trace_mem_list: 跟踪内存列表数量，可能表示跟踪内存分配情况的列表数量。

struct thread_mem_meta *lcore_meta: 指向线程内存元数据的指针，可能用于存储线程的内存分配元数据。

uint64_t epoch_sec: 表示时间戳的秒部分，用于确定事件发生的时间。

uint64_t epoch_nsec: 时间戳的纳秒部分，用于更精确地确定事件发生的时间。

uint64_t uptime_ticks: 从系统启动以来的时钟滴答数，可能用于计算相对时间。

char *ctf_meta: 存储 CTF（Common Trace Format）元数据的字符指针，可能与事件跟踪的格式化和分析相关。

uint32_t ctf_meta_offset_freq: CTF 元数据中频率信息的偏移量。

uint32_t ctf_meta_offset_freq_off_s: CTF 元数据中频率偏移的秒部分。

uint32_t ctf_meta_offset_freq_off: CTF 元数据中频率偏移的纳秒部分。

uint16_t ctf_fixup_done: 表示 CTF 修复是否已完成的标志。

rte_spinlock_t lock: 用于保护结构体中数据的自旋锁。

*/
struct trace {
	char dir[PATH_MAX];
	int dir_offset;
	int register_errno;
	bool status;
	enum rte_trace_mode mode;
	rte_uuid_t uuid;
	uint32_t buff_len;
	STAILQ_HEAD(, trace_arg) args;
	uint32_t nb_trace_points;
	uint32_t nb_trace_mem_list;
	struct thread_mem_meta *lcore_meta;
	uint64_t epoch_sec;
	uint64_t epoch_nsec;
	uint64_t uptime_ticks;
	char *ctf_meta;
	uint32_t ctf_meta_offset_freq;
	uint32_t ctf_meta_offset_freq_off_s;
	uint32_t ctf_meta_offset_freq_off;
	uint16_t ctf_fixup_done;
	rte_spinlock_t lock;
};

/* Helper functions */
static inline uint16_t
trace_id_get(rte_trace_point_t *trace)
{
	return (*trace & __RTE_TRACE_FIELD_ID_MASK) >>
		__RTE_TRACE_FIELD_ID_SHIFT;
}

static inline size_t
trace_mem_sz(uint32_t len)
{
	return len + sizeof(struct __rte_trace_header);
}

/* Trace object functions */
struct trace *trace_obj_get(void);

/* Trace point list functions */
STAILQ_HEAD(trace_point_head, trace_point);
struct trace_point_head *trace_list_head_get(void);

/* Util functions */
const char *trace_mode_to_string(enum rte_trace_mode mode);
const char *trace_area_to_string(enum trace_area_e area);
int trace_args_apply(const char *arg);
void trace_bufsz_args_apply(void);
bool trace_has_duplicate_entry(void);
void trace_uuid_generate(void);
int trace_metadata_create(void);
void trace_metadata_destroy(void);
char *trace_metadata_fixup_field(const char *field);
int trace_mkdir(void);
int trace_epoch_time_save(void);
void trace_mem_free(void);
void trace_mem_per_thread_free(void);

/* EAL interface */
int eal_trace_init(void);
void eal_trace_fini(void);
int eal_trace_args_save(const char *val);
void eal_trace_args_free(void);
int eal_trace_dir_args_save(const char *val);
int eal_trace_mode_args_save(const char *val);
int eal_trace_bufsz_args_save(const char *val);

#endif /* __EAL_TRACE_H */
