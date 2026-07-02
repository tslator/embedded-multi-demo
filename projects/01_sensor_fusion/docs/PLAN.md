# Purpose
Develop a reusable, real-time embedded application that demonstrates multicore programming, RTOS fundamentals, driver development, and sensor fusion.

This project serves as the foundation for later projects.  It emphasizes clean architecture and reusable components rather than application-specific functionality.

Primary implementation target:
- Raspberry Pi Pico (RP2040)

Rationale: 
- Pico is a new MCU with a strong ecosystem
- Pico has two MCU providing a unique solution and challenge
- First time working with the Pico

Comparison implementations:
This project is the first in a series designed to showcase support for multiple targets (Pico, STM32L476RG), languages (C, C++) and schedulers (Bare Metal, FreeRTOS, Zephyr).  Multi-dimensional projects present unique challenges in embedded systems and require careful partitioning and abstractions.

# Phase 1 - Define the System

## Objective
Define what the system does before writing code.

## Deliverables
- Hardware block diagram
- Software architecture diagram
- Data flow diagram
- Task list
- Sensor update rates
- Execution model and inter-core communication decision
- Success criteria

## System Overview

Sensors
- IMU (ICM-42688)
- Quadrature Encoder (US Digital EM2 Encoder)
- ADC (Potentiometer)

This combination of sensors provides the challenge of merging variable sensor rates:

|Sensor|Typical Rate|Trigger|
|---|---|---|
|IMU|1kHz|SPI Interrupt|
|Encoder|Variable|GPIO Interrupt|
|ADC|100Hz|Timer|

Processing
- Data acquisition
- Sensor synchronization
- Sensor fusion
- System health monitoring
- Telemetry

## Important
Keep transports independent from processing.

The application should never "know" whether telemetry leaves over USB or UART.

For this project, the execution model is intentionally split: Core 0 runs a bare-metal superloop for sensor acquisition, and Core 1 runs FreeRTOS for fusion, health, and telemetry.

## Success Criteria
The following measurable outcomes define a successful system. These become the
acceptance criteria for Phase 10 performance evaluation.

| # | Criterion | Target |
|---|---|---|
| SC-1 | `SensorFrame` production rate | 100 Hz ± 5% under nominal load |
| SC-2 | IMU samples captured | Zero missed frames over a 60-second run at 1 kHz ODR |
| SC-3 | End-to-end latency (sensor interrupt → telemetry byte out) | ≤ 50 ms at 10 Hz telemetry rate |
| SC-4 | Sensor fusion output | Realistic orientation estimate (visible response to physical rotation of IMU) |
| SC-5 | Inter-core frame transfer | No corruption or loss detected over a 60-second run |
| SC-6 | Telemetry transport swap | Switching USB CDC → UART requires changes only in `telemetry/transport/` |
| SC-7 | Host-based unit tests | All middleware and driver-logic tests pass without embedded hardware |

## Definition of Done
Explain the complete data flow from sensor interrupt to telemetry output without referencing implementation details.

# Phase 2 - Project Structure

## Objective
Create a project that is easy to understand and maintain.

## Deliverables
Proposed structure:
```text
project/
│
├── app/          # Application logic only — no hardware references
├── drivers/       # Peripheral drivers; call platform/ APIs exclusively
├── middleware/    # Sensor fusion, frame assembly, health monitor
├── telemetry/     # Data model, serialization, transport layers
├── platform/      # HAL/BSP: abstract SPI, GPIO, timer, UART interfaces
│                  #   e.g., platform_spi.h defines spi_transfer()
│                  #         platform/rp2040/ provides the RP2040 implementation
│                  #         platform/stm32/  provides the STM32 implementation
├── config/        # Compile-time configuration (sensor rates, queue depths, etc.)
├── tests/         # Host-based unit tests; hardware stubs live here
└── docs/
```
Each directory should have a single responsibility.

The `platform/` contract is the key portability seam: every driver and middleware
component calls only the abstract `platform_*.h` interfaces. Adding a new MCU target
means implementing those interfaces under a new `platform/<target>/` directory —
no driver or application code should need to change.

## Important
Avoid mixing driver code with application logic.

# Phase 3 - Platform Bring-up

## Objective
Verify the hardware and development environment.

## Tasks
- Build project
- Flash firmware
- Debug firmware
- Verify USB
- Verify both cores execute code
- Verify timing source

## Deliverables
- Blinky
- Serial console
- Dual-core heartbeat: Core 0 increments a counter in shared memory via the hardware FIFO;
  Core 1 reads it and toggles an LED or prints it over USB CDC. This exercises the
  inter-core FIFO path end-to-end before any sensor code is written.

## Definition of Done
Both cores execute independently and communicate successfully.

# Phase 4 - Driver Development

## Objective
Develop reusable drivers for each peripheral.

## Drivers
- IMU
- Encoder
- ADC
  
Each driver should expose a simple interface.

Example:
```c
int initialize(void);
int start(void);
void stop(void);
int read(void *out, size_t len);
```
The application should not know how the hardware works internally.

## Important
Drivers should never print or perform application logic.

## Definition of Done
Each driver can be tested independently.

# Phase 5 - Data Acquisition

## Objective
Collect synchronized sensor data.

## Tasks
Acquire
- IMU
- Encoder
- ADC

Timestamp each sample.
Package the readings into one structure.

