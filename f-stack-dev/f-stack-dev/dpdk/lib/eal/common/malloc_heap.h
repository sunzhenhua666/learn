/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#ifndef MALLOC_HEAP_H_
#define MALLOC_HEAP_H_

#include <stdbool.h>
#include <sys/queue.h>

#include <rte_malloc.h>
#include <rte_spinlock.h>

/* Number of free lists per heap, grouped by size. */
#define RTE_HEAP_NUM_FREELISTS  13
#define RTE_HEAP_NAME_MAX_LEN 32

/* dummy definition, for pointers */
struct malloc_elem;

/**
 * Structure to hold malloc heap
 rte_spinlock_t lock: 这是一个自旋锁，用于在多线程环境中对内存堆进行同步，以防止多个线程同时访问和修改内存堆。

LIST_HEAD(, malloc_elem) free_head[RTE_HEAP_NUM_FREELISTS]: 这是一个数组，每个元素是一个链表头，用于存储不同大小的空闲内存块。malloc_elem 是一个数据结构，用于表示内存块的信息。

struct malloc_elem *volatile first; 和 struct malloc_elem *volatile last;: 这两个指针分别指向内存堆中分配的第一个和最后一个内存块。这些指针可以用于快速查找可用内存块。

unsigned int alloc_count;: 表示当前内存堆已经分配的内存块数量。

unsigned int socket_id;: 表示内存堆所属的套接字 ID，用于 NUMA（非统一内存访问）感知。

size_t total_size;: 表示内存堆的总大小（字节数）。

char name[RTE_HEAP_NAME_MAX_LEN];: 用于存储内存堆的名称，可能是用来标识内存堆的字符串。

__rte_cache_aligned;: 这是一个属性，表示将结构体对齐到 CPU 缓存行的大小，以提高内存访问效率。
 */
struct malloc_heap {
	rte_spinlock_t lock;
	LIST_HEAD(, malloc_elem) free_head[RTE_HEAP_NUM_FREELISTS];
	struct malloc_elem *volatile first;
	struct malloc_elem *volatile last;

	unsigned int alloc_count;
	unsigned int socket_id;
	size_t total_size;
	char name[RTE_HEAP_NAME_MAX_LEN];
} __rte_cache_aligned;

void *
malloc_heap_alloc(const char *type, size_t size, int socket, unsigned int flags,
		size_t align, size_t bound, bool contig);

void *
malloc_heap_alloc_biggest(const char *type, int socket, unsigned int flags,
		size_t align, bool contig);

int
malloc_heap_create(struct malloc_heap *heap, const char *heap_name);

int
malloc_heap_destroy(struct malloc_heap *heap);

struct rte_memseg_list *
malloc_heap_create_external_seg(void *va_addr, rte_iova_t iova_addrs[],
		unsigned int n_pages, size_t page_sz, const char *seg_name,
		unsigned int socket_id);

struct rte_memseg_list *
malloc_heap_find_external_seg(void *va_addr, size_t len);

int
malloc_heap_destroy_external_seg(struct rte_memseg_list *msl);

int
malloc_heap_add_external_memory(struct malloc_heap *heap,
		struct rte_memseg_list *msl);

int
malloc_heap_remove_external_memory(struct malloc_heap *heap, void *va_addr,
		size_t len);

int
malloc_heap_free(struct malloc_elem *elem);

int
malloc_heap_resize(struct malloc_elem *elem, size_t size);

int
malloc_heap_get_stats(struct malloc_heap *heap,
		struct rte_malloc_socket_stats *socket_stats);

void
malloc_heap_dump(struct malloc_heap *heap, FILE *f);

int
malloc_socket_to_heap_id(unsigned int socket_id);

int
rte_eal_malloc_heap_init(void);

#endif /* MALLOC_HEAP_H_ */
