/*
 * Copyright (c) 2022 Rivos Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * HTTP server task implementation, derived from Zephyr dumb_http_server
 * sample code. This code listens for a client connection and then sends all
 * available buffered data in a compressed format.
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <net/socket.h>
#include <kernel.h>

#include <net/net_pkt.h>

#include "ringbuffer.h"
#include "tasks.h"
#include "wire_structs.h"

#define BIND_PORT 8080

#define CHECK(r) { if (r == -1) { printf("Error: " #r "\n"); exit(1); } }

void http_task(void)
{
	int serv;
	struct sockaddr_in bind_addr;
	static int counter;
	int ret;

	serv = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	CHECK(serv);

	bind_addr.sin_family = AF_INET;
	bind_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind_addr.sin_port = htons(BIND_PORT);
	CHECK(bind(serv, (struct sockaddr *)&bind_addr, sizeof(bind_addr)));

	CHECK(listen(serv, 5));

	printf("Single-threaded dumb HTTP server waits for a connection on "
	       "port %d...\n", BIND_PORT);

	while (1) {
		struct sockaddr_in client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		char addr_str[32];
		int req_state = 0;

		int client = accept(serv, (struct sockaddr *)&client_addr,
				    &client_addr_len);
		if (client < 0) {
			printf("Error in accept: %d - continuing\n", errno);
			continue;
		}

		inet_ntop(client_addr.sin_family, &client_addr.sin_addr,
			  addr_str, sizeof(addr_str));
		printf("Connection #%d from %s\n", counter++, addr_str);

		/* Discard HTTP request (or otherwise client will get
		 * connection reset error).
		 */
		while (1) {
			ssize_t r;
			char c;

			r = recv(client, &c, 1, 0);
			if (r == 0) {
				goto close_client;
			}

			if (r < 0) {
				if (errno == EAGAIN || errno == EINTR) {
					continue;
				}

				printf("Got error %d when receiving from "
				       "socket\n", errno);
				goto close_client;
			}
			if (req_state == 0 && c == '\r') {
				req_state++;
			} else if (req_state == 1 && c == '\n') {
				req_state++;
			} else if (req_state == 2 && c == '\r') {
				req_state++;
			} else if (req_state == 3 && c == '\n') {
				break;
			} else {
				req_state = 0;
			}
		}

		while (1) {
			struct wire_data_t wire_data;
			wire_data.reserved1 = wire_data.reserved2 = wire_data.reserved3 = 0;
			const int tx_buf_size = 128;
			struct wire_data_t tx_buf[tx_buf_size];
			int buffered_cnt = 0;

			index_t pop_idx = get_pop_idx();
			/* Stuff and send tx_buf until ringbuffer is empty. */
			while (buffered_cnt < tx_buf_size && pop_buffer(&wire_data.e)) {
				wire_data.entry_type =
					char_from_sample_t(sample_t_from_buffer_idx(pop_idx));
				memcpy(&tx_buf[buffered_cnt++], &wire_data, sizeof(wire_data));
				pop_idx = get_pop_idx();
			}
			if (buffered_cnt != 0) {
				send(client, tx_buf, buffered_cnt * sizeof(wire_data), 0);
			}
			if (buffered_cnt < tx_buf_size) break;
		}

close_client:
		ret = close(client);
		if (ret == 0) {
			printf("Connection from %s closed\n", addr_str);
		} else {
			printf("Got error %d while closing the "
			       "socket\n", errno);
		}

	}
}

K_THREAD_DEFINE(http_task_id, STACK_SIZE * 16, http_task, NULL, NULL, NULL,
		HTTP_TASK_PRIORITY, 0, 0);
