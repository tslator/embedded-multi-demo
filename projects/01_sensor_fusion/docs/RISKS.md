# Project Risks

Tracked risks, mitigations, and resolution status for Project 01 — Sensor Fusion.

| ID | Severity | Phase | Status | Title |
|---|---|---|---|---|
| RISK-001 | High | Phase 3 | Open | FreeRTOS Core 1 startup sequence |
| RISK-002 | High | Phase 6 | Open | SPSC ring buffer SRAM placement |

---

## RISK-001 — FreeRTOS Core 1 Startup Sequence

**Severity**: High  
**Affects**: Phase 3 (Platform Bring-up)  
**Status**: Open

### Description
The RP2040 SDK launches Core 1 via `multicore_launch_core1()`. The FreeRTOS
community port for RP2040 (`FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040`)
requires a specific startup coordination between cores. Getting the sequence wrong
produces a hard fault or a silent hang at startup with no obvious diagnostic output.

### Failure Mode
- Hard fault or infinite loop at boot if Core 1 scheduler is not started in the
  correct order relative to Core 0.
- No error message; symptom is simply that one or both cores stop executing.

### Mitigation
Use the canonical startup sequence:
1. Core 0 calls `multicore_launch_core1(core1_entry)`.
2. `core1_entry()` calls `vTaskStartScheduler()` — Core 1 runs FreeRTOS.
3. Core 0 proceeds with its bare-metal acquisition superloop (no FreeRTOS scheduler
   on Core 0).

Validate this as the **first** thing proven in Phase 3, before any sensor code.
The dual-core heartbeat deliverable (counter via FIFO → LED/USB print) is the
acceptance test for this risk.

### References
- `FreeRTOS-Kernel/portable/ThirdParty/GCC/RP2040/README.md`
- Pico SDK `multicore_launch_core1` documentation

### Resolution
_To be filled in during Phase 3._

---

## RISK-002 — SPSC Ring Buffer SRAM Placement

**Severity**: High  
**Affects**: Phase 6 (Inter-Core Communication)  
**Status**: Open

### Description
The `SensorFrame` SPSC ring buffer must reside in RAM accessible to both cores.
The RP2040 has 264KB of SRAM split into six banks (SRAM0–SRAM5). If the buffer
is placed in the default linker-script region, it may land in a bank subject to
contention when both cores simultaneously access adjacent data structures.

### Failure Mode
- Intermittent data corruption or unexpected latency spikes if both cores hit the
  same SRAM bank simultaneously.
- Symptom is non-deterministic and difficult to reproduce under light load; worsens
  under high sensor rates.

### Mitigation
Before Phase 6 implementation:
1. Add a `.shared_mem` section to the linker script targeting a dedicated SRAM bank
   (e.g., `SRAM4` or `SRAM5`).
2. Annotate the ring buffer declaration with the placement attribute:
   ```c
   __attribute__((section(".shared_mem")))
   static SensorFrameRingBuffer g_frame_ring;
   ```
3. Verify placement in the `.map` file after first build.

This is a one-line decision made before implementation; it is a painful linker
debug session if discovered during integration.

### References
- RP2040 Datasheet §2.6 — Address Map and SRAM bank layout
- `pico-sdk` `memmap_default.ld` linker script

### Resolution
_To be filled in during Phase 6._
