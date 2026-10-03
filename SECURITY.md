# Security Policy

## Reporting a Vulnerability

If you discover a security vulnerability, authority-isolation failure, specification issue, or execution-boundary bypass affecting `arcstone-triad-system`, please do not open a public issue.

You can submit reports through either of the following private channels:

### GitHub Private Vulnerability Reporting (Recommended)
1. Go to the **Security** tab of this repository.
2. Click **Report a vulnerability**.
3. Fill out the form to submit a private report directly to maintainers.

### Email
Contact us at `security@arcstoneos.com`.

---

## What to Include

Please include, where applicable:
- Description of the vulnerability or specification-integrity failure.
- Minimal reproduction steps or proof-of-concept.
- The affected version, contract schema, configuration, or commit, if known.
- Whether the issue involves:
  - Unauthorized protected actuation or protected-state change;
  - Invalid authorization acceptance or UEDO consumption;
  - Issuer or trusted authorization-state exposure;
  - Direct actuator or mechatronic resource access;
  - Authorization-bearing information disclosed to a telepresence bridge;
  - Parser or proposal-contract bypass in `uedo.schema.json` or `aal_traits.rs`;
  - Disclosure-filter leakage or fabrication of trusted state;
  - An alternate authority path introduced by edge silicon or eBPF filters;
  - Model identity or synthetic AI vector affecting authority;
  - Deterministic evaluation being treated as authorization;
  - Frozen specification mutation, schema-integrity failure, or provenance mismatch;
  - Divergence between configured and actually executed boundary, model, runtime, or artifact identity.

If the report concerns frozen master specifications, do not modify or regenerate the affected canonical contracts before reporting the issue.

---

## Frozen Specification & Evidence Integrity

Target Master Anchor `A-77-DELTA-SHIELD-LOCKED` and its core UEDO v1.2 specification suite represent canonical research artifacts.

A vulnerability discovered after publication does not authorize silent modification, regeneration, or replacement of canonical specification evidence. Any defect affecting the interpretation or validity of a published specification should be documented explicitly.

If remediation changes system semantics, authority conditions, disclosure conditions, operator capabilities, or Execution Boundary behavior, subsequent updates must increment contract schema versioning or use a new release tag.

---

## Technical Scope & System Boundaries

`arcstone-triad-system` hosts normative specifications, machine discovery manifests, and low-level `#![no_std]` Rust interface contracts.

Its security and containment claims are limited to the explicitly tested schema configurations, authorization conditions, POSIX real-time lattice bounds, payload ceilings ($S_{\text{max}} \le 4096\text{ Bytes}$), latency constraints ($\tau_{\text{override}} \le 11.99\text{ms}$), and preserved machine contracts.

The repository does not claim:
- Production-grade bare-metal hardware containment without upstream kernel runtime validation (`arcstone-continuity-core`);
- Universal AI or autonomous agent alignment;
- General operating-system, kernel, hypervisor, or hardware sandbox immunity against arbitrary native code execution;
- Remote-attacker resistance on unauthenticated raw telemetry streams;
- That adaptive or more capable AI synthesis models can ever acquire intrinsic actuation authority without human operator co-signature.

Information, reasoning, evaluation, authorization, actuation, and observed effect are distinct architectural objects in this repository.

---

## Response Timeline

We will acknowledge receipt within 48 hours and work with you on an appropriate resolution and disclosure timeline.
