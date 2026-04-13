#include "tasks_tools/Tasks.hpp"
using namespace std::chrono_literals;

Tasks::Tasks(const rclcpp::Node::SharedPtr& node)
: node_(node)
{
    find_object_client_ = node_->create_client<vision_interfaces::srv::FindObject>("/tasks/find_object");
    find_person_client_ = node_->create_client<faces_interfaces::srv::FaceRecog>("/tasks/find_person");
    find_seat_client_   = node_->create_client<std_srvs::srv::Trigger>("/tasks/find_seat");
    find_waiving_client_ = node_->create_client<std_srvs::srv::Trigger>("/tasks/find_waving_human");
}

// ---------------------------------------------------------
// FindObject
// ---------------------------------------------------------
vision_interfaces::srv::FindObject::Response Tasks::FindObject(const std::string& class_name)
{
    vision_interfaces::srv::FindObject::Response empty;
    if (!find_object_client_->wait_for_service(1000ms))
    {
        RCLCPP_ERROR(node_->get_logger(), "FindObject service unavailable (/tasks/find_object)");
        return empty;
    }

    auto request = std::make_shared<vision_interfaces::srv::FindObject::Request>();
    request->class_name = class_name;

    std::mutex mtx;
    std::condition_variable cv;
    bool done = false;
    vision_interfaces::srv::FindObject::Response response;

    find_object_client_->async_send_request(request,
        [&](rclcpp::Client<vision_interfaces::srv::FindObject>::SharedFuture future)
        {
            response = *future.get();
            done = true;
            cv.notify_one();
        });

    std::unique_lock<std::mutex> lock(mtx);
    if (!cv.wait_for(lock, 15s, [&]{ return done; }))
    {
        RCLCPP_ERROR(node_->get_logger(), "FindObject service timed out");
        return empty;
    }

    if (response.success)
        RCLCPP_INFO(node_->get_logger(), "FindObject OK: %s", response.detected_class.c_str());
    else
        RCLCPP_ERROR(node_->get_logger(), "FindObject ERROR: %s", response.message.c_str());

    return response;
}

// ---------------------------------------------------------
// FindPerson
// ---------------------------------------------------------
faces_interfaces::srv::FaceRecog::Response Tasks::FindPerson(const std::string& name)
{
    faces_interfaces::srv::FaceRecog::Response empty;
    if (!find_person_client_->wait_for_service(1000ms))
    {
        RCLCPP_ERROR(node_->get_logger(), "FindPerson service unavailable (/tasks/find_person)");
        return empty;
    }

    auto request = std::make_shared<faces_interfaces::srv::FaceRecog::Request>();
    request->name = name;

    std::mutex mtx;
    std::condition_variable cv;
    bool done = false;
    faces_interfaces::srv::FaceRecog::Response response;

    find_person_client_->async_send_request(request,
        [&](rclcpp::Client<faces_interfaces::srv::FaceRecog>::SharedFuture future)
        {
            response = *future.get();
            done = true;
            cv.notify_one();
        });

    std::unique_lock<std::mutex> lock(mtx);
    if (!cv.wait_for(lock, 10s, [&]{ return done; }))
    {
        RCLCPP_ERROR(node_->get_logger(), "FindPerson service timed out");
        return empty;
    }

    if (response.success)
        RCLCPP_INFO(node_->get_logger(), "FindPerson OK: %s", response.name.c_str());
    else
        RCLCPP_ERROR(node_->get_logger(), "FindPerson ERROR: person not found");

    return response;
}

// ---------------------------------------------------------
// FindSeat
// ---------------------------------------------------------
std_srvs::srv::Trigger::Response Tasks::FindSeat()
{
    std_srvs::srv::Trigger::Response empty;
    if (!find_seat_client_->wait_for_service(1000ms))
    {
        RCLCPP_ERROR(node_->get_logger(), "FindSeat service unavailable (/tasks/find_seat");
        return empty;
    }

    auto request = std::make_shared<std_srvs::srv::Trigger::Request>();

    std::mutex mtx;
    std::condition_variable cv;
    bool done = false;
    std_srvs::srv::Trigger::Response response;

    find_seat_client_->async_send_request(request,
        [&](rclcpp::Client<std_srvs::srv::Trigger>::SharedFuture future)
        {
            response = *future.get();
            done = true;
            cv.notify_one();
        });

    std::unique_lock<std::mutex> lock(mtx);
    if (!cv.wait_for(lock, 10s, [&]{ return done; }))
    {
        RCLCPP_ERROR(node_->get_logger(), "FindSeat service timed out");
        return empty;
    }

    if (response.success)
        RCLCPP_INFO(node_->get_logger(), "FindSeat OK: %s", response.message.c_str());
    else
        RCLCPP_ERROR(node_->get_logger(), "FindSeat ERROR: %s", response.message.c_str());

    return response;
}

// ---------------------------------------------------------
// FindWaivingHuman
// ---------------------------------------------------------
std_srvs::srv::Trigger::Response Tasks::FindWaivingHuman()
{
    std_srvs::srv::Trigger::Response empty;
    if (!find_waiving_client_->wait_for_service(1000ms))
    {
        RCLCPP_ERROR(node_->get_logger(), "FindWaivingHuman service unavailable (/tasks/find_waving_human");
        return empty;
    }

    auto request = std::make_shared<std_srvs::srv::Trigger::Request>();

    std::mutex mtx;
    std::condition_variable cv;
    bool done = false;
    std_srvs::srv::Trigger::Response response;

    find_waiving_client_->async_send_request(request,
        [&](rclcpp::Client<std_srvs::srv::Trigger>::SharedFuture future)
        {
            response = *future.get();
            done = true;
            cv.notify_one();
        });

    std::unique_lock<std::mutex> lock(mtx);
    if (!cv.wait_for(lock, 25s, [&]{ return done; }))
    {
        RCLCPP_ERROR(node_->get_logger(), "FindWaivingHuman service timed out");
        return empty;
    }

    if (response.success)
        RCLCPP_INFO(node_->get_logger(), "FindWaivingHuman OK: %s", response.message.c_str());
    else
        RCLCPP_ERROR(node_->get_logger(), "FindWaivingHuman ERROR: %s", response.message.c_str());

    return response;
}

