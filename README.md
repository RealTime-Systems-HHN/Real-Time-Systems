# Real-Time Systems

# Zephyr Measurement Node
## Overview

This project implements the measurement node used in the distributed RTOS communication benchmark. Running on Zephyr RTOS, the node is responsible for initiating benchmark transactions, measuring end-to-end communication latency using the STM32 hardware cycle counter, and collecting statistical performance data from the distributed communication chain.

The measurement node generates a GPIO trigger signal, waits for the propagated response from the communication path, records the elapsed processor cycles, and stores the collected latency samples for subsequent analysis. The implementation was developed to evaluate communication latency across heterogeneous RTOS environments including FreeRTOS, Zephyr, and Azure RTOS ThreadX.

## Features

* Hardware cycle-counter based latency measurement.
* Automated collection of 5000 benchmark samples.
* Warm-up phase before data acquisition.
* GPIO-based trigger and response detection.
* CSV export of latency measurements through the serial interface.
* Support for multiple communication paths.
* Designed for real-time latency benchmarking and performance evaluation.

## Hardware Setup

This project is configured for the STM32 Nucleo-F439ZI development board.

## Software Requirements

* Zephyr RTOS
* Zephyr SDK
* West Build System
* ARM GCC Toolchain
* STM32 HAL Drivers

## Pin Configuration

### Trigger Output

* PA3 – Generates the benchmark trigger signal and starts the communication sequence.

### Path A Response Input

* PA0 – Receives the return signal from the FreeRTOS forwarding node.

### Path B Response Input

* PA15 – Receives the return signal from the Zephyr forwarding node.

### Path C Response Input

* PB1 – Receives the return signal from the ThreadX forwarding node.

### User Button

* PC13 – Starts a new benchmark execution.

## Benchmark Workflow

1. User presses the onboard button to start the benchmark.
2. The measurement node performs a warm-up phase.
3. A GPIO trigger signal is transmitted through PA3.
4. The communication path propagates the signal through the forwarding nodes.
5. The return signal is received on the selected input pin.
6. The hardware cycle counter records the elapsed latency.
7. The process is repeated for 5000 benchmark iterations.
8. Results are exported in CSV format through the serial terminal.

## Collected Metrics

The benchmark records:

* Minimum latency
* Maximum latency
* Average latency
* Latency range
* Individual sample values
* End-to-end communication latency in processor cycles

## Build and Flash Instructions

### 1. Configure the Project

1. Open the project directory.
2. Verify GPIO assignments and board configuration.
3. Confirm that the appropriate benchmark path is selected.

### 2. Build the Application

```bash
west build -b nucleo_f439zi
```

### 3. Flash the Firmware

```bash
west flash
```

### 4. Run the Benchmark

1. Open a serial terminal.
2. Reset the board.
3. Press the USER button (PC13).
4. Wait for completion of 5000 benchmark iterations.
5. Copy the generated CSV data for analysis in Grafana.

## Benchmark Context

This implementation was developed as part of a master's thesis investigating end-to-end latency characteristics of heterogeneous RTOS communication systems using Zephyr, FreeRTOS, and Azure RTOS ThreadX running on STM32F439ZI development boards.


## FreeRTOS Forwarding Node
## Overview

This project implements a forwarding node using FreeRTOS on an STM32 Nucleo-F439ZI development board. The node receives an external GPIO signal from a preceding board and immediately forwards the signal to the next node in the communication chain. The application was developed as part of a distributed RTOS communication benchmark used to evaluate end-to-end latency across heterogeneous embedded systems.

## Features

* Real-time GPIO signal forwarding using FreeRTOS tasks.
* Low-latency interrupt-driven signal processing.
* Support for daisy-chain communication architectures.
* Heartbeat LED for RTOS status indication.
* Designed for latency benchmarking and performance evaluation.

## Hardware Setup

This project is configured for the STM32 Nucleo-F439ZI development board.

## Software Requirements

* STM32CubeMX
* STM32CubeIDE
* FreeRTOS (CMSIS-V2 integration)
* STM32 HAL Drivers

## Pin Configuration

### Input Signal

* PA4 – Receives the incoming signal from the previous node.

### Forwarding Output

* PA3 – Forwards the received signal to the next node.

### Return Signal

* PB1 – Returns the propagated signal to the measurement node.

