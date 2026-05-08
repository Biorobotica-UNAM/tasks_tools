#include "tasks_tools/Tasks.hpp"
#include <algorithm>
#include <future> 

using namespace std::chrono_literals;

Tasks::Tasks(const rclcpp::Node::SharedPtr& node)
: node_(node)
{
    // IMPORTANTE: cb_group_ debe ser un miembro de la clase definido en el .hpp
    // Esto garantiza que el grupo viva durante toda la vida del objeto Tasks
    cb_group_ = node_->create_callback_group(rclcpp::CallbackGroupType::Reentrant);

    // Registramos los clientes vinculándolos al grupo reentrante para evitar deadlocks
    find_object_client_ = node_->create_client<vision_interfaces::srv::FindObject>(
        "/tasks/find_object", rmw_qos_profile_services_default, cb_group_);
    
    find_person_client_ = node_->create_client<faces_interfaces::srv::FaceRecog>(
        "/tasks/find_person", rmw_qos_profile_services_default, cb_group_);
    
    find_seat_client_ = node_->create_client<std_srvs::srv::Trigger>(
        "/tasks/find_seat", rmw_qos_profile_services_default, cb_group_);
    
    find_waiving_client_ = node_->create_client<std_srvs::srv::Trigger>(
        "/tasks/find_waving_human", rmw_qos_profile_services_default, cb_group_);
}

// ---------------------------------------------------------
// FindObject (Sincronización segura mediante promesas)
// ---------------------------------------------------------
vision_interfaces::srv::FindObject::Response Tasks::FindObject(const std::string& class_name) {
    vision_interfaces::srv::FindObject::Response response;
    response.success = false;

    if (!find_object_client_->wait_for_service(1s)) {
        RCLCPP_ERROR(node_->get_logger(), "FindObject service unavailable");
        return response;
    }

    auto request = std::make_shared<vision_interfaces::srv::FindObject::Request>();
    request->class_name = class_name;

    auto promise = std::make_shared<std::promise<vision_interfaces::srv::FindObject::Response>>();
    auto future = promise->get_future();

    find_object_client_->async_send_request(request,
        [promise](rclcpp::Client<vision_interfaces::srv::FindObject>::SharedFuture future_cb) {
            promise->set_value(*future_cb.get());
        });

    // Esperamos el resultado (40s es buen margen para el escaneo)
    if (future.wait_for(40s) == std::future_status::ready) {
        response = future.get();
        if (response.success) {
            // CORRECCIÓN: Acceso a la nueva estructura PoseStamped
            // response.pose -> PoseStamped
            // response.pose.pose -> Pose
            // response.pose.pose.position -> Point
            RCLCPP_INFO(node_->get_logger(), 
                "Tasks: Objeto [%s] recibido en base_link: [x:%.2f, y:%.2f, z:%.2f]", 
                response.detected_class.c_str(), 
                response.pose.pose.position.x, 
                response.pose.pose.position.y,
                response.pose.pose.position.z);
        }
    } else {
        RCLCPP_ERROR(node_->get_logger(), "FindObject: Tiempo de espera agotado (Timeout)");
    }

    return response;
}

// ---------------------------------------------------------
// FindPerson
// ---------------------------------------------------------
faces_interfaces::srv::FaceRecog::Response Tasks::FindPerson(const std::string& name)
{
    faces_interfaces::srv::FaceRecog::Response response;
    response.success = false;

    if (!find_person_client_->wait_for_service(1s)) {
        RCLCPP_ERROR(node_->get_logger(), "FindPerson service unavailable");
        return response;
    }

    auto request = std::make_shared<faces_interfaces::srv::FaceRecog::Request>();
    request->name = name;

    auto promise = std::make_shared<std::promise<faces_interfaces::srv::FaceRecog::Response>>();
    auto future = promise->get_future();

    find_person_client_->async_send_request(request,
        [promise](rclcpp::Client<faces_interfaces::srv::FaceRecog>::SharedFuture future_cb) {
            promise->set_value(*future_cb.get());
        });

    if (future.wait_for(15s) == std::future_status::ready) {
        response = future.get();
        if (response.success)
            RCLCPP_INFO(node_->get_logger(), "FindPerson OK: %s", response.name.c_str());
    }

    return response;
}

// ---------------------------------------------------------
// FindSeat
// ---------------------------------------------------------
std_srvs::srv::Trigger::Response Tasks::FindSeat()
{
    std_srvs::srv::Trigger::Response response;
    response.success = false;

    if (!find_seat_client_->wait_for_service(1s)) {
        RCLCPP_ERROR(node_->get_logger(), "FindSeat service unavailable");
        return response;
    }

    auto request = std::make_shared<std_srvs::srv::Trigger::Request>();
    auto promise = std::make_shared<std::promise<std_srvs::srv::Trigger::Response>>();
    auto future = promise->get_future();

    find_seat_client_->async_send_request(request,
        [promise](rclcpp::Client<std_srvs::srv::Trigger>::SharedFuture future_cb) {
            promise->set_value(*future_cb.get());
        });

    if (future.wait_for(15s) == std::future_status::ready) {
        response = future.get();
        if (response.success)
            RCLCPP_INFO(node_->get_logger(), "FindSeat OK: %s", response.message.c_str());
    }

    return response;
}

// ---------------------------------------------------------
// FindWaivingHuman
// ---------------------------------------------------------
std_srvs::srv::Trigger::Response Tasks::FindWavingHuman()
{
    std_srvs::srv::Trigger::Response response;
    response.success = false;

    if (!find_waiving_client_->wait_for_service(1s)) {
        RCLCPP_ERROR(node_->get_logger(), "FindWaivingHuman service unavailable");
        return response;
    }

    auto request = std::make_shared<std_srvs::srv::Trigger::Request>();
    auto promise = std::make_shared<std::promise<std_srvs::srv::Trigger::Response>>();
    auto future = promise->get_future();

    find_waiving_client_->async_send_request(request,
        [promise](rclcpp::Client<std_srvs::srv::Trigger>::SharedFuture future_cb) {
            promise->set_value(*future_cb.get());
        });

    if (future.wait_for(30s) == std::future_status::ready) {
        response = future.get();
        if (response.success)
            RCLCPP_INFO(node_->get_logger(), "FindWaivingHuman OK: %s", response.message.c_str());
    }

    return response;
}