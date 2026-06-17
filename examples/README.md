# Examples

Standalone "bring-up" sketches used while developing the clock. Each folder is a
self-contained `main.cpp` that exercises one subsystem. They are **not** built by
the default PlatformIO environment and they are **not** unit tests (those belong in
`test/`). To run one, point an environment's `src_dir` at its folder, or copy its
`main.cpp` into `src/` temporarily.

| Sketch | What it verifies |
| --- | --- |
| `adafruit_strand_test` | Drives all six DotStar rings with a chase using the Adafruit_DotStar library. |
| `fastled_strand_test` | Same chase, but using FastLED (the library the firmware actually uses). |
| `gps_echo_test` | Bridges USB serial <-> GPS serial so you can talk to the GPS module directly and brute-force its baud rate. |
| `gps_pps_test` | Confirms the GPS fix and that the 1 PPS interrupt is firing. |
| `gps_time_test` | Parses the ZDA sentence into a time and measures PPS-to-NMEA delay. |
| `hand_jump_test` | Logs fractional hand positions to debug "jumping" of the rendered minute hand. |
| `pixel_flash_test` | Earlier iteration of the full clock render, kept for reference. |

These were the originals referenced as the "extras folder" in early notes.