## Project Workflow

1. Receive signal on PA4.
2. Trigger FreeRTOS processing task.
3. Forward signal through PA3.
4. Return signal through PB1.
5. Continue operation for subsequent benchmark iterations.

## Build and Flash Instructions

### 1. STM32CubeMX Configuration

1. Open the `.ioc` file.
2. Verify GPIO and clock configuration.
3. Generate code using STM32CubeMX.

### 2. Build in STM32CubeIDE

1. Open the project.
2. Verify application code in `Core/Src/main.c`.
3. Build the project.
4. Confirm that compilation completes successfully.

### 3. Flash to Target Board

1. Connect the STM32 board through ST-Link.
2. Click Run or Debug.
3. Select STM32 Cortex-M C/C++ Application.
4. Wait until programming completes successfully.

# Zephyr Forwarding Node

## Overview

This project implements a forwarding node using Zephyr RTOS on an STM32 Nucleo-F439ZI development board. The application participates in a distributed communication benchmark by receiving a GPIO signal from a preceding node and forwarding it to the next stage of the communication chain. The system was designed to evaluate deterministic communication latency in heterogeneous RTOS environments.

## Features

* GPIO interrupt-based signal forwarding.
* Real-time event handling using Zephyr RTOS.
* Support for multi-node daisy-chain communication.
* Lightweight forwarding architecture.
* Suitable for latency measurement and benchmarking.

## Hardware Setup

This project targets the STM32 Nucleo-F439ZI development board.

## Software Requirements

* Zephyr SDK
* West Build System
* STM32CubeIDE (optional)
* ARM GCC Toolchain

## Pin Configuration

### Input Signal

* PA4 – Receives the signal from the previous communication node.

### Forwarding Output

* PA3 – Sends the signal to the next node.

### Return Signal

* PB1 – Returns the propagated signal to the measurement board.

## Project Workflow

1. Detect incoming GPIO interrupt.
2. Execute Zephyr callback handler.
3. Forward signal to the next communication stage.
4. Return signal to the measurement node.
5. Repeat for all benchmark iterations.

## Build and Flash Instructions

### 1. Configure the Project

1. Open the project directory.
2. Verify board configuration and GPIO assignments.
3. Confirm that the correct target board is selected.

### 2. Build the Application

```bash
west build -b nucleo_f439zi
```

### 3. Flash the Firmware

```bash
west flash
```

Alternatively, the generated binary can be imported into STM32CubeIDE and flashed using ST-Link.

# Azure RTOS ThreadX Forwarding Node

## Overview

This project implements a forwarding node using Azure RTOS ThreadX on an STM32 Nucleo-F439ZI development board. The node receives a GPIO signal from the previous communication stage and forwards it through the distributed benchmark network. The implementation was developed to evaluate communication latency across multiple RTOS platforms operating within the same distributed architecture.

## Features

* Azure RTOS ThreadX based signal forwarding.
* Low-overhead GPIO event processing.
* Deterministic communication behaviour.
* Multi-node distributed communication support.
* Benchmark-oriented implementation for latency evaluation.

## Hardware Setup

This project is configured for the STM32 Nucleo-F439ZI development board.

## Software Requirements

* STM32CubeMX
* STM32CubeIDE
* Azure RTOS ThreadX
* STM32 HAL Drivers

## Pin Configuration

### Input Signal

* PA4 – Receives the incoming signal from the previous node.

### Forwarding Output

* PA3 – Sends the signal to the next communication stage.

### Return Signal

* PB1 – Returns the propagated signal to the measurement board.

## Project Workflow

1. Receive signal through GPIO input.
2. Process the event within ThreadX.
3. Forward the signal to the next communication stage.
4. Return the propagated signal to the measurement node.
5. Continue execution for benchmark operation.

## Build and Flash Instructions

### 1. STM32CubeMX Configuration

1. Open the project `.ioc` file.
2. Verify ThreadX middleware configuration.
3. Generate project code.

### 2. Build in STM32CubeIDE

1. Open the project.
2. Verify application source files.
3. Build the firmware.
4. Confirm successful compilation.

### 3. Flash to Target Board

1. Connect the board using ST-Link.
2. Click Run or Debug.
3. Select STM32 Cortex-M C/C++ Application.
4. Wait until programming completes successfully.


