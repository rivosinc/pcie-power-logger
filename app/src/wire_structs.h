/*
 * SPDX-FileCopyrightText: 2023 Rivos Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Structs sent over wire to http client. */

#ifndef __WIRE_STRUCTS_H__
#define __WIRE_STRUCTS_H__

#include <zephyr/kernel.h>

#include "ringbuffer.h"

struct wire_data_t {
	/* 20 bits timestamp, 12 bits raw ADC data. */
	struct entry e;
	/*
	 * Current time for synchronization: 'T'.
	 * PCIE1: 'I' (current) or 'V' (voltage).
	 * PCIE2: 'J' (current) or 'W' (voltage).
	 */
	uint8_t entry_type;
	uint8_t reserved1;
	uint8_t reserved2;
	uint8_t reserved3;
} __packed;

BUILD_ASSERT(sizeof(struct wire_data_t) == 8);

#endif /* __WIRE_STRUCTS_H__ */
