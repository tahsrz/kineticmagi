# TahNetworkSubsystem Integration Notes

## Overview
`UTahNetworkSubsystem` runs as a `UGameInstanceSubsystem`, so socket lifecycle persists across map loads and world transitions.

## Key Design Points
1. Background network reads run on a worker thread via `Async(EAsyncExecution::Thread, ...)`.
2. Parsed payloads are marshaled back to the Game Thread with `AsyncTask(ENamedThreads::GameThread, ...)` before broadcasting delegates.
3. Binary parsing is hardened and explicit:
   - little-endian field reads
   - strict bounds checks
   - overflow-safe length validation
   - checksum verification (FNV-1a 32-bit)
4. Parsing avoids raw `reinterpret_cast` packet-to-struct assumptions.

## Files
- `Source/kineticmagi/Public/TahNetworkSubsystem.h`
- `Source/kineticmagi/TahNetworkSubsystem.cpp`
- `Source/kineticmagi/Public/TahBinaryTypes.h`

## Build Configuration
`KineticMagi.Build.cs` must include:
- `Sockets`
- `Networking`

## Current Threading Safety
- Socket read loop stays off the Game Thread.
- Delegate broadcast occurs on Game Thread only.

## Production Follow-up
Current TCP handling processes each `Recv` chunk independently. For production MMO traffic, add frame reassembly (accumulator buffer + parse-complete-frame loop) to correctly handle packet fragmentation/coalescing.
