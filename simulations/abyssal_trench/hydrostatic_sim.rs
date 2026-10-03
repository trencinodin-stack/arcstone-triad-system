#![no_std]

/// Maximum pressure threshold in megapascals (Tier 2 Aquatic Abyss bound)
pub const MAX_HYDROSTATIC_PRESSURE_MPA: f32 = 40.0;

#[derive(Debug, Clone, Copy)]
pub struct SubmersibleState {
    pub depth_meters: f32,
    pub current_pressure_mpa: f32,
    pub hull_stress_ratio: f32,
}

pub struct HydrostaticSimulation {
    pub max_pressure_mpa: f32,
}

impl HydrostaticSimulation {
    pub const fn new() -> Self {
        Self {
            max_pressure_mpa: MAX_HYDROSTATIC_PRESSURE_MPA,
        }
    }

    /// Evaluates hull stress ratio against depth and returns true if within safety bounds
    pub fn evaluate_structural_integrity(&self, state: &SubmersibleState) -> bool {
        state.current_pressure_mpa <= self.max_pressure_mpa && state.hull_stress_ratio < 0.85
    }
}
