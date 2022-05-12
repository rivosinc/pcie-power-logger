/*
 * Copyright (c) 2022 Rivos Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * I2C task implementation which continuously grabs current and voltage
 * measurement data from the PMD device and stuffs the data into a circular
 * buffer.
 */
#include <device.h>
#include <drivers/i2c.h>
#include <stdio.h>
#include <zephyr.h>

#include "ringbuffer.h"
#include "pmd.h"
#include "tasks.h"

void i2c_task(void) {
	const struct device *i2c_dev =
		DEVICE_DT_GET(DT_NODELABEL(i2c1));

	struct i2c_msg msgs[2];
	uint8_t rd_addr;
	uint8_t rd_data[2] = {0, 0};
	msgs[0].buf = &rd_addr;
	msgs[0].len = 1;
	msgs[0].flags = I2C_MSG_WRITE;

	msgs[1].buf = &rd_data[0];
	msgs[1].len = 2;
	msgs[1].flags = I2C_MSG_RESTART | I2C_MSG_READ | I2C_MSG_STOP;

	while (1) {
		rd_addr = i2c_addr_from_sample_t(
			sample_t_from_buffer_idx(get_push_idx()));
		int res = i2c_transfer(i2c_dev, &msgs[0], 2, PMD_I2C_ADDR);
		if (res != 0) {
			printf("I2C ERR %d %x %x\n", res, rd_data[0], rd_data[1]);
		} else {
			/* Store timestamp and raw ADC data. */
			struct entry e =
				{ (((uint32_t)k_uptime_ticks() << ENTRY_TIMESTAMP_SHIFT) &
				  ENTRY_TIMESTAMP_MASK) |
				((((rd_data[1] << 4) + (rd_data[0] >> 4)) << ENTRY_DATA_SHIFT) &
				  ENTRY_DATA_MASK) };
			push_buffer(e);
		}
	}
}

K_THREAD_DEFINE(i2c_task_id, STACK_SIZE, i2c_task, NULL, NULL, NULL,
		I2C_TASK_PRIORITY, 0, 0);
