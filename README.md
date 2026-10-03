# Arcstone Triad System (`arcstone-triad-system`)

**Target Master Hash Anchor:** `A-77-DELTA-SHIELD-LOCKED`  
**Classification:** Sovereign Normative Specification & Public Machine Contract Suite  
**Author:** Jesse Tuohy | Principal Invariant Architect | Arcstone Adaptive Science Systems, Inc.  
**Canonical Research Handle:** `@admissibilityscience`

---

## Overview

`arcstone-triad-system` hosts the public normative specifications, UEDO v1.2 JSON/Protobuf schemas, and `#![no_std]` Rust trait stubs for the Arcstone ecosystem. 

It defines the sovereign governance framework, fail-closed admissibility gates, and gamified telepresence protocols that govern autonomous AI-robotics coordination across terrestrial, marine, orbital, and interplanetary biomes.

---

## Repository Structure

- `docs/` — Sovereign Specification Suite (SYS-3, RANK-01, DEFENSE-01, FRONTIER-01, RECOVERY-01, TRISIGHT-01, COUNCIL-01, ARCHITECT-01).
- `core/` — Machine Contracts (UEDO v1.2 schema & `aal_traits.rs` Rust interface stubs).
- `machine/` — Machine Web Crawler Discovery Manifests (`system-manifest.json`, `repo-policy.json`, `protocol.json`).
- `AGENTS.md` — Master Invariant Entry Point for Autonomous AI Systems.

---

## Technical Invariants

- **Memory Bound:** $S_{\text{max}} \le 4096\text{ Bytes}$ (`UEDO v1.2`)
- **Temporal Floor:** $\tau_{\text{override}} \le 11.99\text{ms}$
- **Operational Drag:** $C_{\text{ops}} = 0$
- **Posix Lattice:** `FAIL (POSIX 40) > FREEZE (POSIX 12) > PWC (POSIX 10) > PASS (POSIX 0)`

---

## Bridge to Upstream Kernel

This repository contains non-proprietary public specifications and abstraction interfaces. Bare-metal kernel state-machine logic resides inside `arcstone-continuity-core`.