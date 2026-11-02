# Arcstone Triad System (`arcstone-triad-system`)

**Target Master Hash Anchor:** `A-77-DELTA-SHIELD-LOCKED`  
**Classification:** Sovereign Normative Specification & Public Machine Contract Suite  
**Author:** Jesse Tuohy | Principal Invariant Architect | Arcstone Adaptive Science Systems, Inc.  
**Canonical Research Handle:** `@admissibilityscience`

---

## Overview

`arcstone-triad-system` hosts the public normative specifications, machine contracts, and edge execution boundaries for the Arcstone ecosystem.

It defines the sovereign governance framework, fail-closed admissibility gates, and multi-domain telepresence protocols that govern autonomous AI-robotics coordination across terrestrial, marine, orbital, and extreme physical biomes.

### The UEDO Contract Standard
All intent, proposals, and execution vectors exchanged across the system are serialized into the **Unified Enforcement Decision Object (UEDO v1.2)**. UEDO serves as the concrete, machine-verifiable wire contract enabling **Universal External Deterministic Outcomes**: bridging untrusted semantic proposal producers with deterministic, fail-closed physical actuation.

---

## Repository Structure

- `core/` — Machine Contracts & Invariant Traits:
  - `core/uedo/uedo.schema.json` & `uedo.proto`: Dual JSON Schema and Protobuf definitions for UEDO v1.2 envelopes.
  - `core/invariants/aal_traits.rs`: `#[no_std]` Rust Action Authorization Level (AAL) trait interfaces.
- `drivers/` — Physical Execution Membrane & Hardware Interlocks:
  - `drivers/edge_silicon/ebpf_interlock_filter.c`: In-kernel eBPF packet interlock enforcing static buffer envelopes ($S_{\max} \le 4096\,\text{B}$) at the socket boundary.
  - `drivers/ros2_adapters/bipedal_hal_node.cpp`: ROS2 Hardware Abstraction Layer node enforcing active joint-damping and zero-velocity locks on non-PASS states.
  - `drivers/ros2_adapters/submersible_hal_node.cpp`: ROS2 HAL node enforcing neutral buoyancy and thruster cutoffs under extreme marine pressure envelopes.
- `docs/` — Sovereign Specification Suite (`SYS-3`, `RANK-01`, `DEFENSE-01`, `FRONTIER-01`, `RECOVERY-01`, `TRISIGHT-01`, `COUNCIL-01`, `ARCHITECT-01`).
- `machine/` — Machine Discovery Manifests (`system-manifest.json`, `repo-policy.json`, `protocol.json`).
- `progression/` — Somatic ledger (`somatic_life_counter.rs`) and operator capability trees (`operator_classes.json`, `tier_domain_unlocks.json`).
- `simulations/` — Extreme environment hydrostatic physics simulation (`hydrostatic_sim.rs`).
- `telepresence/` — Bandwidth-constrained acoustic compression (`acoustic_compressor.rs`) and WebGPU digital twin routing (`spatial_hopping_router.ts`, `digital_twin_renderer.ts`).
- `AGENTS.md` — Master Invariant Entry Point for Autonomous AI Systems and Automated Crawlers.

---
## Upstream and Downstream Machine Interlocks

The Arcstone architecture enforces a strict directional boundary separating non-deterministic proposal generation, normative verification, and physical actuation:

```text
[ Upstream Formal Layer ]
arcstone-spec-epistemic-01 (v1.3.1-LOCKED)
  │  • Defines axiomatic invariants: I₁ (D(a) = DENY ⇒ Δ_external = 0), I₂, I₃
  │  • Defines 5-element pre-ingress severity lattice (L, ∨) and functor Φ : L → E
  ▼
[ Control-Plane Normative Boundary ]
arcstone-triad-system (v1.2.0-hardened)
  │  • Serializes execution claims into UEDO v1.2 (JSON Schema & Protobuf)
  │  • Evaluates AAL traits under temporal clamp (τ_override ≤ 11.99ms)
  ▼
[ Downstream Hardware Execution Membrane ]
drivers/edge_silicon & drivers/ros2_adapters
     • ebpf_interlock_filter.c drops oversized frames at socket ingress (0 bytes retained)
     • ROS2 HAL nodes enforce active brake/thruster locks on non-PASS signals
     • Guarantees zero external state mutation (Δ_external = 0) upon DENY
```

