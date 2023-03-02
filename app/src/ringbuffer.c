/*
 * SPDX-FileCopyrightText: 2023 Rivos Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Circular buffer implementation for (quickly) storing and retrieving entries.
 * In order for this code to work correctly without locking, push_buffer() must
 * only be called by Thread A, pop_buffer must only be called by Thread B, and
 * the loads + stores must be atomic.
 */
#include <zephyr.h>

#include "ringbuffer.h"

/* 2^16 entries means unsigned overflow eliminates need for mod operator. */
typedef uint16_t index_t;
static struct entry ring_buffer[65536];
static index_t push_idx = 0;
static index_t pop_idx = 0;

void push_buffer(struct entry e) {
	ring_buffer[push_idx++] = e;
}

bool pop_buffer(struct entry *e) {
	/* Overflow causes empty buffer. */
	if (push_idx == pop_idx) return false;
	*e = ring_buffer[pop_idx++];
	return true;
}

/*
 * These functions allow the pusher and popper to store and retrieve  different
 * types of data (e.g., voltage and current) in the same ringbuffer without
 * using additional storage bits to distinguish between the types (e.g., store
 * voltage in even numbered idx, store current in odd numbered idx).
 */
index_t get_push_idx() {
	return push_idx;
}

index_t get_pop_idx() {
	return pop_idx;
}
