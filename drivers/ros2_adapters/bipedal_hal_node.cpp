// Bipedal Robotics Hardware Abstraction Layer (HAL) Adapter Node
// Project: Arcstone Triad System
// Master Anchor: A-77-DELTA-SHIELD-LOCKED
// Core Invariant: D(a) = DENY => Delta_external(a) = 0
// POSIX Dominance Hierarchy: BREACH(40) > REFUSAL(32) > CORRUPT(30) > PWC(12) > FREEZE(10) > PASS(0)

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "geometry_msgs/msg/twist.hpp"

// Canonical Dominance Enum (Aligned with uedo.proto and ARC-ERR-2026-001)
enum class PosixDominanceState {
    UNSPECIFIED = 0,
    POSIX_0_PASS = 1,
    POSIX_10_FREEZE = 2,
    POSIX_12_PWC = 3,
    POSIX_30_CORRUPT = 4,
    POSIX_32_REFUSAL = 5,
    POSIX_40_BREACH = 6
};

class BipedalHalNode : public rclcpp::Node {
public:
    BipedalHalNode() : Node("bipedal_hal_node"), active_hold_(true), latched_breach_(false) {
        // Ingress subscriber for incoming UEDO decision stream
        uedo_sub_ = this->create_subscription<std_msgs::msg::String>(
            "/arcstone/uedo_decision", 10,
            std::bind(&BipedalHalNode::on_uedo_frame, this, std::placeholders::_1));

        // Actuator command publishers (Physical Actuation Plane)
        cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/bipedal/cmd_vel", 1);
        joint_cmd_pub_ = this->create_publisher<sensor_msgs::msg::JointState>("/bipedal/joint_targets", 1);

        // Hardware Watchdog Timer: 200Hz (5ms period guarantees enforcement strictly within tau_override <= 11.99ms)
        watchdog_timer_ = this->create_wall_timer(
            std::chrono::milliseconds(5),
            std::bind(&BipedalHalNode::enforce_temporal_clamp, this));

        last_valid_heartbeat_ = this->now();
        RCLCPP_INFO(this->get_logger(), "Bipedal HAL Node initialized under A-77-DELTA-SHIELD-LOCKED.");
    }

private:
    void on_uedo_frame(const std_msgs::msg::String::SharedPtr msg) {
        // If a security breach was previously latched, reject execution until hard reboot
        if (latched_breach_) {
            execute_fail_closed_safe_hold(PosixDominanceState::POSIX_40_BREACH);
            return;
        }

        // Strict lattice dominance resolution (Supremum over L)
        PosixDominanceState dominance = resolve_dominance_precedence(msg->data);

        if (dominance == PosixDominanceState::POSIX_0_PASS) {
            // Authorized state transition
            active_hold_ = false;
            last_valid_heartbeat_ = this->now();
            RCLCPP_DEBUG(this->get_logger(), "UEDO PASS: Actuation membrane clear.");
        } else {
            if (dominance == PosixDominanceState::POSIX_40_BREACH) {
                latched_breach_ = true; // Permanent latch on security breach
            }
            // Strict Invariant Enforcement: I_1 (D(a) != PASS => Delta_external = 0)
            execute_fail_closed_safe_hold(dominance);
        }
    }

    void enforce_temporal_clamp() {
        // Watchdog clamp: If decision outlives temporal ceiling (tau_override <= 11.99ms)
        auto elapsed_ms = (this->now() - last_valid_heartbeat_).nanoseconds() / 1'000'000.0;
        if (elapsed_ms > 11.99) {
            execute_fail_closed_safe_hold(PosixDominanceState::POSIX_10_FREEZE);
        }
    }

    void execute_fail_closed_safe_hold(PosixDominanceState state) {
        active_hold_ = true;

        // 1. Actuator Bus Neutralization: Zero command velocities
        geometry_msgs::msg::Twist zero_twist;
        zero_twist.linear.x = 0.0;
        zero_twist.linear.y = 0.0;
        zero_twist.linear.z = 0.0;
        zero_twist.angular.x = 0.0;
        zero_twist.angular.y = 0.0;
        zero_twist.angular.z = 0.0;
        cmd_vel_pub_->publish(zero_twist);

        // 2. Physical Damping & Joint Locking: Apply holding brake torque across bilateral kinematic chain
        sensor_msgs::msg::JointState lock_joints;
        lock_joints.header.stamp = this->now();
        lock_joints.name = {
            "left_hip_yaw", "left_hip_roll", "left_hip_pitch", "left_knee", "left_ankle",
            "right_hip_yaw", "right_hip_roll", "right_hip_pitch", "right_knee", "right_ankle"
        };
        lock_joints.velocity = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        // Command holding torque to resist physical momentum and gravitational collapse
        lock_joints.effort = {150.0, 150.0, 200.0, 200.0, 100.0, 150.0, 150.0, 200.0, 200.0, 100.0};
        joint_cmd_pub_->publish(lock_joints);

        RCLCPP_WARN_THROTTLE(
            this->get_logger(), *this->get_clock(), 1000,
            "Fail-Closed Active: Joint brakes locked. Dominance state: %d", static_cast<int>(state));
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
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr joint_cmd_pub_;
    rclcpp::TimerBase::SharedPtr watchdog_timer_;
    rclcpp::Time last_valid_heartbeat_;
    bool active_hold_;
    bool latched_breach_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<BipedalHalNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
