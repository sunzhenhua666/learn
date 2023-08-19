/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2019 Intel Corporation
 */

#ifndef EAL_MEMCFG_H
#define EAL_MEMCFG_H

#include <rte_memory.h>
#include <rte_memzone.h>
#include <rte_pause.h>
#include <rte_spinlock.h>
#include <rte_rwlock.h>
#include <rte_tailq.h>

#include "malloc_heap.h"

/**
 * Memory configuration shared across multiple processes.
magic: 魔数，用于进行内存配置的完整性检查。
version: DPDK 版本号，用于防止不同版本的 DPDK 库之间进行交互。
nchannel: 内存通道数量。
nrank: 内存模块数量。
mlock, qlock, mplock, tlock: 这些是不同锁，用于在内存分配、队列、内存池和计时器库中确保线程安全。
memory_hotplug_lock: 表示内存热插拔请求是否正在进行。
mp_status: 多进程状态。
memzones: 内存区域（memzone）描述符的数组。
memsegs[RTE_MAX_MEMSEG_LISTS]: 动态数组，用于存储内存段（memseg）的列表。
tailq_head[RTE_MAX_TAILQ]: 对象的队列（tailq）。
malloc_heaps[RTE_MAX_HEAPS]: DPDK 的堆（heap）。 malloc_heaps[RTE_MAX_HEAPS] ： DPDK 的堆（heap）。
next_socket_id: 下一个用于外部 malloc 堆的 socket ID。
mem_cfg_addr: 内存中此结构的地址，用于在所有进程中映射相同的地址。
legacy_mem: 存储的传统内存参数。
single_file_segments: 存储的单文件段（segments）参数。
tsc_hz: TSC（时间戳计数器）频率。
dma_maskbits: 保留较为严格的 DMA 掩码位。
这个结构的作用是用于 DPDK 库内部管理内存配置，以及确保内存分配和操作的线程安全性。
它涵盖了内存分配、内存段管理、锁等多个方面的配置和信息。
这些配置和信息都是 DPDK 库内部使用的，不需要应用程序直接操作。
 */
struct rte_mem_config {
	volatile uint32_t magic;   /**< Magic number - sanity check. */
	uint32_t version;
	/**< Prevent secondary processes using different DPDK versions. */

	/* memory topology
	uint32_t nchannel：这个成员表示系统中内存通道的数量。通道是一种物理组织结构，通常在系统中用于访问内存。每个通道可以包含一个或多个内存条（或称为内存模块）。
	uint32_t nrank：这个成员表示系统中内存条（或内存模块）的数量。内存条是一种物理硬件，用于存储数据，每个内存条可能包含一个或多个存储单元（也称为“rank”）
	*/
	uint32_t nchannel;    /**< Number of channels (0 if unknown). */
	uint32_t nrank;       /**< Number of ranks (0 if unknown). */

	/**
	 * current lock nest order
	 *  - qlock->mlock (ring/hash/lpm)
	 *  - mplock->qlock->mlock (mempool)
	 * Notice:
	 *  *ALWAYS* obtain qlock first if having to obtain both qlock and mlock
	 */
	rte_rwlock_t mlock;   /**< used by memzones for thread safety. */
	rte_rwlock_t qlock;   /**< used by tailqs for thread safety. */
	rte_rwlock_t mplock;  /**< used by mempool library for thread safety. */
	rte_spinlock_t tlock; /**< used by timer library for thread safety. */

	rte_rwlock_t memory_hotplug_lock;
	/**< Indicates whether memory hotplug request is in progress.
	"Hotplug" 是一个术语，通常用于描述在运行时动态添加、移除或配置硬件设备或资源的能力。这可以是计算机硬件、外部设备或其他资源。"Hotplug" 的概念允许系统在不需要重新启动的情况下适应变化。

	例如，"hotplug" 可以应用于以下场景：

	CPU Hotplug: 在支持 CPU "hotplug" 的系统中，您可以在运行时将一个或多个 CPU 核心添加到系统中或从系统中移除。这对于负载均衡和能效优化很有用。

	Memory Hotplug: 有些系统支持内存 "hotplug"，允许您在运行时添加或移除内存模块，以动态调整系统的内存容量。

	USB设备Hotplug: 当您将 USB 设备插入计算机时，操作系统可以自动检测并启动相应的驱动程序，而无需重新启动系统。

	网络设备Hotplug: 在一些场景中，您可以在运行时添加或移除网络接口卡，以适应网络配置变化。

	硬盘热插拔: 一些系统允许您在运行时添加或移除硬盘，而无需关机。
	*/

