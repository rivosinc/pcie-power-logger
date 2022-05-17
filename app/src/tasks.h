/*
 * Copyright (c) 2022 Rivos Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __TASKS_H__
#define __TASKS_H__

#include "pmd.h"
#include "ringbuffer.h"

#include <zephyr.h>

#define STACK_SIZE 1024

/*
 * Try to maximize I2C bus usage by making the I2C task higher priority. The
 * HTTP process should be able to consume / send data at a rate much faster than
 * the I2C process can produce it (~10 KB / sec * 8 bytes per entry).
 */
#define I2C_TASK_PRIORITY 6
#define HTTP_TASK_PRIORITY 7

/*
 * Number of input channels to monitor on PMD.
 * 1 = PCIE1 only.
 * 2 = PCIE1 and PCIE2.
 */
#define INPUT_CHANNELS 2
/*
 * Record 31 current samples for every 1 voltage sample since voltage usually
 * changes at a slower rate.
 */
#define VOLTAGE_IDX 32

BUILD_ASSERT(INPUT_CHANNELS == 1 || INPUT_CHANNELS == 2);

enum sample_t {
	PCIE1_VOLTAGE,
	PCIE2_VOLTAGE,
	PCIE1_CURRENT,
	PCIE2_CURRENT,
};

static inline enum sample_t sample_t_from_buffer_idx(index_t idx) {
	bool voltage = (idx % VOLTAGE_IDX) == 0;
	if (voltage) {
		return (idx % (VOLTAGE_IDX * INPUT_CHANNELS)) == 0
			? PCIE1_VOLTAGE : PCIE2_VOLTAGE;
	} else {
	return (idx % INPUT_CHANNELS == 0) ? PCIE1_CURRENT : PCIE2_CURRENT;
	}
}

static inline uint8_t i2c_addr_from_sample_t(enum sample_t sample) {
	switch (sample) {
		case PCIE1_VOLTAGE: return PMD_PCIE1_VOLTAGE_I2C_CMD; break;
		case PCIE2_VOLTAGE: return PMD_PCIE2_VOLTAGE_I2C_CMD; break;
		case PCIE1_CURRENT: return PMD_PCIE1_CURRENT_I2C_CMD; break;
		case PCIE2_CURRENT: return PMD_PCIE2_CURRENT_I2C_CMD; break;
	}
	return 0;
}

static inline char char_from_sample_t(enum sample_t sample) {
	switch (sample) {
		case PCIE1_VOLTAGE: return 'V'; break;
		case PCIE2_VOLTAGE: return 'W'; break;
		case PCIE1_CURRENT: return 'I'; break;
		case PCIE2_CURRENT: return 'J'; break;
	}
	return 0;
}

#endif /* __TASKS_H__ */
