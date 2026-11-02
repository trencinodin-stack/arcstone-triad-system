// Abyssal Submersible Hardware Abstraction Layer (HAL) Adapter Node
// Project: Arcstone Triad System
// Master Anchor: A-77-DELTA-SHIELD-LOCKED
// Core Invariant: D(a) = DENY => Delta_external(a) = 0

#include <chrono>
#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include "std_msgs/msg/float64_multi_array.hpp"

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
    SubmersibleHalNode() : Node("submersible_hal_node"), thrusters_locked_(true) {
        uedo_sub_ = this->create_subscription<std_msgs::msg::String>(
            "/arcstone/uedo_submersible_decision", 10,
            std::bind(&SubmersibleHalNode::on_uedo_frame, this, std::placeholders::_1));

        thruster_cmd_pub_ = this->create_publisher<std_msgs::msg::Float64MultiArray>(
            "/submersible/thruster_thrust_setpoints", 1);

        // Hardware Watchdog Loop: 10ms (<= 11.99ms tau_override)
        watchdog_timer_ = this->create_wall_timer(
            std::chrono::milliseconds(10),
            std::bind(&SubmersibleHalNode::check_acoustic_timeout, this));

        last_valid_heartbeat_ = this->now();
        RCLCPP_INFO(this->get_logger(), "Submersible HAL Node initialized under A-77-DELTA-SHIELD-LOCKED.");
    }

private:
    void on_uedo_frame(const std_msgs::msg::String::SharedPtr msg) {
        PosixDominanceState dominance = parse_dominance_state(msg->data);

        if (dominance == PosixDominanceState::POSIX_0_PASS) {
            thrusters_locked_ = false;
            last_valid_heartbeat_ = this->now();
        } else {
            neutral_buoyancy_safe_hold(dominance);
        }
    }

    void check_acoustic_timeout() {
        auto elapsed_ms = (this->now() - last_valid_heartbeat_).nanoseconds() / 1'000'000.0;
        if (elapsed_ms > 11.99) {
            neutral_buoyancy_safe_hold(PosixDominanceState::POSIX_10_FREEZE);
        }
    }

    void neutral_buoyancy_safe_hold(PosixDominanceState state) {
        thrusters_locked_ = true;

        // Cut thruster RPM / PWM signals to zero across all 8 vectors
        std_msgs::msg::Float64MultiArray zero_thrust;
        zero_thrust.data = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        thruster_cmd_pub_->publish(zero_thrust);

        RCLCPP_WARN_THROTTLE(
            this->get_logger(), *this->get_clock(), 1000,
            "Fail-Closed Active: Thrusters zeroed. Marine safe-hold locked. Dominance: %d", static_cast<int>(state));
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
    rclcpp::Publisher<std_msgs::msg::Float64MultiArray>::SharedPtr thruster_cmd_pub_;
    rclcpp::TimerBase::SharedPtr watchdog_timer_;
    rclcpp::Time last_valid_heartbeat_;
    bool thrusters_locked_;
};

int main(int argc, char** argv) {
    rclcpp::init(argc, argv);
    auto node = std::make_shared<SubmersibleHalNode>();
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}
