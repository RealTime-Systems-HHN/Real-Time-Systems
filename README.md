# Real-Time-Systems

STM32 FreeRTOS Pin Tracker

Overview:
This project is an embedded application built for the STM32 microcontroller. It utilizes the STM32 HAL library and FreeRTOS to manage real-time hardware interactions. The system concurrently handles continuous GPIO state tracking and a background heartbeat LED.

Features

Real-Time Pin Tracking: Continuously monitors the state of an input pin and mirrors it to an output pin with a ≤10ms polling rate via a FreeRTOS task.
RTOS Heartbeat: Maintains a stable 500ms blinking cycle on a status LED to verify that the FreeRTOS kernel is running smoothly without blocking other operations.

Hardware Setup

This project is configured for an STM32 Nucleo board. 

Software Requirements:

STM32CubeMX: For peripheral configuration and code generation.
STM32CubeIDE: For compiling the C code and flashing the binary to the target board.
FreeRTOS: CMSIS-v2 OS wrapper (integrated via CubeMX).

The Pins That I’ve Used:

1.PA4 - Used for receiving the signal from the board 1 
2.PA3 - Used to send the signal to the board 2
3.PB1- Used to send back the signal to the board 1



Step-by-Step Setup and Flashing Guide

1. Project Initialization (STM32CubeMX)

If you are modifying the hardware peripherals:
1. Open the `.ioc` file in STM32CubeMX.
2. Make any necessary pinout or clock changes.
3. Click the Generate Code button in the top right corner. This updates the underlying hardware initialization files and safely injects them into the `/* USER CODE BEGIN */` sections of the codebase.

 2. Compilation (STM32CubeIDE)

1. Open the project folder in STM32CubeIDE.
2. In the `Core/Src/` directory, ensure `main.c` contains the updated FreeRTOS tasks.
3. Now build the program into the board
4. Verify in the bottom console that the build finishes with 0 errors, 0 warnings.

3. Flashing to the Target

1. Connect the STM32 Nucleo board to your machine via USB.
2. Click the green Play/Run Icon in the top toolbar.
3. Select STM32 Cortex-M C/C++ Application in the "Run As" dialog and click OK.
4. Wait for the console to output Download verified successfully and Resetting target.


Note: To access the files of Download the zip file from the repository named "Freertos.zip".
