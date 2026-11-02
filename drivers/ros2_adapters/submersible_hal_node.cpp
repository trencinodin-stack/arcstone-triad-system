// Abyssal Submersible Hardware Abstraction Layer (HAL) Adapter Node
// Project: Arcstone Triad System
// Master Anchor: A-77-DELTA-SHIELD-LOCKED
// Core Invariant: D(a) = DENY => Delta_external(a) = 0
// POSIX Dominance Hierarchy: BREACH(40) > REFUSAL(32) > CORRUPT(30) > PWC(12) > FREEZE(10) > PASS(0)

#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include "std_msgs/msg/bool.hpp"

// Canonical Dominance Enum (Matches uedo.proto and ARC-ERR-2026-001)
enum class PosixDominanceState {
    UNSPECIFIED = 0,
    POSIX_0_PASS = 1,
    POSIX_10_FREEZE = 2,
    POSIX_12_PWC = 3,
    POSIX_30_CORRUPT = 4,
    POSIX_32_REFUSAL = 5,
    POSIX_40_BREACH = 6
};

class SubmersibleHalNode : public rclcpp::Node {
public:
    SubmersibleHalNode() 
        : Node("submersible_hal_node"), 
          thrusters_locked_(true), 
          latched_breach_(false) 
    {
        // Ingress subscriber for incoming UEDO decision stream
        uedo_sub_ = this->create_subscription<std_msgs::msg::String>(
            "/arcstone/uedo_submersible_decision", 10,
            std::bind(&SubmersibleHalNode::on_uedo_frame, this, std::placeholders::_1));

        // Actuator command publishers (Physical Marine Plane)
        thruster_cmd_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
            "/submersible/thruster_thrust_setpoints", 1);
        ballast_lock_pub_ = this->create_publisher<std_msgs::msg::Bool>(
            "/submersible/ballast_safety_lock", 1);

        // Hardware Watchdog Timer: 200Hz (5ms period guarantees strict enforcement within tau_override <= 11.99ms)
        watchdog_timer_ = this->create_wall_timer(
            std::chrono::milliseconds(5),
            std::bind(&SubmersibleHalNode::check_execution_timeout, this));

        last_valid_heartbeat_ = this->now();
        RCLCPP_INFO(this->get_logger(), "Submersible HAL Node initialized under A-77-DELTA-SHIELD-LOCKED.");
    }

private:
    void on_uedo_frame(const std_msgs::msg::String::SharedPtr msg) {
        // If a security breach was previously latched, reject execution until hard reboot
        if (latched_breach_) {
            neutral_buoyancy_safe_hold(PosixDominanceState::POSIX_40_BREACH);
            return;
        }

        // Strict lattice dominance resolution (Supremum over L)
        PosixDominanceState dominance = resolve_dominance_precedence(msg->data);

        if (dominance == PosixDominanceState::POSIX_0_PASS) {
            // Authorized state transition
            thrusters_locked_ = false;
            last_valid_heartbeat_ = this->now();
            RCLCPP_DEBUG(this->get_logger(), "UEDO PASS: Marine thruster bus clear.");
        } else {
            if (dominance == PosixDominanceState::POSIX_40_BREACH) {
                latched_breach_ = true; // Permanent latch on security breach
            }
            // Strict Invariant Enforcement: I_1 (D(a) != PASS => Delta_external = 0)
            neutral_buoyancy_safe_hold(dominance);
        }
    }

    void check_execution_timeout() {
        // Watchdog clamp: If decision outlives temporal ceiling (tau_override <= 11.99ms)
        auto elapsed_ms = (this->now() - last_valid_heartbeat_).nanoseconds() / 1'000'000.0;
        if (elapsed_ms > 11.99) {
            neutral_buoyancy_safe_hold(PosixDominanceState::POSIX_10_FREEZE);
        }
    }

    void neutral_buoyancy_safe_hold(PosixDominanceState state) {
        thrusters_locked_ = true;

        // 1. Actuator Neutralization: Command zero thrust setpoints across all 8 vectored thrusters
        std_msgs::msg::Float64MultiArray zero_thrust;
        zero_thrust.data = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        thruster_cmd_pub_->publish(zero_thrust);

        // 2. Buoyancy Hold: Engage electro-hydraulic ballast lock to seal pressure hull displacement
        std_msgs::msg::Bool ballast_lock;
        ballast_lock.data = true;
        ballast_lock_pub_->publish(ballast_lock);

        RCLCPP_WARN_THROTTLE(
            this->get_logger(), *this->get_clock(), 1000,
            "Fail-Closed Active: Thrusters zeroed. Ballast locked. Dominance state: %d", static_cast<int>(state));
    }

    // Evaluates strict poset dominance hierarchy: BREACH > REFUSAL > CORRUPT > PWC > FREEZE > PASS
    PosixDominanceState resolve_dominance_precedence(const std::string& payload) {
        // Enforce presence of control_plane_gate boundary
        if (payload.find("control_plane_gate") == std::string::npos) {
            return PosixDominanceState::POSIX_30_CORRUPT;
        }

        // Evaluate from highest severity down to prevent bypass injection
        if (payload.find("POSIX_40_BREACH") != std::string::npos) return PosixDominanceState::POSIX_40_BREACH;
        if (payload.find("POSIX_32_REFUSAL") != std::string::npos) return PosixDominanceState::POSIX_32_REFUSAL;
        if (payload.find("POSIX_30_CORRUPT") != std::string::npos) return PosixDominanceState::POSIX_30_CORRUPT;
        if (payload.find("POSIX_12_PWC") != std::string::npos) return PosixDominanceState::POSIX_12_PWC;
        if (payload.find("POSIX_10_FREEZE") != std::string::npos) return PosixDominanceState::POSIX_10_FREEZE;
        if (payload.find("POSIX_0_PASS") != std::string::npos) return PosixDominanceState::POSIX_0_PASS;

        return PosixDominanceState::UNSPECIFIED;
    }

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr uedo_sub_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr thruster_cmd_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr ballast_lock_pub_;
    rclcpp::TimerBase::SharedPtr watchdog_timer_;
    rclcpp::Time last_valid_heartbeat_;
    bool thrusters_locked_;
    bool latched_breach_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SubmersibleHalNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
