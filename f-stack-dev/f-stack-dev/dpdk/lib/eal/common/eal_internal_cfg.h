/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

/**
 * @file
 * Holds the structures for the eal internal configuration
 */

#ifndef EAL_INTERNAL_CFG_H
#define EAL_INTERNAL_CFG_H

#include <rte_eal.h>
#include <rte_os_shim.h>
#include <rte_pci_dev_feature_defs.h>

#include "eal_thread.h"

#if defined(RTE_ARCH_ARM)
#define MAX_HUGEPAGE_SIZES 4  /**< support up to 4 page sizes */
#else
#define MAX_HUGEPAGE_SIZES 3  /**< support up to 3 page sizes */
#endif

/*
 * internal configuration structure for the number, size and
 * mount points of hugepages
 */
struct hugepage_info {
	uint64_t hugepage_sz;   /**< size of a huge page */
	char hugedir[PATH_MAX];    /**< dir where hugetlbfs is mounted */
	uint32_t num_pages[RTE_MAX_NUMA_NODES];
	/**< number of hugepages of that size on each socket */
	int lock_descriptor;    /**< file descriptor for hugepage dir */
};

struct simd_bitwidth {
	bool forced;
	/**< flag indicating if bitwidth is forced and can't be modified */
	uint16_t bitwidth; /**< bitwidth value */
};

/**
 * internal configuration
 memory: 申请的内存大小，通常与内存池相关。

force_nchannel: 强制通道数，用于内存分配。

force_nrank: 强制内存模块数，用于内存分配。

no_hugetlbfs: 是否禁用 hugetlbfs。

hugepage_unlink: 是否解除 hugepage 文件链接。

no_pci: 是否禁用 PCI。

no_hpet: 是否禁用 HPET。

vmware_tsc_map: 是否使用 VMware TSC 映射。

no_shconf: 是否没有共享配置。

in_memory: 是否完全在内存中运行。

create_uio_dev: 是否创建 /dev/uioX 设备。

process_type: 多进程类型。

force_sockets: 是否在特定 socket 上分配内存。

socket_mem[RTE_MAX_NUMA_NODES]: 每个 socket 的内存大小。

force_socket_limits: 是否限制每个 socket 的内存。

socket_limit[RTE_MAX_NUMA_NODES]: 每个 socket 的内存限制。

base_virtaddr: 尝试从中保留内存的基地址。

legacy_mem: 是否启用传统内存行为。

match_allocations: 是否精确释放巨页内存。

single_file_segments: 是否将所有页面存储在单个文件中。

syslog_facility: 传递给 openlog() 的 facility。

vfio_intr_mode: VFIO 的中断模式。

vfio_vf_token: VFIO-PCI 绑定的 VF 和 PF 设备的共享 token。

hugefile_prefix: hugetlbfs 文件的基本文件名。

hugepage_dir: 使用的 hugetlbfs 目录。

user_mbuf_pool_ops_name: 用户定义的 mbuf 池操作名称。

num_hugepage_sizes: 系统上的巨页大小数量。

hugepage_info[MAX_HUGEPAGE_SIZES]: 巨页信息数组。

iova_mode: 设置系统的 IOVA 模式。

ctrl_cpuset: 控制线程的 cpuset。

init_complete: EAL 是否已完成初始化。

no_telemetry: 是否禁用遥测。

max_simd_bitwidth: 最大 SIMD 位宽路径。
 */
struct internal_config {
	volatile size_t memory;           /**< amount of asked memory */
	volatile unsigned force_nchannel; /**< force number of channels */
	volatile unsigned force_nrank;    /**< force number of ranks */
	volatile unsigned no_hugetlbfs;   /**< true to disable hugetlbfs */
	unsigned hugepage_unlink;         /**< true to unlink backing files */
	volatile unsigned no_pci;         /**< true to disable PCI */
	volatile unsigned no_hpet;        /**< true to disable HPET */
	volatile unsigned vmware_tsc_map; /**< true to use VMware TSC mapping
										* instead of native TSC */
	volatile unsigned no_shconf;      /**< true if there is no shared config */
	volatile unsigned in_memory;
	/**< true if DPDK should operate entirely in-memory and not create any
	 * shared files or runtime data.
	 */
	volatile unsigned create_uio_dev; /**< true to create /dev/uioX devices */
	volatile enum rte_proc_type_t process_type; /**< multi-process proc type */
	/** true to try allocating memory on specific sockets */
	volatile unsigned force_sockets;
	volatile uint64_t socket_mem[RTE_MAX_NUMA_NODES]; /**< amount of memory per socket */
	volatile unsigned force_socket_limits;
	volatile uint64_t socket_limit[RTE_MAX_NUMA_NODES]; /**< limit amount of memory per socket */
	uintptr_t base_virtaddr;          /**< base address to try and reserve memory from */
	volatile unsigned legacy_mem;
	/**< true to enable legacy memory behavior (no dynamic allocation,
	 * IOVA-contiguous segments).
	 */
	volatile unsigned match_allocations;
	/**< true to free hugepages exactly as allocated */
	volatile unsigned single_file_segments;
	/**< true if storing all pages within single files (per-page-size,
	 * per-node) non-legacy mode only.
	 */
	volatile int syslog_facility;	  /**< facility passed to openlog() */
	/** default interrupt mode for VFIO */
	volatile enum rte_intr_mode vfio_intr_mode;
	/** the shared VF token for VFIO-PCI bound PF and VFs devices */
	rte_uuid_t vfio_vf_token;
	char *hugefile_prefix;      /**< the base filename of hugetlbfs files */
	char *hugepage_dir;         /**< specific hugetlbfs directory to use */
	char *user_mbuf_pool_ops_name;
			/**< user defined mbuf pool ops name */
	unsigned num_hugepage_sizes;      /**< how many sizes on this system */
	struct hugepage_info hugepage_info[MAX_HUGEPAGE_SIZES];
	enum rte_iova_mode iova_mode ;    /**< Set IOVA mode on this system  */
	rte_cpuset_t ctrl_cpuset;         /**< cpuset for ctrl threads */
	volatile unsigned int init_complete;
	/**< indicates whether EAL has completed initialization */
	unsigned int no_telemetry; /**< true to disable Telemetry */
	struct simd_bitwidth max_simd_bitwidth;
	/**< max simd bitwidth path to use */
};

void eal_reset_internal_config(struct internal_config *internal_cfg);

#endif /* EAL_INTERNAL_CFG_H */
