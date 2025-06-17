
# NovaCore – RISC-V Out-of-Order CPU Simulator (Tomasulo's Algorithm)

**Author:** Ayush Kumar
© 2025 Ayush Kumar

---

## Overview

NovaCore is a cycle-accurate RISC-V CPU simulator implementing **Tomasulo’s algorithm** for dynamic scheduling and out-of-order execution. It models realistic processor behavior, including register renaming, multi-cycle operations, and precise exception handling.

---

## Core Features

✅ **Out-of-order execution** using Tomasulo’s algorithm
✅ **Register renaming** with separate RATs for integer and floating-point registers
✅ **Multi-cycle execution** with distinct latencies per operation
✅ **In-order commit** via a reorder buffer (ROB)
✅ **Common data bus (CDB)** for result broadcasting
✅ **Memory operations**: load (lw) / store (sw)
✅ **Floating-point support**: fadd alongside integer operations

---

## Architecture Components

* **32 integer registers:** x0–x31
* **32 floating-point registers:** f0–f31
* **Register Alias Table (RAT):** resolves WAR/WAW hazards
* **Reservation Stations (RS):** buffer and schedule instructions dynamically
* **Reorder Buffer (ROB):** ensures in-order commit
* **Common Data Bus (CDB):** broadcasts execution results

---

## Supported Instructions & Latencies

| Instruction  | Type           | Latency (cycles) |
| ------------ | -------------- | ---------------- |
| `add`, `sub` | Integer ALU    | 1                |
| `mul`        | Integer ALU    | 3                |
| `div`        | Integer ALU    | 7                |
| `lw`         | Memory Load    | 2                |
| `sw`         | Memory Store   | 2                |
| `fadd`       | Floating Point | 4                |

---

## Architecture Diagram

```
+---------------------------+
|        Instruction         |
|          Queue             |
+---------------------------+
            |
            v
+-----------------+   +-----------------+
| Reservation      |   | Reservation      |
| Stations (Int)    |   | Stations (FP)    |
+-----------------+   +-----------------+
            |                   |
            v                   v
+----------------------------------------+
| Functional Units (ALU / FP ALU / MEM)  |
+----------------------------------------+
            |
            v
+----------------+
| Common Data Bus |
+----------------+
            |
            v
+---------------------------+
| Reorder Buffer (in-order)  |
+---------------------------+
            |
            v
+---------------------------+
|  Register File + RAT       |
+---------------------------+
```

---

## How Register Renaming Eliminates Hazards

The **Register Alias Table (RAT)** dynamically maps architectural registers to reorder buffer entries, eliminating:

* **WAR (Write After Read)** hazards
* **WAW (Write After Write)** hazards
  This allows independent instructions to execute out-of-order while preserving program correctness.

---

## Pipeline Stages

1️⃣ **Issue** – Dispatch instructions to reservation stations if slots available
2️⃣ **Execute** – Operate in functional unit after operands ready
3️⃣ **CDB Write** – Result broadcast to waiting RS / ROB entries
4️⃣ **Commit** – In-order writeback via ROB

---

## Example Assembly Program

```assembly
add x1, x0, x0
add x2, x0, x0
add x3, x1, x2
mul x4, x3, x3
sw x4, 0(x0)
lw x5, 0(x0)
```

---

## Example Execution Trace

```
Cycle 1: Issue add x1, x0, x0
Cycle 2: Execute add x1, x0, x0
Cycle 3: Write CDB add x1, x0, x0
...
Cycle 6: Commit lw x5, 0(x0)
```

---

## Example Code Snippet (RAT update)

```cpp
// Register renaming for destination register
RAT[inst.rd] = robIndex;
```

---

## Build & Run

```bash
mkdir build && cd build
cmake ..
make
./NovaCore ../programs/test.asm
```

---

## Future Work

* Branch prediction + speculative execution
* Multiple functional units
* Cache hierarchy (L1/L2 simulation)
* More RISC-V instruction coverage
* Performance metrics (CPI, throughput, stall cycles)

---

## License

© 2025 Ayush Kumar. All rights reserved.

---
