# PCIe Power Logger

This project provides firmware and tools for logging PCIe power consumption
as recorded by an ElmorLabs PMD. Firmware continuously requests voltage +
current readings from the PMD and stores them for later http download.
Downloaded data can be decoded and visualized using included utilities.

The firmware is based on the Zephyr project runtime and runs on a Nucleo-F767ZI board.

It currently expects to have the Zephyr v2.7 sources in `ZEPHYR_BASE`.

## Hardware Requirements
- [STM Nucleo-F767ZI board](https://www.st.com/en/evaluation-tools/nucleo-f767zi.html). Other Zephyr-supported MCU boards should work with minimal tweaks.
- [ElmorLabs PMD (Power Measurement Device)](https://elmorlabs.com/product/elmorlabs-pmd-power-measurement-device/).

## Setup
1) Connect PMD PCIE1 and PCIE2 as pass-through for PCIe device under test.

2) Connect PMD I2C to Nucleo I2C:
* SCL: Green (PMD) to D15 (Nucleo CN7).
* SDA: White (PMD) to D14 (Nucleo CN7).
* GND: Black (PMD) to any ground pin on Nucleo.

3. Connect Nucleo to Ethernet (default IP address 192.0.2.1).

## Building / testing

- Building and flashing firmware:
```
$ west build -b nucleo_f767zi -s app
$ west flash
```

- Ingesting and Decoding Power Data (on Linux host): See [util/run.sh](https://gitlab.ba.rivosinc.com/rv/sw/int/pcie-power-logger/-/blob/rivos/main/util/run.sh).
```
$ ./util/run.sh (writes to output.csv)
```

- Visualizing Data: See [samples/README](https://gitlab.ba.rivosinc.com/rv/sw/int/pcie-power-logger/-/blob/rivos/main/samples/README) for ideas.
