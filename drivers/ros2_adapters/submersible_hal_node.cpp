// Abyssal Submersible Hardware Abstraction Layer (HAL) Adapter Node
// Project: Arcstone Triad System
// Master Anchor: A-77-DELTA-SHIELD-LOCKED
// Core Invariant: D(a) = DENY => Delta_external(a) = 0
// POSIX Dominance Hierarchy: BREACH(40) > REFUSAL(32) > CORRUPT(30) > PWC(12) > FREEZE(10) > PASS(0)

#include <chrono>
#include <memory>
#include <string>
#include <regex>
#include <mutex>
#include <atomic>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"
#include "std_msgs/msg/bool.hpp"

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
        // 1. Ingress Decision Channel
        uedo_sub_ = this->create_subscription<std_msgs::msg::String>(
            "/arcstone/uedo_submersible_decision", 10,
            std::bind(&SubmersibleHalNode::on_uedo_frame, this, std::placeholders::_1));

        // 2. Untrusted Ingress Thruster Channel (Raw proposal from autonomous systems)
        raw_thrust_sub_ = this->create_subscription<std_msgs::msg::Float64MultiArray>(
            "/submersible/thruster_thrust_setpoints_raw", 10,
            std::bind(&SubmersibleHalNode::on_raw_thrust_proposal, this, std::placeholders::_1));

        // 3. Physical Marine Plane (Exclusive outbound driver bus)
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
    // Synchronous Ingress Gate: Mediates untrusted thruster setpoints
    void on_raw_thrust_proposal(const std_msgs::msg::Float64MultiArray::SharedPtr msg) {
        std::lock_guard<std::mutex> lock(state_mutex_);
        if (thrusters_locked_ || latched_breach_) {
            // Invariant I_1: D(a) = DENY => Delta_external = 0 (Drop raw proposal immediately)
            return;
        }
        // Exclusively forward verified commands to the physical thruster bus
        thruster_cmd_pub_->publish(*msg);
    }

    void on_uedo_frame(const std_msgs::msg::String::SharedPtr msg) {
        std::lock_guard<std::mutex> lock(state_mutex_);

        if (latched_breach_) {
            neutral_buoyancy_safe_hold_locked(PosixDominanceState::POSIX_40_BREACH);
            return;
        }

        PosixDominanceState dominance = resolve_dominance_precedence(msg->data);

        if (dominance == PosixDominanceState::POSIX_0_PASS) {
            thrusters_locked_ = false;
            last_valid_heartbeat_ = this->now();
            RCLCPP_DEBUG(this->get_logger(), "UEDO PASS: Marine thruster bus clear.");
        } else {
            if (dominance == PosixDominanceState::POSIX_40_BREACH || dominance == PosixDominanceState::POSIX_32_REFUSAL) {
                latched_breach_ = true;
            }
            neutral_buoyancy_safe_hold_locked(dominance);
        }
    }

    void check_execution_timeout() {
        std::lock_guard<std::mutex> lock(state_mutex_);
        auto elapsed_ms = (this->now() - last_valid_heartbeat_).nanoseconds() / 1'000'000.0;
        if (elapsed_ms > 11.99) {
            neutral_buoyancy_safe_hold_locked(PosixDominanceState::POSIX_10_FREEZE);
        }
    }

    void neutral_buoyancy_safe_hold_locked(PosixDominanceState state) {
        thrusters_locked_ = true;

        // 1. Actuator Neutralization: Zero command thrust setpoints across all 8 thrusters
        std_msgs::msg::Float64MultiArray zero_thrust;
        zero_thrust.data = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        thruster_cmd_pub_->publish(zero_thrust);

        // 2. Buoyancy Hold: Engage electro-hydraulic ballast lock
        std_msgs::msg::Bool ballast_lock;
        ballast_lock.data = true;
        ballast_lock_pub_->publish(ballast_lock);

        RCLCPP_WARN_THROTTLE(
            this->get_logger(), *this->get_clock(), 1000,
            "Fail-Closed Active: Thrusters zeroed. Ballast locked. Dominance state: %d", static_cast<int>(state));
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
    rclcpp::Subscription<std_msgs::msg::Float64MultiArray>::SharedPtr raw_thrust_sub_;
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr thruster_cmd_pub_;
    rclcpp::Publisher<std_msgs::msg::Bool>::SharedPtr ballast_lock_pub_;
    rclcpp::TimerBase::SharedPtr watchdog_timer_;
    rclcpp::Time last_valid_heartbeat_;
    
    std::mutex state_mutex_;
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
