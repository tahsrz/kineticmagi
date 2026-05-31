# KineticMagi Monorepo

`KineticMagi` is the repo root for the game client and social simulation runtime.

## Structure

- `Source/`, `Content/`, `Config/`, `KineticMagi.uproject`: Unreal game client.
- `social-runtime/`: .NET social packet ingress + scoring sandbox.
- `docs/`: design and context documents.
- `shared-schema/`: reserved for shared packet/schema definitions.
- `server/`: reserved for authoritative MMO services.
- `tools/`: build/content tooling.
- `deploy/`: packaging and deployment scripts.

## Quick Start

1. Unreal client:
   - Open `KineticMagi.uproject` in UE 5.7.

2. Social runtime sandbox:
   - `cd social-runtime`
   - `dotnet run`

## Current Status

- `LBP_PhysicsSpringMode` is integrated and compiling in the Unreal module.
- `social-runtime` validates TAH v2 packet headers, routes social payloads, and runs bounded affinity/hook scoring.
