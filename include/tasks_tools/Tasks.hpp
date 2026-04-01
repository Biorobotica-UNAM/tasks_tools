#pragma once
#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "rclcpp/rclcpp.hpp"
#include "vision_interfaces/srv/find_object.hpp"

class Tasks
{
public:
    explicit Tasks(const rclcpp::Node::SharedPtr& node);

    vision_interfaces::srv::FindObject::Response Find(const std::string& category,
                                                       const std::string& item);
private:
    rclcpp::Node::SharedPtr node_;
    rclcpp::Client<vision_interfaces::srv::FindObject>::SharedPtr find_object_client_;
};
