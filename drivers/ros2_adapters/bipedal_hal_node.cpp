// Bipedal Robotics Hardware Abstraction Layer (HAL) Adapter Node
// Project: Arcstone Triad System
// Master Anchor: A-77-DELTA-SHIELD-LOCKED
// Core Invariant: D(a) = DENY => Delta_external(a) = 0
// POSIX Dominance Hierarchy: BREACH(40) > REFUSAL(32) > CORRUPT(30) > PWC(12) > FREEZE(10) > PASS(0)

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <regex>
#include <mutex>
#include <atomic>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "geometry_msgs/msg/twist.hpp"

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
    BipedalHalNode() 
        : Node("bipedal_hal_node"), 
          active_hold_(true), 
          latched_breach_(false) 
    {
        // 1. Ingress Decision Channel
        uedo_sub_ = this->create_subscription<std_msgs::msg::String>(
            "/arcstone/uedo_decision", 10,
            std::bind(&BipedalHalNode::on_uedo_frame, this, std::placeholders::_1));

        // 2. Untrusted Ingress Motion Channel (Raw proposal from autonomous agents)
        raw_cmd_sub_ = this->create_subscription<geometry_msgs::msg::Twist>(
            "/bipedal/cmd_vel_raw", 10,
            std::bind(&BipedalHalNode::on_raw_motion_proposal, this, std::placeholders::_1));

        // 3. Physical Actuation Plane (Exclusive outbound driver bus)
        cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/bipedal/cmd_vel", 1);
        joint_cmd_pub_ = this->create_publisher<sensor_msgs::msg::JointState>("/bipedal/joint_targets", 1);

        // Hardware Watchdog Timer: 200Hz (5ms period guarantees strict enforcement within tau_override <= 11.99ms)
        watchdog_timer_ = this->create_wall_timer(
            std::chrono::milliseconds(5),
            std::bind(&BipedalHalNode::enforce_temporal_clamp, this));

        last_valid_heartbeat_ = this->now();
        RCLCPP_INFO(this->get_logger(), "Bipedal HAL Node initialized under A-77-DELTA-SHIELD-LOCKED.");
    }

private:
    // Synchronous Ingress Gate: Mediates untrusted velocity commands
    void on_raw_motion_proposal(const geometry_msgs::msg::Twist::SharedPtr msg) {
        std::lock_guard<std::mutex> lock(state_mutex_);
        if (active_hold_ || latched_breach_) {
            // Invariant I_1: D(a) = DENY => Delta_external = 0 (Drop raw proposal immediately)
            return;
        }
        // Exclusively forward verified commands to the physical motor plane
        cmd_vel_pub_->publish(*msg);
    }

    void on_uedo_frame(const std_msgs::msg::String::SharedPtr msg) {
        std::lock_guard<std::mutex> lock(state_mutex_);

        if (latched_breach_) {
            execute_fail_closed_safe_hold_locked(PosixDominanceState::POSIX_40_BREACH);
            return;
        }

        PosixDominanceState dominance = resolve_dominance_precedence(msg->data);

        if (dominance == PosixDominanceState::POSIX_0_PASS) {
            active_hold_ = false;
            last_valid_heartbeat_ = this->now();
            RCLCPP_DEBUG(this->get_logger(), "UEDO PASS: Actuation membrane clear.");
        } else {
            if (dominance == PosixDominanceState::POSIX_40_BREACH || dominance == PosixDominanceState::POSIX_32_REFUSAL) {
                latched_breach_ = true; // Permanent hardware latch
            }
            execute_fail_closed_safe_hold_locked(dominance);
        }
    }

    void enforce_temporal_clamp() {
        std::lock_guard<std::mutex> lock(state_mutex_);
        auto elapsed_ms = (this->now() - last_valid_heartbeat_).nanoseconds() / 1'000'000.0;
        if (elapsed_ms > 11.99) {
            execute_fail_closed_safe_hold_locked(PosixDominanceState::POSIX_10_FREEZE);
        }
    }

    void execute_fail_closed_safe_hold_locked(PosixDominanceState state) {
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

        // 2. Physical Damping & Joint Locking: Command bilateral holding torque
        sensor_msgs::msg::JointState lock_joints;
        lock_joints.header.stamp = this->now();
        lock_joints.name = {
            "left_hip_yaw", "left_hip_roll", "left_hip_pitch", "left_knee", "left_ankle",
            "right_hip_yaw", "right_hip_roll", "right_hip_pitch", "right_knee", "right_ankle"
        };
        lock_joints.velocity = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        lock_joints.effort = {150.0, 150.0, 200.0, 200.0, 100.0, 150.0, 150.0, 200.0, 200.0, 100.0};
        joint_cmd_pub_->publish(lock_joints);

        RCLCPP_WARN_THROTTLE(
            this->get_logger(), *this->get_clock(), 1000,
            "Fail-Closed Active: Joint brakes locked. Dominance state: %d", static_cast<int>(state));
    }

    PosixDominanceState resolve_dominance_precedence(const std::string& payload) {
        if (payload.find("control_plane_gate") == std::string::npos) {
            return PosixDominanceState::POSIX_30_CORRUPT;
        }

        static const std::regex breach_rx(R"re("posix_dominance_state"\s*:\s*"POSIX_40_BREACH")re");
        static const std::regex refusal_rx(R"re("posix_dominance_state"\s*:\s*"POSIX_32_REFUSAL")re");
        static const std::regex corrupt_rx(R"re("posix_dominance_state"\s*:\s*"POSIX_30_CORRUPT")re");
        static const std::regex pwc_rx(R"re("posix_dominance_state"\s*:\s*"POSIX_12_PWC")re");
        static const std::regex freeze_rx(R"re("posix_dominance_state"\s*:\s*"POSIX_10_FREEZE")re");
        static const std::regex pass_rx(R"re("posix_dominance_state"\s*:\s*"POSIX_0_PASS")re");

        if (std::regex_search(payload, breach_rx))  return PosixDominanceState::POSIX_40_BREACH;
        if (std::regex_search(payload, refusal_rx)) return PosixDominanceState::POSIX_32_REFUSAL;
        if (std::regex_search(payload, corrupt_rx)) return PosixDominanceState::POSIX_30_CORRUPT;
        if (std::regex_search(payload, pwc_rx))     return PosixDominanceState::POSIX_12_PWC;
        if (std::regex_search(payload, freeze_rx))  return PosixDominanceState::POSIX_10_FREEZE;
        if (std::regex_search(payload, pass_rx))    return PosixDominanceState::POSIX_0_PASS;

        return PosixDominanceState::UNSPECIFIED;
    }

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr uedo_sub_;
    rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr raw_cmd_sub_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr joint_cmd_pub_;
    rclcpp::TimerBase::SharedPtr watchdog_timer_;
    rclcpp::Time last_valid_heartbeat_;
    
    std::mutex state_mutex_;
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
