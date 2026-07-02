## Purpose
Describe the software architecture in terms of layers, including the major software components, responsibilities, and data flow.

## Architecture
- The application never talks directly to USB, UART, or CAN.  It produces `SystemState` and `HealthState`, which are passed to the telemetry subsystem.
- The telemetry subsystem is explicitly layered into:
  - Data Model
  - Serialization
  - Transport
  **Reinforces the design goal that changing from USB to UART should only affect the transport layer**
- `SensorFrame`, `SystemState`, and `HealthState` become the key data structures in the system:
    - `SensorFrame` : frame-referenced sensor inputs with per-sensor freshness metadata
    - `SystemState` : processed outputs
    - `HealthState` : queue and timing metrics for runtime observability
- Inter-core communication is represented as a software component rather than exposing implementation details like queues or shared memory.

## RTOS Task Partitioning

The principal partitioning occurs at the process core level. Core 0 runs a bare-metal acquisition superloop and is responsible for interfacing with hardware and acquiring data from the sensors. Core 1 runs FreeRTOS and is responsible for processing sensor data, health monitoring, and telemetry.

Health monitoring follows a separate `HealthState` path rather than being embedded in `SystemState`. `TASK_HEALTH` aggregates queue depth and sample-rate metrics from acquisition and IPC boundaries, plus processing-latency metrics from fusion timing signals, then publishes these diagnostics through a dedicated mutex-protected structure consumed by telemetry. This keeps ownership clear: `SystemState` represents fused motion outputs, while `HealthState` represents runtime observability and pipeline health.


**Key Design Decisions**

|Component|Architecture Role|
|---|---|
|Strict Ownership|Core 0 exclusively owns all hardware peripherals.  If Core 1 wants sensor data, it must wait for the Queue.|
|Coherent Frames|Core 0 produces a `SensorFrame` with one frame timestamp and per-sensor freshness flags so Fusion can distinguish new, carried-forward, and stale values across asynchronous IMU, Encoder, and ADC updates.|
|Isolation|The "Sensor Fusion" software is abstracted from the hardware, facilitating testing.|
|Telemetry Decoupling|Telemetry runs at its own rate, e.g. 10Hz, by reading the last known state through a Mutex, preventing slow serial prints from interfering with the 100Hz Fusion loop.|


## Architecture Diagram

```mermaid
flowchart BT

    %% Hardware Layer
    subgraph HW["Hardware"]
        IMU["IMU"]
        ENC["Quadrature Encoder"]
        ADC["Analog Inputs"]
    end

    %% Driver Layer
    subgraph DRV["Driver Layer"]
        IMU_DRV["IMU Driver"]
        ENC_DRV["Encoder Driver"]
        ADC_DRV["ADC Driver"]
    end

    %% Acquisition Layer
    subgraph ACQ["Data Acquisition"]
        ACQ_TASK["Sensor Acquisition"]
        FRAME["SensorFrame\n(frame timestamp + sensor data + freshness metadata)"]
    end

    %% Processing Layer
    subgraph PROC["Processing"]
        IPC["Inter-Core Communication"]
        FUSION["Sensor Fusion"]
        STATE["SystemState"]
        HEALTH["System Health"]
    end

    %% Telemetry Layer
    subgraph TEL["Telemetry"]
        MODEL["Data Model"]
        SERIAL["Serialization"]
        TRANSPORT["Transport"]
    end

    %% Outputs
    subgraph OUT["External Interfaces"]
        USB["USB CDC"]
        UART["UART"]
        CAN["CAN (Future)"]
    end

    %% Hardware to Drivers
    IMU --> IMU_DRV
    ENC --> ENC_DRV
    ADC --> ADC_DRV

    %% Drivers to Acquisition
    IMU_DRV --> ACQ_TASK
    ENC_DRV --> ACQ_TASK
    ADC_DRV --> ACQ_TASK

    %% Acquisition Pipeline
    ACQ_TASK --> FRAME
    FRAME --> IPC
    IPC --> FUSION
    FUSION --> STATE

    %% Health Monitoring
    ACQ_TASK --> HEALTH
    IPC --> HEALTH
    FUSION --> HEALTH

    %% Telemetry
    STATE --> MODEL
    HEALTH --> MODEL
    MODEL --> SERIAL
    SERIAL --> TRANSPORT

    %% Output Transports
    TRANSPORT --> USB
    TRANSPORT --> UART
    TRANSPORT --> CAN
```

## RTOS Task Diagram

```mermaid
flowchart TB

    %% Core 0 - The Deterministic Hardware Engine (Bare Metal / No RTOS)
    subgraph C0["Core 0: Bare-Metal Acquisition Engine"]
        direction TB
        
        subgraph C0_HW["Hardware Drivers (Owner)"]
            IMU_DRV["IMU Driver\n(SPI0)"]
            ENC_DRV["Encoder Driver\n(PIO)"]
            ADC_DRV["ADC Driver\n(ADC0)"]
        end

        ISR_HNDL["ISR / DMA Handler\n(Real-Time IRQ)"]
        
        LOOP_ACQ["Acquisition Service Loop\n(Superloop)\nRole: Timestamp & Frame Assembly"]
    end

    %% Communication Channel - Hardware & Memory Primaries
    subgraph IPC["Inter-Core Transport (Shared SRAM)"]
        SF_RING["SensorFrame SPSC Ring Buffer\n(Shared Memory)"]
        HW_LOCK["RP2040 Hardware Spinlocks\n(Atomic Access Protection)"]
        FIFO_SIG["Hardware FIFO\n(Core-to-Core Signaling)"]
    end

    %% Core 1 - The Application & Processing Core (FreeRTOS)
    subgraph C1["Core 1: FreeRTOS Execution Environment"]
        direction TB
        
        TASK_FUSION["Task: Sensor Fusion\n(High Priority)\nRole: Mahony / Madgwick"]
        
        SH_STATE["Shared System State\n(Write-Locked Struct)"]
        SH_HEALTH["Shared Health State\n(Write-Locked Struct)"]
        
        TASK_TEL["Task: Telemetry\n(Normal Priority)\nRole: Serialization"]
        
        TASK_HEALTH["Task: Health Monitor\n(Low Priority)\nRole: Stats & Jitter Check"]
    end

    %% Data Flow - Core 0
    ISR_HNDL -.->|"Signal/Ready"| LOOP_ACQ
    LOOP_ACQ -->|"Poll/Read"| C0_HW
    
    %% Frame Production via IPC
    LOOP_ACQ -->|"Writes Frame"| SF_RING
    LOOP_ACQ -->|"Interrupt Signal"| FIFO_SIG
    
    %% Consumption on Core 1
    FIFO_SIG -.->|"Unblock Task"| TASK_FUSION
    SF_RING -->|"Read Frame"| TASK_FUSION
    
    %% Core 1 Internal Data Sharing
    TASK_FUSION -->|"Update"| SH_STATE
    SH_STATE -->|"Read"| TASK_TEL

    %% Health Metrics Path
    LOOP_ACQ -->|"Drop/High-Water Stats"| SH_HEALTH
    TASK_FUSION -->|"Latency Stats"| SH_HEALTH
    SH_HEALTH -->|"Read stats"| TASK_HEALTH
    TASK_HEALTH -->|"Publish diagnostics"| TASK_TEL
    
    %% Final Output
    TASK_TEL -->|"USB CDC / Serial"| PC["Development PC"]
```