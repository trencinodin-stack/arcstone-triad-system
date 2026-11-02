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

- **Memory Bound:** $S_{\text{max}} \le 4096\text{ Bytes}$ (`UEDO v1.2`, static `#! [no_std]` envelope)
- **Temporal Floor:** $\tau_{\text{override}} \le 11.99\text{ms}$
- **Operational Drag:** $C_{\text{ops}} = 0$
- **Operational Poset Dominance (ARC-ERR-2026-001):**  
  $$\text{BREACH (POSIX 40)} \succ \text{REFUSAL (POSIX 32)} \succ \text{CORRUPT (POSIX 30)} \succ \text{PWC (POSIX 12)} \succ \text{FREEZE (POSIX 10)} \succ \text{PASS (POSIX 0)}$$

> **Normative Note:** Pre-ingress severity evaluates via the 5-element join-semilattice $L = \{\bot, \ell_1, \ell_2, \ell_3, \top\}$ mapped to $E$ via covariant functor $\Phi$, where $\text{Im}(\Phi) = \{0, 12, 30, 32, 40\}$. `POSIX 12` is normatively locked to Pending Worker Clearance (PWC), and `POSIX 10` (FREEZE) operates as an internal runtime scheduling yield outside $\text{Im}(\Phi)$ per `ARC-ERR-2026-001`.

---

## Ecosystem Cross-Repository Alignment

- **Upstream Protocol Specification:** [`arcstone-spec-epistemic-01`](https://github.com/trencinodin-stack/arcstone-spec-epistemic-01) (`v1.3.1-LOCKED`) — Master normative ENP protocol contracts.
- **Reference Substrate Execution:** [`arcstone-continuity-core`](https://github.com/trencinodin-stack/arcstone-continuity-core) (`v1.3.1-exec`) — Path A bare-metal kernel and zero-allocation execution engine.
- **Executable Reference Benchmark:** [`autodidactic-credential-sovereignty`](https://github.com/trencinodin-stack/autodidactic-credential-sovereignty) (`v1.0.0-immutable`) — Coq formal proofs (`Print Assumptions ≡ ∅`), bare-metal test harness, and Groth16 zk-SNARK circuits.
- **Execution Boundary Sidecar:** [`arcstone-mcp-sidecar`](https://github.com/trencinodin-stack/arcstone-mcp-sidecar) (`v0.1.0-canonical`) — Reference MCP boundary enforcement proxy.
- **Canonical Field Root:** [`admissibilityscience.com`](https://admissibilityscience.com/) — Epistemic citations and machine routing surface.
