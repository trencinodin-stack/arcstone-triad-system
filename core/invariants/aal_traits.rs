#![no_std]

/// Target Master Hash Anchor for the Sovereign Arcstone Triad System
pub const TARGET_MASTER_HASH_ANCHOR: &str = "A-77-DELTA-SHIELD-LOCKED";

/// Maximum static payload bound in bytes for UEDO v1.2 serialization
pub const S_MAX_PAYLOAD_BYTES: usize = 4096;

/// Temporal execution ceiling in milliseconds (POSIX PREEMPT_RT floor)
pub const TAU_OVERRIDE_CEILING_MS: f64 = 11.99;

/// Minimum Guardian Alignment Index required for admissible actuation
pub const GUARDIAN_INDEX_FLOOR: f32 = 1.0;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum PosixDominanceState {
    Pass = 0,
    Pwc = 10,
    Freeze = 12,
    SecurityBreachFail = 40,
}

pub trait AdmissibilityGate {
    type Error;

    fn validate_payload_bounds(&self, size_in_bytes: usize) -> Result<(), Self::Error>;
    fn evaluate_guardian_index(&self, index: f32) -> PosixDominanceState;
    fn enforce_temporal_ceiling(&self, delta_ms: f64) -> PosixDominanceState;
}

pub trait CleanErasure {
    fn scrub_fram_memory(&mut self) -> Result<(), ()>;
    fn trigger_passive_thermal_fusing(&mut self) -> Result<(), ()>;
}