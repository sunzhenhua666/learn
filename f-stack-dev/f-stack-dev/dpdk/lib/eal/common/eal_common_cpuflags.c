/* SPDX-License-Identifier: BSD-3-Clause
 * Copyright(c) 2010-2014 Intel Corporation
 */

#include <stdio.h>

#include <rte_common.h>
#include <rte_cpuflags.h>
/*
rte_cpu_is_supported() 是 DPDK（Data Plane Development Kit）库中的一个函数，
用于检查当前 CPU 是否受到 DPDK 库支持。
这个函数可以用于在程序启动时检查系统的 CPU 是否符合 DPDK 的要求。

`rte_cpu_is_supported()` 函数的实现原理涉及到 DPDK 库在编译时和运行时进行的 CPU 特性检测。
这个函数的目的是在程序启动时检查当前系统的 CPU 是否符合 DPDK 库的要求，以避免在不受支持的 CPU 上运行 DPDK 应用程序导致的问题。

具体的实现原理可能会因 DPDK 版本和架构的不同而有所不同，但一般来说，它包含以下步骤：

1. **编译时特性检测**：
   在 DPDK 编译时，会根据目标架构和配置选项生成一个特性检测的头文件（通常是 `rte_config.h`）。这个头文件会根据编译环境和目标平台的特性定义预处理宏，用于指示支持的 CPU 特性。这些特性可以包括 SIMD 指令集、虚拟化特性、内存模型等。

2. **运行时检测**：
   当调用 `rte_cpu_is_supported()` 函数时，DPDK 库会查询运行时系统的 CPU 特性。这可能涉及在运行时使用 CPUID 指令或其他相关机制来获取当前系统的 CPU 特性。然后，DPDK 会将运行时检测到的特性与编译时预定义的特性进行比较，以确定当前 CPU 是否受到支持。

3. **返回结果**：
   根据比较的结果，函数会返回一个整数值。通常情况下，返回值为 0 表示当前 CPU 不受 DPDK 支持，返回非零值表示当前 CPU 受到支持。

总体来说，`rte_cpu_is_supported()` 函数通过在编译时和运行时进行特性检测，判断当前系统的 CPU 是否受到 DPDK 支持。这样可以在程序启动时进行判断，以避免在不受支持的 CPU 上运行 DPDK 应用程序。不同的 DPDK 版本和架构可能会有不同的实现细节，但核心思想是类似的。

*/
int
rte_cpu_is_supported(void)
{
	/* This is generated at compile-time by the build system */
	static const enum rte_cpu_flag_t compile_time_flags[] = {
			RTE_COMPILE_TIME_CPUFLAGS
	};
	unsigned count = RTE_DIM(compile_time_flags), i;
	int ret;

	for (i = 0; i < count; i++) {
		ret = rte_cpu_get_flag_enabled(compile_time_flags[i]);

		if (ret < 0) {
			fprintf(stderr,
				"ERROR: CPU feature flag lookup failed with error %d\n",
				ret);
			return 0;
		}
		if (!ret) {
			fprintf(stderr,
			        "ERROR: This system does not support \"%s\".\n"
			        "Please check that RTE_MACHINE is set correctly.\n",
			        rte_cpu_get_flag_name(compile_time_flags[i]));
			return 0;
		}
	}

	return 1;
}
