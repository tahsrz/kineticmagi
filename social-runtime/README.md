# Social Runtime

Deterministic social simulation sandbox for TAH packets.

## What It Does

1. Validates binary packet headers and checksums (`TahBinaryHeaderV2Codec`).
2. Routes supported payloads (`TahNetworkIngress`).
3. Applies bounded asymptotic social scoring (`SocialEngine`).
4. Fires proactive threshold hooks on score crossings.

## Active Files

- `Program.cs`
- `TahBinaryHeaderV2.cs`
- `TahNetworkIngress.cs`
- `SocialEngine.cs`
- `CityHash.cs`

## Run

```powershell
dotnet run
```

The demo builds `AgentInteraction` packets, processes them through ingress, updates directional affinity, and prints hook events.