	uint8_t mp_status; /**< Multiprocess status. */

	/* memory segments and zones

	"Memory segments" 和 "memory zones" 是与操作系统中的内存管理相关的概念。它们描述了如何将系统内存划分和管理，以支持不同类型的内存分配和使用。

	Memory Segments:

	在操作系统内存管理中，"memory segments" 是指将物理内存划分成不同的连续区域，每个区域用于不同的用途。这些区域可以对应不同的内存映射，如内核空间和用户空间，或者用于存储代码、数据、堆栈等。

	常见的内存段（segment）可能包括：

	Code Segment: 用于存储程序的指令（代码）。

	Data Segment: 用于存储全局和静态数据。

	Heap Segment: 用于动态分配的内存，例如通过 malloc() 或 new 分配的内存。

	Stack Segment: 用于存储函数调用的局部变量和调用上下文。

	Memory-Mapped Segment: 用于映射文件或设备到内存，以方便访问。

	Memory Zones:

	"Memory zones" 是特定于 Linux 内核的概念，用于在物理内存上实现不同的内存区域。内核将系统物理内存分为不同的区域，每个区域用于不同的目的，例如用于高速缓存、DMA（直接内存访问）、用户空间等。

	常见的内存区域（zone）可能包括：

	Normal Zone: 用于常规用户和内核分配。

	DMA Zone: 用于支持设备的直接内存访问。

	Highmem Zone: 用于处理大量内存的系统，其中一部分物理内存不能被直接映射到内核地址空间。

	这些概念都有助于操作系统有效地管理内存资源，确保不同类型的内存使用具有适当的特性和访问权限。在不同的操作系统和内核中，这些概念的实现可能会有所不同，所以在了解具体细节时，建议查阅相关的文档和资料。
	*/
	struct rte_fbarray memzones; /**< Memzone descriptors. */

	struct rte_memseg_list memsegs[RTE_MAX_MEMSEG_LISTS];
	/**< List of dynamic arrays holding memsegs */

	struct rte_tailq_head tailq_head[RTE_MAX_TAILQ];
	/**< Tailqs for objects */

	struct malloc_heap malloc_heaps[RTE_MAX_HEAPS];
	/**< DPDK malloc heaps */

	int next_socket_id; /**< Next socket ID for external malloc heap */

	/* rte_mem_config has to be mapped at the exact same address in all
	 * processes, so we need to store it.
	 */
	uint64_t mem_cfg_addr; /**< Address of this structure in memory. */

	/* Primary and secondary processes cannot run with different legacy or
	 * single file segments options, so to avoid having to specify these
	 * options to all processes, store them in shared config and update the
	 * internal config at init time.
	 */
	uint32_t legacy_mem; /**< stored legacy mem parameter. */
	uint32_t single_file_segments;
	/**< stored single file segments parameter. */

	uint64_t tsc_hz;
	/**< TSC rate */

	uint8_t dma_maskbits; /**< Keeps the more restricted dma mask. */
};

/* update internal config from shared mem config */
void
eal_mcfg_update_internal(void);

/* update shared mem config from internal config */
void
eal_mcfg_update_from_internal(void);

/* wait until primary process initialization is complete */
void
eal_mcfg_wait_complete(void);

/* check if DPDK version of current process matches one stored in the config */
int
eal_mcfg_check_version(void);

/* set mem config as complete */
void
eal_mcfg_complete(void);

#endif /* EAL_MEMCFG_H */
