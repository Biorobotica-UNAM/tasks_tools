#pragma once
#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "rclcpp/rclcpp.hpp"
#include "vision_interfaces/srv/get_bbox.hpp"
#include "vision_interfaces/srv/find_object.hpp"
#include "faces_interfaces/srv/face_recog.hpp"
#include "human_interfaces/srv/detect_pointing.hpp" 
#include "std_srvs/srv/trigger.hpp"

class Tasks
{
public:
    explicit Tasks(const rclcpp::Node::SharedPtr& node);

    vision_interfaces::srv::FindObject::Response FindObject(const std::string& class_name);
    faces_interfaces::srv::FaceRecog::Response   FindPerson(const std::string& name);
    std_srvs::srv::Trigger::Response             FindSeat();
    std_srvs::srv::Trigger::Response             FindWavingHuman();

private:
    rclcpp::Node::SharedPtr node_;

    rclcpp::Client<vision_interfaces::srv::FindObject>::SharedPtr find_object_client_;    rclcpp::Client<faces_interfaces::srv::FaceRecog>::SharedPtr   find_person_client_;
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr             find_seat_client_;
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr             find_waiving_client_;
};
