# Material Sandbox Beta

## Goal

The first public-feeling build of KineticMagi should be a large, messy, replayable sandbox where the player can conjure materials, mix elements, discover reactions, and interact with physics.

This beta is not a campaign, MMO, or polished creator marketplace. It is a proof that the core toy is fun.

**Beta promise:**

> Spawn into a huge yard, conjure matter, mix elements, discover materials, and build whatever survives.

## Primary Player Loop

1. Enter the sandbox map.
2. Select an element, material, or shape.
3. Preview placement in the world.
4. Place it if the target volume is valid.
5. Push, pull, stack, heat, cool, electrify, or mix it.
6. Discover new material recipes and reactions.
7. Use discoveries to build larger structures or cause chain reactions.
8. Clear, reset, or keep experimenting.

## Sandbox Map: The Yard

Working name: **The Yard**

The Yard should be huge and practical. It should feel like a physics testing ground, not a finished fantasy zone.

Recommended zones:

- Flat open build field
- Ramp and slope testing area
- Pit / drop testing area
- Water basin
- Heat / fire testing pad
- Electricity testing pad
- Magnet testing pad
- Dummy enemy/NPC area
- Tall tower / vertical traversal test
- Breakable object range
- Bridge-building gap
- Cleanup/reset station

## Placement Rule

Players can conjure materials anywhere valid except when the placement volume overlaps a living actor.

No lore explanation is needed.

### Valid targets

- Ground
- Walls
- Existing nonliving objects
- Empty supported space, if the placement type allows it
- Water or other fluids, if the material supports placement there

### Invalid targets

- Player character
- NPCs
- Enemies
- Animals
- Living plant actors
- Any actor/component tagged as living

### Feedback

- Valid placement preview: green/normal ghost
- Invalid placement preview: red ghost
- Invalid reason text: `Blocked by character` or `Cannot place there`
- Release while invalid: no spawn

## Starter Controls

Temporary beta controls can be simple.

- `1` Stone / mineral block
- `2` Wood plank
- `3` Iron block or beam
- `4` Copper wire/strip
- `5` Glass/silicon sand
- `6` Water sphere/blob
- `7` Hydrogen gas bubble
- `8` Oxygen gas bubble
- `9` Carbon dust/chunk
- `0` Heat / spark tool
- `Q/E` Rotate preview
- Left click: place
- Right click: delete aimed player-created object
- Hold interact: kinetic grab/pull
- Undo: remove last placed object
- Reset: clear player-created sandbox objects

## Matter System Philosophy

The player should discover matter through experimentation.

Avoid showing the full periodic table as 118 menu buttons in the first beta. Use a small set of primitive elements and material forms, then let discoveries expand the available toolkit.

The system should be chemistry-inspired, not a strict chemistry simulator.

## Starter Elements

Recommended initial set:

| Element / Input | Symbol | Gameplay behavior |
|---|---:|---|
| Hydrogen | H | Light gas, buoyant, explosive with oxygen and spark |
| Oxygen | O | Feeds fire, combines into water/glass/oxides |
| Carbon | C | Fuel, dust, graphite/diamond-like paths, steel ingredient |
| Iron | Fe | Heavy, strong, magnetic, rusts |
| Copper | Cu | Conductive, useful for electricity puzzles |
| Silicon | Si | Sand/glass/crystal base |
| Sodium | Na | Reactive with water |
| Chlorine | Cl | Toxic gas, salt ingredient |
| Sulfur | S | Flammable, acid ingredient |
| Calcium | Ca | Stone/lime/mineral ingredient |
| Nitrogen | N | Gas, pressure/cold interactions |
| Mercury | Hg | Heavy liquid metal, advanced hazard material |

For the first playable prototype, this can be reduced further to H, O, C, Fe, Cu, Si, Na, Cl, Water, Heat, Cold, and Electricity.

## Starter Discoveries

