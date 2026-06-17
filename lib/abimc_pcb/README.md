# abimc_pcb

Board-specific pin mapping for the Analog Binary Infinity Mirror Clock PCB.

This header maps the six DotStar/APA102 strand data + clock lines, the GPS serial
port, and the GPS PPS interrupt pin to the microcontroller pins used by a given
revision of the ABIMC board. Select the revision by defining the matching macro
**before** including the header:

```cpp
#define ABIMC_V_1_3
#include <abimc_pcb.h>
```

Supported revisions:

| Macro | Notes |
| --- | --- |
| `ABIMC_V_1_0` | First fabricated board. |
| `ABIMC_V_1_3` | Strand 5/6 data pins swapped vs 1.0. |
| (default) | `ABIMC_V_1_4` layout, used when no revision macro is defined. |

All revisions share one clock line (`PIN_PA05`) across the strands and use
`Serial1` for the GPS, with `PIN_PA04` for PPS. See `src/abimc_pcb.h` for the
exact pin assignments.
