# HW Design Note 001

## Title
RP2040-safe interface for 5V quadrature encoder outputs (US Digital EM2)

## Context
The US Digital EM2 encoder provides 5V push-pull outputs for Channel A, Channel B, and optional Index. The Raspberry Pi Pico (RP2040 GPIO) is a 3.3V device and is not 5V tolerant.

Design goal:
- Protect RP2040 inputs from overvoltage
- Preserve edge quality for quadrature decoding
- Keep implementation practical for breadboard and bench bring-up

## Electrical Constraint
RP2040 input voltage must not exceed the IO rail by more than the absolute maximum margin. In practice, treat GPIO as 3.3V-only and avoid direct 5V connection.

System requirement:
- Shared ground between encoder supply and RP2040 board

## Recommended Solutions

### Recommended prototype path: Passive resistor divider per channel
Use one divider for each signal used (A, B, and Index if used).

Per-channel values:
- Rtop = 2.2 kOhm
- Rbottom = 3.9 kOhm

Output estimate:
- Vout = 5.0 * (3.9 / (2.2 + 3.9)) = 3.20V

Current estimate:
- I = 5.0 / (2.2k + 3.9k) = 0.82 mA

Wiring:
1. Encoder output -> 2.2 kOhm (Rtop)
2. Divider node -> RP2040 GPIO
3. Divider node -> 3.9 kOhm (Rbottom) -> GND

Why:
- Cheap and simple
- Current load stays within typical encoder drive capability
- Works well for many moderate-speed encoder applications

### Side note (if moving toward a production-oriented revision): Dual-supply level translator
Use a dual-supply translator such as SN74LVC2T45 for A/B channels.

Power and control:
- VCCA -> 3.3V (RP2040 domain)
- VCCB -> 5V (encoder domain)
- DIR -> set for encoder-to-RP2040 direction
- OE -> do not float; hold disabled until both rails are valid, then enable

Signal mapping:
- EM2 A -> translator B-side input -> translator A-side output -> RP2040 GPIO
- EM2 B -> translator B-side input -> translator A-side output -> RP2040 GPIO

Index channel:
- SN74LVC2T45 is 2-bit only. For A/B/Index, use either:
1. Two dual-bit devices, or
2. A wider translator device with explicit pin mapping

Why:
- Strong signal integrity
- Good high-speed margin
- Cleaner behavior across cable length and noise environments

Note:
- For this repository, treat this as an escalation path or future PCB option, not the default bench build path.

## Why other common options are not recommended
- 74AHCT125 at 5V rail: output high near 5V is unsafe for RP2040 GPIO
- NPN inverting stage: inverts logic and adds software/hardware complexity for quadrature phase handling
- BSS138-style bidirectional shifter boards: intended for open-drain buses; not ideal for push-pull encoder outputs

## Design Decision for Project 01
Default to resistor dividers for initial bring-up and documentation consistency in this prototype-focused repository.

If this were moving toward production, we would consider moving to a translator IC in a PCB revision when either condition is true:
1. Edge quality degrades at target max RPM/cable length
2. Missed counts appear during stress testing

## Validation Checklist
1. Confirm common ground between encoder and Pico
2. Measure logic-high at RP2040 pin (target near 3.2V with divider)
3. Verify rise/fall edges on RP2040 pin under worst cable length
4. Run max-speed spin test and verify no count loss
5. Verify correct A/B phase order and direction
6. If Index is enabled, verify one pulse per revolution
7. Power-sequence test:
   - Encoder on first, Pico second
   - Pico on first, encoder second
   - Ensure no invalid back-power behavior

## Notes for Future PCB
If this were moving toward production, we would consider the following:
- Include footprints for both resistor-divider path and translator path if board area allows
- Add test points on A, B, and Index after level shifting
- Keep traces short and route A/B as a matched pair where practical
- Add optional RC filtering footprint only if bench data shows ringing/noise issues

## Status
Approved for prototype use with resistor divider values 2.2 kOhm / 3.9 kOhm per channel.