| Discovery | Recipe / Trigger | Gameplay Use |
|---|---|---|
| Water | Hydrogen + Oxygen | Liquid flow, cooling, buoyancy |
| Steam | Water + Heat | Pressure, lift, launch force |
| Ice | Water + Cold | Slippery platforms, temporary bridges |
| Glass | Silicon + Oxygen + Heat | Transparent fragile panes, lenses |
| Steel | Iron + Carbon + Heat | Stronger beams and structures |
| Rust | Iron + Oxygen + Water | Weakens metal over time |
| Salt | Sodium + Chlorine | Crystal/mineral blocks |
| Explosion | Hydrogen + Oxygen + Spark | Force burst |
| Shock Water | Water + Electricity | Hazard field |
| Magnetized Iron | Iron + Electricity | Pulls or attracts metal objects |
| Smoke | Carbon + Heat + Oxygen | Visibility blocker |
| Acid | Sulfur + Oxygen + Water | Dissolves selected materials |

## Material Forms

The same element/material can be conjured in different forms.

| Form | Examples | Use |
|---|---|---|
| Block | Stone cube, iron cube | Weight, structure, obstacle |
| Beam | Wood plank, steel girder | Bridges, levers, supports |
| Sheet | Glass pane, copper sheet | Barriers, reflectors, circuits |
| Wire | Copper wire, iron chain | Conductivity, links, machines |
| Powder | Carbon dust, sulfur powder | Reactions, fuel, mixing |
| Liquid | Water, mercury, acid | Flow, pressure, hazards |
| Gas | Hydrogen, oxygen, chlorine | Buoyancy, combustion, area effects |
| Pad | Cloth pad, rubber-like pad | Bounce, cushioning, movement |

## Discovery Journal

Working name: **Matter Journal**

When the player discovers a new material or reaction, show a simple notification:

```text
New Discovery: Glass
Recipe: Silicon + Oxygen + Heat
Properties: brittle, transparent, reflective
Added to Matter Journal
```

The journal should track:

- Discovered name
- Recipe inputs
- Required condition, if any
- Properties
- Gameplay uses
- Whether it can be directly conjured afterward

## Required Beta Systems

### Must-have

- Large sandbox map
- Placement preview
- Valid/invalid placement check
- Living actor overlap block
- Spawn material actor
- Delete aimed created object
- Clear all created objects
- Kinetic grab/pull interaction
- At least four solid materials
- At least three simple reactions
- Matter Journal stub

### Should-have

- Hotbar or radial menu
- Undo last placement
- Object limit / stability budget
- Reaction notification UI
- Dummy enemies/NPCs for blocking tests
- Basic save/load for sandbox layouts

### Later

- Multiplayer sandbox
- Published realms
- Remix/recast flow
- Social-runtime affinity recommendations
- Creator profiles
- Full material marketplace/archive

## Technical Direction

### Actor tags/components

Use a simple actor/component marker for living placement blocking.

Suggested options:

- `Living` gameplay tag
- `Living` actor tag
- `ULivingMatterComponent`
- Pawn inheritance check as fallback

Placement should fail if the candidate volume overlaps any actor considered living.

### Data-driven material definitions

Each material should eventually be data-driven.

Possible fields:

```text
MaterialId
DisplayName
ElementInputs
Form
Mass
Hardness
Flexibility
Conductivity
Magnetism
Flammability
Buoyancy
Fragility
Toxicity
TemperatureBehavior
ValidPlacementRules
ReactionTags
UnlockState
```

### Reaction definition shape

```text
ReactionId
RequiredInputs
RequiredCondition
ConsumedInputs
ProducedMaterial
ProducedEffect
ForceOutput
HeatOutput
JournalDiscoveryId
```

Example:

```text
ReactionId: hydrogen_oxygen_spark_explosion
RequiredInputs: Hydrogen, Oxygen
RequiredCondition: Spark
ProducedEffect: RadialForceBurst
JournalDiscoveryId: explosion
```

## First Vertical Slice

Build one prototype room inside The Yard with:

1. Placement preview.
2. Stone block, wood plank, iron cube, water blob.
3. Player cannot place over self or dummy NPC.
4. Player can stack blocks to reach a ledge.
5. Player can drop iron into water.
6. Player can apply electricity to water and trigger a visible reaction.
7. Player receives one Matter Journal notification.
8. Player can clear all spawned objects and retry.

If this is fun for ten minutes, the larger design is worth expanding.