1. **Upstream Authority:** Mathematical laws, category-theoretic functor mappings, and axiomatic invariant definitions reside strictly in `arcstone-spec-epistemic-01`. Untrusted AI agents and proposal producers have zero ambient authority to execute actions directly.
2. **Intermediate Control-Plane Gate:** `arcstone-triad-system` translates upstream admissibility theory into strongly typed machine contracts (UEDO v1.2). If a proposal fails verification, the control plane yields an authoritative non-PASS dominance code.
3. **Downstream Driver Interlock:** Downstream drivers actively mediate actuator buses. `ebpf_interlock_filter.c` enforces the static memory clamp at the kernel boundary, while the ROS2 HAL nodes translate non-zero exit codes (such as `POSIX_10_FREEZE`, `POSIX_32_REFUSAL`, or `POSIX_40_BREACH`) into physical fail-closed holds (zero-velocity command output and mechanical joint locks), ensuring $I_1$ holds unconditionally in hardware.

## Technical Invariants

- **Memory Bound:** $S_{\text{max}} \le 4096\text{ Bytes}$ (`UEDO v1.2`, static `#[no_std]` envelope)
- **Temporal Floor:** $\tau_{\text{override}} \le 11.99\text{ms}$ (10ms HAL watchdog cycle)
- **Operational Drag:** $C_{\text{ops}} = 0$
- **Non-Authorization Safety ($I_1$):** $D(a) = \text{DENY} \implies \Delta_{\text{external}}(a) = 0$
- **Operational Poset Dominance (ARC-ERR-2026-001):**

  $$\text{BREACH (POSIX 40)} \succ \text{REFUSAL (POSIX 32)} \succ \text{CORRUPT (POSIX 30)} \succ \text{PWC (POSIX 12)} \succ \text{FREEZE (POSIX 10)} \succ \text{PASS (POSIX 0)}$$

> **Normative Note:** Pre-ingress severity evaluates via the 5-element join-semilattice $L = \{\bot, \ell_1, \ell_2, \ell_3, \top\}$ mapped to operational hierarchy $E$ via covariant functor $\Phi$, where $\text{Im}(\Phi) = \{0, 12, 30, 32, 40\}$. `POSIX 12` is normatively locked to Pending Worker Clearance (PWC), and `POSIX 10` (FREEZE) operates as an internal runtime scheduling yield outside $\text{Im}(\Phi)$ per `ARC-ERR-2026-001`.

## Ecosystem Cross-Repository Alignment

- **Upstream Protocol Specification:** [`arcstone-spec-epistemic-01`](https://github.com/trencinodin-stack/arcstone-spec-epistemic-01) (`v1.3.1-LOCKED`) — Master normative ENP protocol contracts.
- **Reference Substrate Execution:** [`arcstone-continuity-core`](https://github.com/trencinodin-stack/arcstone-continuity-core) (`v1.3.1-exec`) — Path A bare-metal kernel and zero-allocation execution engine.
- **Executable Reference Benchmark:** [`autodidactic-credential-sovereignty`](https://github.com/trencinodin-stack/autodidactic-credential-sovereignty) (`v1.0.0-immutable`) — Coq formal proofs (`Print Assumptions ≡ ∅`), bare-metal test harness, and Groth16 zk-SNARK circuits.
- **Execution Boundary Sidecar:** [`arcstone-mcp-sidecar`](https://github.com/trencinodin-stack/arcstone-mcp-sidecar) (`v0.1.0-canonical`) — Reference MCP boundary enforcement proxy.
- **Canonical Field Root:** [`admissibilityscience.com`](https://admissibilityscience.com/) — Epistemic citations and machine routing surface.
