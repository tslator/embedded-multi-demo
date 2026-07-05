# Embedded Multi Demo

## Goal
Create a standardized, multi-target embedded portfolio demonstrating software portability across hardware, schedulers, and languages.
* Target Hardware: Raspberry Pi Pico (RP2040), STM32 (NUCLEO-L476RG)
* Execution Environments: Bare Metal (Superloop), FreeRTOS, Zephyr
* Implementation Languages: Purse C (C11), Purse C++ (C++17/20)
* Build System: CMake with ```FetchContent``` for autmated dependency management (Pico SDK, STM32CubeHAL, FreeRTOS, GoogleTest)
* Testing Frameworks: Ceedling (C) and GoogleTest (C++)

## Proposed Projects

1. Multicore Sensor Fusion Pipeline (IMU + Encoder + ADC)
2. High-Rate UART/USB Telemetry Router
3. Motor Control Loop + Safety Monitor
4. Networked Command Processor (MQTT/CAN/BLE)
5. Filesystem + Sensor Logger (SD Card + RTC + ADC)

### Multicore Sensor Fusion Pipeline
A dual-core real-time pipelin that ingests multiple sensor streams and produces fused state estimates.

Demonstrates:
- Interrupts:
  - IMU data-ready interrupt
  - Quadrature encoder ISR
  - ADC conversion complete
- Synchronization:
  - shared state vector protection
- Serialization:
  - tasks wait on semaphores signaled by ISRs)
- Cache / Memory behavior:
  - DMA Buffers
  - Zero-copy queues
  - Cache coherency on MCUs with data cache
- RTOS primitives:
  - Task priorities
  - Queues for sensor messages
  - Timers for perioic fusion updates
  - Event groups for multi-sensor readiness
 
### High-Rate UART/USB Telemetry Router
A system that receives high-frequency binary packets, parses them and routes them to multiple destinations (SD card, BLE, display)

Demonstrates:
- Interrupts:
  - UART RX or USB CDC
- Serialization:
  - RX ISR gives a semaphore to wake the parser task
- Synchronization:
  - Mutex around shared telemetry buffer or ring buffer
- Cache / Memory:
  - DMA-driven UART
  - Circular buffers
  - Cache maintenance on MCUs with D-cache
- RTOS primitives:
  - Stream buffers
  - Message queues
  - Priority inversion scenarios (logging vs routing)

 ### Motor Control Loop + Safety Monitor
 A deterministic motor control loop running at 1-10kHz, with a separate safety supervisor task.

 Demonstrates:
 - Interrupts:
   - PWM timer interrupt
   - Overcurrent interrupt
 - Synchronization:
   - Mutex for shared motor state (current, velocity, duty cycle)
 - Serialization:
   - Semaphore from PWM ISR to wake control loop task
 - Cache / Memory:
   - Fast access to control parameters
   - Optional use of tightly coupled memory (TCM)
 - RTOS primitives:
   - High-priority periodic tasks
   - Watchdog integration
   - Event groups for fault conditions
  
### Networked Command Processor (MQTT / CAN / BLE)
A command interpreter that receives commands from multiple transports and executes them deterministrically.

Demonstrates:
- Interrupts:
  - CAN RX interrupt
  - BLE event interrupt
- Synchronization:
  - Mutex around shared command table
- Serialization:
  - Semaphores for waking command parser tasks
- Cache / Memory
  - DMA for CAN
  - Cache alignment for network buffers
- RTOS primitives:
  - Multiple queues feeding a single parser
  - Priority-based command execution
  - Deferred interrupt processing
 
### Filesystem + Sensor Logger (SD card + RTC + ADC)
A periodic logger that writes sensor data to SD card while maintaining time synchronization.

Demonstrates:
- Interrupts:
  - RTC tick
  - ADC conversion complete
  - SDIO DMA complete
- Synchronization:
  - Mutex around filesystem access
- Serialization:
  - Semaphore from DMA ISR to wake the writer task
- Cache / Memory:
  - Cache line flushes for SDIO DMA
  - Large buffers for batching writes
- RTOS primitives:
  - Timers for periodic logging
  - Queues for sensor data
  - Priority tuning to avoid SD card latency spikes


# Unit Testing
The Ceedling test environment is completely hosted within the workspace.

```text
embedded-multi-demo/
├── Gemfile
├── ruby/
│   ├── installation/   <-- Extracted Ruby engine
│   └── bundle/         <-- Third-party gems (Ceedling/Benchmark)
└── projects/
    └── 01_sensor_fusion/
        └── project.yaml
        └── tests
            └── folders mimic the source folders under <project>, e.g. 01_sensor_fusion
        └── run_tests.sh
        └── run_test.bat
```

The Ruby environment is not under source control so it must be created after cloning the repository as follows:

1. Create the folder
```bash
mkdir -p ruby/bundle
```
2. Extract the 7-zip archive: this file is under source control
3. Rename the parent folder of the `bin` folder to `installation`
4. Move `installation` folder under `ruby` folder
5. Execute the following commands from the project root folder, e.g. embedded-multi-demo
```bash
export PATH="$PWD/ruby/installation/bin:$PATH"
bundle config set --local path 'ruby/bundle'
bundle install
```
6. Change to `projects/01_sensor_fusion` and run the following script:
```bash
./run_tests.sh # or ./run_tests.bat
```
