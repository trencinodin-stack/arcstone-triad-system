#![no_std]

use crate::PosixDominanceState;

/// Maximum somatic life allocation ceiling
pub const L_MAX_CEILING: u8 = 10;

#[derive(Debug, Clone, Copy)]
pub struct SomaticLifeCounter {
    pub l_op: u8,
    pub token_b_revoked: bool,
}

impl SomaticLifeCounter {
    pub const fn new(initial_lives: u8) -> Self {
        let lives = if initial_lives > L_MAX_CEILING {
            L_MAX_CEILING
        } else {
            initial_lives
        };

        Self {
            l_op: lives,
            token_b_revoked: false,
        }
    }

    /// Deducts penalty lives based on substrate destruction.
    /// Triggers POSIX 40 permanent ban if L_op drops to 0.
    pub fn deduct_life(&mut self, delta_loss: u8) -> PosixDominanceState {
        if self.l_op <= delta_loss {
            self.l_op = 0;
            self.token_b_revoked = true;
            PosixDominanceState::SecurityBreachFail
        } else {
            self.l_op -= delta_loss;
            PosixDominanceState::Pass
        }
    }
}
