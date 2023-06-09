/*
 * SPDX-FileCopyrightText: 2023 Rivos Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include <zephyr/kernel.h>

/*
 * Entry data is stored in ringbuffer and sent directly on the wire. Embedded
 * code does not decode it, decoding is done completely on client-side.
 */
struct entry {
	// uint32_t timestamp_ticks : 20;
	// uint16_t raw_data : 12;
	uint32_t bits;
} __packed;

#define ENTRY_TIMESTAMP_MASK 0xfffff000
#define ENTRY_TIMESTAMP_SHIFT 12
#define ENTRY_DATA_MASK 0xfff
#define ENTRY_DATA_SHIFT 0

typedef uint16_t index_t;

void push_buffer(struct entry e);
index_t get_push_idx();

bool pop_buffer(struct entry *e);
index_t get_pop_idx();

#endif /* __RINGBUFFER_H__ */