Example:
```text
SensorFrame
    frame_timestamp
    imu (+ freshness metadata)
    encoder (+ freshness metadata)
    adc (+ freshness metadata)
```

## Important
Think of this as taking a photograph of the system.
Each `SensorFrame` is referenced to a single frame timestamp, and each sensor value carries freshness metadata indicating whether it was newly sampled, carried forward, or stale relative to that frame time.

## Definition of Done
Sensor frames are produced at a fixed rate.

# Phase 6 - Inter-Core Communication

## Objective
Transfer sensor frames safely between cores.

## Concepts

- RP2040 hardware inter-core FIFO
- Shared SRAM ring buffer
- Ownership transfer
- Optional RTOS queue/semaphore usage on Core 1 only

Core 0 - Produces `SensorFrames` from the bare-metal acquisition loop
Core 1 - Consumes `SensorFrames` under FreeRTOS

## Important
Never allow both cores to modify the same object simultaneously.
Use the hardware FIFO to signal frame availability and a shared-memory SPSC buffer to move ownership cleanly from Core 0 to Core 1.

## Definition of Done
Frames are transferred without corruption or loss.

# Phase 7 - Sensor Fusion

## Objective
Convert raw measurements into useful information.

## Initial Algorithm
Choose a simple algorithm.

Examples:
- Complementary Filter
- Mahony Filter

Delay more advanced filters until the architecture is proven.

## Output

```text
SystemState
    orientation
    velocity
    position
    timestamp
```

Health metrics are tracked in a separate `HealthState` structure (queue depth, sample rate, processing latency, and error counters) and merged in telemetry.

## Definition of Done
Moving the hardware produces realistic orientation estimates.

# Phase 8 - Telemetry

## Objective
Expose system information for observation and debugging.
The telemetry subsystem is independent of both the application and the transport.

## Design Layers

### Layer 1 - Data Model
Defines what information exists.

Example
- Timestamp
- IMU
- Encoder
- ADC
- Orientation
- Health
- Performance metrics

### Layer 2 - Serialization
Converts the data model into a wire message via a pluggable serializer interface.

**Serializer abstraction**
All serializers implement the same interface contract:
```c
typedef struct {
    int (*serialize)(const TelemetryFrame *frame, uint8_t *buf, size_t buf_len, size_t *out_len);
    int (*deserialize)(const uint8_t *buf, size_t len, TelemetryFrame *frame_out);
} Serializer;
```
The transport layer only sees `uint8_t *` bytes and a length — it is completely
independent of which serializer is active. Swapping formats is a single
configuration change.

**Primary format: MessagePack**
MessagePack is a compact, self-describing binary format with broad PC-side tooling
support (Python `msgpack`, Node.js, Wireshark dissectors). A zero-malloc embedded
implementation (`mpack`) is well-maintained and widely used in embedded projects.

- Compact wire encoding — typically 40–60% smaller than equivalent JSON
- No schema file required (unlike Protobuf)
- Human-readable tooling available for debugging
- `mpack` (https://github.com/ludocode/mpack): single-header, no heap, BSD licensed

**Comparison of candidate formats**

| Format | Size | PC Tooling | Embedded Complexity | Schema Required |
|---|---|---|---|---|
| Human-readable text | Large | Trivial | None | No |
| JSON | Large | Excellent | Moderate (avoid full parsers) | No |
| MessagePack ✓ | Small | Excellent | Low (`mpack`) | No |
| CBOR | Small | Good | Low (`cn-cbor`) | No |
| Binary (hand-packed) | Smallest | Poor (custom) | Low | Informal |

Human-readable text should be used during early bringup (Phase 3–6) for ease of
debugging over a serial terminal. MessagePack replaces it in Phase 8 for efficiency.
CBOR is architecturally identical to MessagePack and could be substituted by
swapping only the serializer implementation.

The implementation should make it easy to change formats later.

### Layer 3 - Transport
Responsible only for transmitting bytes.

Example
- USB CDC
- UART
- CAN (future)

Changing transports should not require changing the serialization logic.

## Definition of Done
Telemetry works over USB CDC.
Switching to UART requires only replacing the transport implementation.

# Phase 9 - System Health
## Objective
Monitor the application operation.
Monitor
- Queue depth
- Sample rate
- Processing latency
- Core utilization
- Missed samples
- Driver errors

Report health through the telemetry interface.

## Definition of Done
System health can be monitored without attaching a debugger.

# Phase 10 - Performance Evaluation

## Objective
Measure the real-time behavior of the system.

Measure
- Sensor latency
- Fusion latency
- End-to-End latency
- CPU usage
- Stack usage
- Memory usage
- Queue utilization

## Important
Measure first.
Optimize second.

## Definition of Done
Baseline performance numbers are documented.

# Phase 11 - Robustness
## Objective
Verify the system behaves correctly under failure conditions.

Test
- Sensor disconnect
- Queue overflow
- High CPU load
- Invalid sensor data
- Timing jitter
The system should continue operating whenever practical.

## Definition of Done
Failures are detected, reported and handled gracefully.


# Phase 12 - Documentation
## Objective
Document the project as if another engineer will maintain it.
Include
- Architecture overview
- Hardware connections
- Software organization
- Task descriptions
- Driver interfaces
- Telemetry protocol
- Performance measurements
- Lessons learned

## Definition of Done
A new developer can understand the project without reading the source code first.
