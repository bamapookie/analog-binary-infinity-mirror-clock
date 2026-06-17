# Roadmap

This project is being resurrected. This document tracks the plan so the context
isn't lost again. Items are grouped into tracks; tracks can progress in parallel.

## Guiding principles

- **GPS-first, offline-first.** A big part of the appeal is that the clock needs
  no Wi-Fi and never needs to be set. It should work anywhere on Earth out of the
  box. Network time (NTP) may be added later as an *optional* convenience, never a
  requirement.
- **Losing power or signal is a feature.** With no valid time data, the clock
  should flash `12:00` like an old VCR until it reacquires a fix. (The firmware
  already approximates this in `set12oClock()`; finishing/polishing this is a
  firmware task.)
- **Unreadable on purpose.** The whole point is a clock that is hard to read but
  still gives an analog sense of time passing.

## Track A — Repository & firmware hygiene (in progress)

- [x] Remove EAGLE autosave/backup cruft and IDE files from version control.
- [x] Fix the non-compiling `adafruit_strand_test` sketch and the duplicated
      block in `fastled_strand_test`.
- [x] Move dev sketches out of `test/` (reserved for unit tests) into `examples/`.
- [x] Add dual licensing (MIT firmware + CERN-OHL-S hardware).
- [ ] Add continuous integration that builds the firmware with PlatformIO.
- [ ] Pin `lib_deps` versions for reproducible builds.
- [ ] **Firmware correctness pass** (from the resurrection audit):
  - [x] Mark ISR-shared globals `volatile` (`ppsStartOfSecondMillis`,
        `ppsStartOfMinuteMillis`, `currentMinute`, `nextLeapMinute`,
        `numberOfLeapSeconds`).
  - [x] Fix the "torn read" guard in `setClock()` (replaced the self-comparison
        with a `noInterrupts()` snapshot of the minute/start-of-minute pair).
  - [x] Bound the array writes in `updateStrandSeconds()` (now wraps with `MOD`)
        so the seconds feature is safe to re-enable.
  - [ ] Actually populate `numberOfLeapSeconds` / `nextLeapMinute` from the parsed
        GPS leap-second fields, or remove the dead parsing — today the leap-second
        feature is wired up but never fed any data.
  - [ ] Finish or remove the commented-out seconds "ripple" rendering.
  - [x] Polish the VCR `12:00` flash for the no-signal state — reviewed; the loop
        already blinks `set12oClock()` at ~1 Hz while GPS time is invalid, which is
        the intended VCR behavior. No change needed beyond confirming it.

## Track B — Library extraction (next)

Break the monolithic `main.cpp` into reusable, host-testable libraries. The key
win is making the time and rendering logic framework-agnostic so it can be unit
tested on a PC via `pio test`, turning `test/` into real tests.

- `AbimcTimekeeper` — leap-second-aware, sub-second UTC clock with pluggable time
  *sources* (PPS-disciplined GPS today; NTP/RTC later) behind one interface.
- `AbimcRender` — pure logic: time -> per-ring binary bit -> hand angle, with
  fractional-pixel anti-aliasing and the seconds ripple. Operates on an abstract
  pixel buffer so it has no FastLED dependency.
- `AbimcConfig` — settings model (seconds on/off, timezone, custom-tz rules) plus
  persistence. Back-end for the magnet UI below.
- `abimc_pcb` — already a library; keep, versioned per board revision.

## Track C — Hardware v2 (deferred; no hardware change yet)

Captured for when we're ready. Current decision is to **keep the existing
ItsyBitsy M4 hardware** and do the software work first.

- **MCU candidates if/when we revise:** ESP32-S3 (Wi-Fi/BLE, NTP, OTA, BLE config)
  or RP2350/Pico 2 W (PIO-driven LEDs, deterministic timing). Either would keep
  GPS as the primary, offline time source per the guiding principles.
- **LEDs:** stay with APA102-class (separate clock line = high PWM refresh and
  global brightness, which makes smooth moving hands look good). Move to the
  smaller APA102-2020 / SK9822 package to enable the desk-size shrink; custom
  flex-PCB rings at small diameters.
- **GPS:** upgrade the 6-year-old MTK3339/XA1110 to a u-blox MAX-M10S — better
  sensitivity, lower power, still has PPS, and reports current + pending leap
  seconds directly (UBX-NAV-TIMELS), which would greatly simplify the leap-second
  code.
- **RTC:** add a battery-backed DS3231 so time survives power loss without
  reacquiring a fix.
- **Power:** budget carefully — 504 APA102 at full white is a theoretical ~30 A;
  rely on brightness limiting and the sparse hands-only display.

## Track D — Desk-sized enclosure (future)

The original clock was too big. Shrink it to a desk-sized unit.

- Recompute ring radii for a ~100-150 mm diameter target.
- Smaller infinity-mirror stack and a 3D-printed housing.
- Let this constrain the Track B/C PCB so the electronics are designed for
  desk-scale ring counts up front, even if the first v2 build is larger.

## Track E — Magnet-based configuration (future)

Configure the clock by dropping small magnets into recesses molded into the 3D
printed back panel. Magnets are non-volatile with no battery, so configuration
survives power loss inherently.

- **Sensing:** digital Hall-effect sensors (e.g., DRV5032), one per recess; use
  *omnipolar* parts so magnet orientation doesn't matter. Route many sensors
  through an I2C I/O expander (MCP23017) or analog mux to save GPIO.
- **Dial option:** a single 3D Hall sensor (TLV493D / MLX90393) under a rotatable
  magnet senses angle — e.g., rotate to dial in a UTC offset.
- **Proposed config bits:**
  - 1 recess = seconds hand on/off.
  - 1 recess = DST on/off.
  - A small group of recesses (binary) or a Hall dial = timezone / UTC offset.
  - 1 recess = "use custom profile", loading a personal timezone
    (e.g. `North_America/Margaritaville`) defined once over BLE/serial and saved.
