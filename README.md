# NovaCore: Out‑of‑Order RISC‑V Simulator (Tomasulo)

## Overview
NovaCore is a C++ simulator that implements out‑of‑order execution using Tomasulo’s algorithm.  
It supports:  
- Integer ops: `add, sub, mul, div`  
- Loads/Stores: `lw, sw`  
- Register renaming (RAT), Reservation Stations, ROB, and CDB  

## Build & Run

```bash
# from project root
mkdir build && cd build
cmake ..
make

# run the sample program
./NovaCore ../programs/test.asm
