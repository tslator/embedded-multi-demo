## Motor Assembly
Leveraging a single motor and encoder from a disassembled [Arlobot robot](https://github.com/tslator/arlobot_freesoc).  The quadrature encoder (US Digital EM2) reads a 2000 count/rev disk with index.  The encoder is a 5v device and requires resister divider to interface with the Pico.

NOTE: An appropriate PWM signal is required to drive the motor electrically, but the motor shaft can be turned manually to trigger encoder counts.

## Power
Using a bread board power supply that provides 3.3v and 5v rails.

## Analog Input
A simple 10kΩ potentiometer is used to trigger the ADC.

## IMU Input
The ICM-42688 is connected to the SPI bus and interrupt.

### INT1 vs INT2 — Interrupt Pin Selection

The ICM-42688-P exposes two configurable interrupt output pins. The choice affects
ISR complexity and Core 0 workload.

| Attribute | INT1 | INT2 |
|---|---|---|
| **Configurable functions** | Data Ready, FIFO Watermark, FIFO Full, UI-AGC, Reset Done, PLL Ready | Data Ready, FIFO Watermark, FIFO Full only |
| **Default state** | Active after reset | Inactive after reset |
| **Output mode** | Push-pull or open-drain (configurable) | Push-pull or open-drain (configurable) |
| **Best use** | General-purpose: route whichever interrupt mode is chosen | Offload a second trigger (e.g. FIFO Full alert) to free INT1 for primary use |

**Recommended configuration for this project: INT1, FIFO Watermark mode**

Configuring INT1 to fire on a FIFO watermark (e.g., 8 samples) rather than
per-sample Data Ready reduces ISR entry frequency by 8× at 1kHz (125Hz ISR vs
1kHz ISR) while the FIFO buffers samples between bursts. The ISR drains the FIFO
in a single SPI burst read, reducing bus overhead and Core 0 interrupt load.

If a second interrupt is needed in future (e.g., hardware "FIFO nearly full" as an
overflow guard), INT2 can be enabled independently without reconfiguring INT1.

**INT1 is used for the initial implementation. The interrupt mode (Data Ready vs
FIFO Watermark) is deferred to the driver development phase (Phase 4), where
timing measurements will confirm which approach best meets the 1kHz frame rate.**

## Pico

General processing:
1. Sensor inputs are received
2. Placed into sensor queues stored (shared memory)
3. Retrieved from the sensor queues
4. Processed
5. Transmitted to Serial for Telemetry

### Core 0
#### Motor Encoder
Quadrature signals (Ch A and Ch B) are connected to GPIO 0 and GPIO 1 to take advantage of the PIO state machine for quadrature processing.

#### Analog Input
Potentiometer input is routed to the GPIO 26.

#### IMU Input
SPI bus communication connections are via GPIO 16/17/18/19 while the interrupt output is connected to GPIO 20 (INT1).
For this prototype, high-rate IMU reads may run up to 24MHz while setup/configuration writes follow the driver and datasheet write-limit guidance.

The interrupt mode used on INT1 (Data Ready vs. FIFO Watermark) is intentionally
deferred to Phase 4 driver development, where timing measurements will confirm the
best approach. See the INT1 vs INT2 section below and HW-DESIGN-NOTE-001 for
rationale. The diagram labels INT1 as "DRDY Interrupt" as a placeholder reflecting
the default power-on register state; this will be updated once the driver decision
is made.

### Shared Memory
Protected memory that holds sensor queues.

### Core 1
Retrieves data from sensor queues, processes, and transmits the data for monitoring.


## Hardware Diagram

```mermaid
flowchart TB
    subgraph PC["Development PC"]
        IDE["IDE / Serial Monitor"]
    end

    subgraph POWER["Power"]
        USB_PWR["USB 5V (VBUS)"]
        REG["3.3V Regulator (on-board)"]
        USB_PWR --> REG
    end

    subgraph PICO["Raspberry Pi Pico (RP2040)"]
        direction TB
        subgraph CORE0["Core 0 — Sensor Acquisition"]
            SPI_BUS["SPI0\nGPIO 16/17/18/19"]
            PIO["PIO State Machine\nGPIO 0 / GPIO 1"]
            ADC_CH["ADC0\nGPIO 26"]
            ISR["ISR Handler\nGPIO 20 (INT1)"]
        end
        subgraph CORE1["Core 1 — Fusion & Telemetry"]
            FUSION["Sensor Fusion Task\n(Complementary / Mahony)"]
            STATE["State Estimator\nrobot_state_t"]
            TELEM["Telemetry Task"]
        end
        subgraph SHARED["Shared Memory (Protected)"]
            QUEUE["Sensor Data Queue\n(IMU / Encoder / ADC)"]
        end
        USB_CDC["USB CDC\n(Power + Serial)"]

        CORE0 --> QUEUE
        QUEUE --> CORE1
        CORE1 --> TELEM
        TELEM --> USB_CDC
    end

    subgraph IMU["TDK ICM-42688-P"]
        ACCEL["3-Axis Accelerometer"]
        GYRO["3-Axis Gyroscope"]
        FIFO_BUF["2KB FIFO Buffer"]
        INT_PIN["INT1 (Data Ready)"]
    end

    subgraph MOTOR_ASSY["Motor Assembly"]
        MOTOR["DC Motor"]
        subgraph ENC_MOD["US Digital EM2 Encoder"]
            ENC_A["Channel A (5V Push-Pull)"]
            ENC_B["Channel B (5V Push-Pull)"]
        end
        MOTOR --- ENC_MOD
    end

    subgraph LEVEL["Resistor Divider\n(5V → 3.3V)"]
        RDA["Ch A: 2.2kΩ / 3.9kΩ"]
        RDB["Ch B: 2.2kΩ / 3.9kΩ"]
    end

    subgraph ANALOG["Analog Input"]
        POT["10kΩ Potentiometer\n3.3V / Wiper / GND"]
    end

    %% Power connections
    USB_PWR --> PICO
    REG --> IMU
    USB_PWR --> ENC_MOD

    %% PC connection
    PC <--> USB_CDC

    %% IMU to Pico
    IMU <-- "SPI\nRead up to 24MHz; config writes per driver/spec\nSCK/MOSI/MISO/CS" --> SPI_BUS
    INT_PIN -- "GPIO 20\nDRDY Interrupt" --> ISR

    %% Encoder to level shifter to Pico
    ENC_A --> RDA
    ENC_B --> RDB
    RDA -- "GPIO 0\n3.3V Logic" --> PIO
    RDB -- "GPIO 1\n3.3V Logic" --> PIO

    %% Potentiometer to Pico
    POT -- "GPIO 26\nAnalog Wiper" --> ADC_CH

    %% Styling
    style CORE0 fill:#1a3a5c,stroke:#4da6ff,stroke-width:2px,color:#ffffff
    style CORE1 fill:#1a3a1a,stroke:#4dff88,stroke-width:2px,color:#ffffff
    style SHARED fill:#3a2a00,stroke:#ffb347,stroke-width:2px,color:#ffffff
```