#pragma once

#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "rclcpp/rclcpp.hpp"
#include "std_srvs/srv/set_bool.hpp"

class Tasks
{
public:
    explicit Tasks(const rclcpp::Node::SharedPtr& node);

    std::string Find(const std::string& grammar);

private:
    rclcpp::Node::SharedPtr node_;

    rclcpp::Client<std_srvs::srv::SetBool>::SharedPtr human_follower_client_;
};
