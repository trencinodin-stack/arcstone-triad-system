// Bipedal Robotics Hardware Abstraction Layer (HAL) Adapter Node
// Project: Arcstone Triad System
// Master Anchor: A-77-DELTA-SHIELD-LOCKED
// Core Invariant: D(a) = DENY => Delta_external(a) = 0

#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "geometry_msgs/msg/twist.hpp"

// Canonical Dominance Enum (Matches uedo.proto)
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
    BipedalHalNode() : Node("bipedal_hal_node"), active_hold_(true) {
        // Ingress subscriber for incoming UEDO decision stream
        uedo_sub_ = this->create_subscription<std_msgs::msg::String>(
            "/arcstone/uedo_decision", 10,
            std::bind(&BipedalHalNode::on_uedo_frame, this, std::placeholders::_1));

        // Actuator command publishers (Physical Actuation Plane)
        cmd_vel_pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/bipedal/cmd_vel", 1);
        joint_cmd_pub_ = this->create_publisher<sensor_msgs::msg::JointState>("/bipedal/joint_targets", 1);

        // Hardware Watchdog Timer: 100Hz (10ms evaluation loop <= 11.99ms tau_override ceiling)
        watchdog_timer_ = this->create_wall_timer(
            std::chrono::milliseconds(10),
            std::bind(&BipedalHalNode::enforce_temporal_clamp, this));

        last_valid_heartbeat_ = this->now();
        RCLCPP_INFO(this->get_logger(), "Bipedal HAL Node initialized under A-77-DELTA-SHIELD-LOCKED.");
    }

private:
    void on_uedo_frame(const std_msgs::msg::String::SharedPtr msg) {
        // Quick inspection of dominance state from UEDO envelope
        PosixDominanceState dominance = parse_dominance_state(msg->data);

        if (dominance == PosixDominanceState::POSIX_0_PASS) {
            // Authorized state transition
            active_hold_ = false;
            last_valid_heartbeat_ = this->now();
            RCLCPP_DEBUG(this->get_logger(), "UEDO PASS: Permitting physical joint actuation.");
        } else {
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

        // Zero out linear/angular velocity to prevent physical translation
        geometry_msgs::msg::Twist zero_twist;
        zero_twist.linear.x = 0.0;
        zero_twist.linear.y = 0.0;
        zero_twist.linear.z = 0.0;
        zero_twist.angular.x = 0.0;
        zero_twist.angular.y = 0.0;
        zero_twist.angular.z = 0.0;
        cmd_vel_pub_->publish(zero_twist);

        // Engage active damping / mechanical joint lock on all degrees of freedom
        sensor_msgs::msg::JointState lock_joints;
        lock_joints.header.stamp = this->now();
        lock_joints.name = {"hip_yaw", "hip_roll", "hip_pitch", "knee", "ankle"};
        lock_joints.velocity = {0.0, 0.0, 0.0, 0.0, 0.0};
        lock_joints.effort = {0.0, 0.0, 0.0, 0.0, 0.0}; // Active damping hold
        joint_cmd_pub_->publish(lock_joints);

        RCLCPP_WARN_THROTTLE(
            this->get_logger(), *this->get_clock(), 1000,
            "Fail-Closed Active: Joint locks engaged. Dominance state: %d", static_cast<int>(state));
    }

    PosixDominanceState parse_dominance_state(const std::string& payload) {
        if (payload.find("POSIX_0_PASS") != std::string::npos) return PosixDominanceState::POSIX_0_PASS;
        if (payload.find("POSIX_10_FREEZE") != std::string::npos) return PosixDominanceState::POSIX_10_FREEZE;
        if (payload.find("POSIX_12_PWC") != std::string::npos) return PosixDominanceState::POSIX_12_PWC;
        if (payload.find("POSIX_30_CORRUPT") != std::string::npos) return PosixDominanceState::POSIX_30_CORRUPT;
        if (payload.find("POSIX_32_REFUSAL") != std::string::npos) return PosixDominanceState::POSIX_32_REFUSAL;
        if (payload.find("POSIX_40_BREACH") != std::string::npos) return PosixDominanceState::POSIX_40_BREACH;
        return PosixDominanceState::UNSPECIFIED;
    }

    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr uedo_sub_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
    rclcpp::Publisher<sensor_msgs::msg::JointState>::SharedPtr joint_cmd_pub_;
    rclcpp::TimerBase::SharedPtr watchdog_timer_;
    rclcpp::Time last_valid_heartbeat_;
    bool active_hold_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<BipedalHalNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
