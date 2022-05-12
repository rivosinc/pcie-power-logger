/*
 * Copyright (c) 2022 Rivos Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Definitions for ElmorLabs PMD, taken from device datasheet. */
#ifndef __PMD_H__
#define __PMD_H__

#include <zephyr.h>

#define PMD_I2C_ADDR 0x12

#define PMD_PCIE1_VOLTAGE_I2C_CMD 0x3
#define PMD_PCIE1_CURRENT_I2C_CMD 0x5
#define PMD_PCIE2_VOLTAGE_I2C_CMD 0x7
#define PMD_PCIE2_CURRENT_I2C_CMD 0x9

#endif /* __PMD_H__ */
