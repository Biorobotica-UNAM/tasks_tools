#pragma once
#include <string>
#include <memory>
#include <mutex>
#include <condition_variable>
#include <future> // Necesario para la sincronía con promesas
#include "rclcpp/rclcpp.hpp"
#include "vision_interfaces/srv/find_object.hpp"
#include "faces_interfaces/srv/face_recog.hpp"
#include "std_srvs/srv/trigger.hpp"

class Tasks
{
public:
    explicit Tasks(const rclcpp::Node::SharedPtr& node);

    // Métodos de búsqueda que ahora usan promesas para ser "thread-safe"
    vision_interfaces::srv::FindObject::Response FindObject(const std::string& class_name);
    faces_interfaces::srv::FaceRecog::Response   FindPerson(const std::string& name);
    std_srvs::srv::Trigger::Response             FindSeat();
    std_srvs::srv::Trigger::Response             FindWavingHuman();

private:
    rclcpp::Node::SharedPtr node_;

    // Grupo de rellamada reentrante para permitir ejecución paralela[cite: 8]
    rclcpp::CallbackGroup::SharedPtr cb_group_;

    // Clientes de servicio
    rclcpp::Client<vision_interfaces::srv::FindObject>::SharedPtr find_object_client_;
    rclcpp::Client<faces_interfaces::srv::FaceRecog>::SharedPtr   find_person_client_;
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr             find_seat_client_;
    rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr             find_waiving_client_;
};