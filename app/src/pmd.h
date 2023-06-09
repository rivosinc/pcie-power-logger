/*
 * SPDX-FileCopyrightText: 2023 Rivos Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Definitions for ElmorLabs PMD, taken from device datasheet. */
#ifndef __PMD_H__
#define __PMD_H__

#include <zephyr/kernel.h>

#define PMD_I2C_ADDR 0x12

#define PMD_PCIE1_VOLTAGE_I2C_CMD 0x3
#define PMD_PCIE1_CURRENT_I2C_CMD 0x5
#define PMD_PCIE2_VOLTAGE_I2C_CMD 0x7
#define PMD_PCIE2_CURRENT_I2C_CMD 0x9
#define PMD_EPS1_VOLTAGE_I2C_CMD 0xb
#define PMD_EPS1_CURRENT_I2C_CMD 0xd
#define PMD_EPS2_VOLTAGE_I2C_CMD 0xf
#define PMD_EPS2_CURRENT_I2C_CMD 0x11

/* Alternate / undocumented regs that implement latching in newer PMD FW. */
#define PMD_PCIE1_VOLTAGE_ALT_I2C_CMD 0x80
#define PMD_PCIE1_CURRENT_ALT_I2C_CMD 0x82
#define PMD_PCIE2_VOLTAGE_ALT_I2C_CMD 0x84
#define PMD_PCIE2_CURRENT_ALT_I2C_CMD 0x86
#define PMD_EPS1_VOLTAGE_ALT_I2C_CMD 0x88
#define PMD_EPS1_CURRENT_ALT_I2C_CMD 0x8a
#define PMD_EPS2_VOLTAGE_ALT_I2C_CMD 0x8c
#define PMD_EPS2_CURRENT_ALT_I2C_CMD 0x8e

#endif /* __PMD_H__ */
