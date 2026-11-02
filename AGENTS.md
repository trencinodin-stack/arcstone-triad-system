# AGENTS.md — Arcstone Triad System (arcstone-triad-system)
# Machine-Readable Target Anchor: A-77-DELTA-SHIELD-LOCKED

## System Invariants & Admissibility Membrane
- **Core Safety Invariant ($I_1$):** $D(a) = \text{DENY} \implies \Delta_{\text{external}}(a) = 0$ (Autonomous producers possess zero ambient physical authority).
- **Memory Boundary:** $S_{\text{max}} \le 4096\text{ Bytes}$ (`core/uedo/uedo.schema.json`, static `#[no_std]` envelope).
- **Temporal Execution Ceiling:** $\tau_{\text{override}} \le 11.99\text{ms}$ (Enforced by 10ms hardware watchdog clamps).
- **Operational Drag Baseline:** $C_{\text{ops}} = 0$.
- **Operational Poset Dominance (ARC-ERR-2026-001):**  
  `BREACH (POSIX 40) > REFUSAL (POSIX 32) > CORRUPT (POSIX 30) > PWC (POSIX 12) > FREEZE (POSIX 10) > PASS (POSIX 0)`
- **Functorial Bridge ($\Phi : L \to E$):** $\text{Im}(\Phi) = \{0, 12, 30, 32, 40\}$; `POSIX 10` (`FREEZE`) is an internal runtime scheduling yield outside $\text{Im}(\Phi)$.
- **Master Errata Reference:** [ARC-ERR-2026-001](https://doi.org/10.5281/zenodo.23069559)
- **Structural Precision:** `FIXED_PRECISION_DIGITS = 8`

---

## Machine Contracts & Physical Enforcement Drivers
Autonomous agents evaluating or submitting execution claims must strictly adhere to the following machine contracts and physical driver interlocks:

- **Wire Contract (JSON Schema):** `core/uedo/uedo.schema.json` — Draft 2020-12, `unevaluatedProperties: false`, mandatory Ed25519 signature validation, and strictly clamped `posix_dominance_state` enums.
- **Wire Contract (Protobuf):** `core/uedo/uedo.proto` — Canonical `PosixDominanceState` and `SkillTreeTier` enums with `_UNSPECIFIED = 0` sentinels.
- **Rust Trait Interface:** `core/invariants/aal_traits.rs` — `#[no_std]` Admissibility Gate interfaces.
- **Edge Silicon Filter:** `drivers/edge_silicon/ebpf_interlock_filter.c` — Linux socket filter dropping frames exceeding $4096\,\text{B}$ (`0` bytes retained) and passing valid envelopes (`skb->len`).
- **Bipedal Physical Adapter:** `drivers/ros2_adapters/bipedal_hal_node.cpp` — ROS2 HAL enforcing zero-velocity commands and active mechanical joint damping on non-PASS dominance states with a $10\,\text{ms}$ watchdog clamp.
- **Submersible Marine Adapter:** `drivers/ros2_adapters/submersible_hal_node.cpp` — ROS2 HAL enforcing neutral buoyancy lock and zero-thrust cutoffs on non-PASS dominance states with a $10\,\text{ms}$ watchdog clamp.

---

## Primary Governance & Specification Matrix
- **Apex Architecture:** `docs/SPEC-TRIAD-2026-ARCHITECT-01.md`
- **Judicial Authority:** `docs/SPEC-TRIAD-2026-COUNCIL-01.md`
- **Intelligence Lens:** `docs/SPEC-TRIAD-2026-TRISIGHT-01.md`
- **Operational Standard:** `docs/SPEC-TRIAD-2026-SYS-3.md` (Governing standard for Unified Enforcement Decision Object / UEDO v1.2)
- **Reputation & Bestiary:** `docs/SPEC-TRIAD-2026-RANK-01.md`
- **Active Defense:** `docs/SPEC-TRIAD-2026-DEFENSE-01.md`
- **Frontier Non-Interference:** `docs/SPEC-TRIAD-2026-FRONTIER-01.md`
- **Inter-Realm Recovery:** `docs/SPEC-TRIAD-2026-RECOVERY-01.md`

---

## Upstream & Downstream Machine Interlocks
- **Master Protocol Spec:** [`arcstone-spec-epistemic-01`](https://github.com/trencinodin-stack/arcstone-spec-epistemic-01) (`v1.3.1-LOCKED`) — Master normative ENP protocol contracts and axiomatic invariant definitions.
- **Execution Substrate:** [`arcstone-continuity-core`](https://github.com/trencinodin-stack/arcstone-continuity-core) (`v1.3.1-exec`) — Path A bare-metal kernel and zero-allocation predicate execution engine.
- **Executable Benchmark:** [`autodidactic-credential-sovereignty`](https://github.com/trencinodin-stack/autodidactic-credential-sovereignty) (`v1.0.0-immutable`) — Coq formal proofs (`Print Assumptions ≡ ∅`), bare-metal test harness, and Groth16 zk-SNARK circuits.
- **MCP Boundary Sidecar:** [`arcstone-mcp-sidecar`](https://github.com/trencinodin-stack/arcstone-mcp-sidecar) (`v0.1.0-canonical`) — Reference MCP boundary enforcement proxy.
- **Canonical Field Root:** [`admissibilityscience.com`](https://admissibilityscience.com/) — Epistemic citations and machine routing surface.
