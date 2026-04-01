#include "tasks_tools/Tasks.hpp"
using namespace std::chrono_literals;

Tasks::Tasks(const rclcpp::Node::SharedPtr& node)
: node_(node)
{
    find_object_client_ = node_->create_client<vision_interfaces::srv::FindObject>("/tasks/find_object");
}

// ---------------------------------------------------------
// Find
// ---------------------------------------------------------
vision_interfaces::srv::FindObject::Response Tasks::Find(const std::string& category,
                                                          const std::string& item)
{
    vision_interfaces::srv::FindObject::Response empty;

    if (!find_object_client_->wait_for_service(1000ms))
    {
        RCLCPP_ERROR(node_->get_logger(), "FindObject service unavailable (/tasks/find_object)");
        return empty;
    }

    auto request = std::make_shared<vision_interfaces::srv::FindObject::Request>();
    request->class_name = item;

    std::mutex mtx;
    std::condition_variable cv;
    bool done = false;
    vision_interfaces::srv::FindObject::Response response;

    auto callback = [&](rclcpp::Client<vision_interfaces::srv::FindObject>::SharedFuture future)
    {
        response = *future.get();
        done = true;
        cv.notify_one();
    };

    find_object_client_->async_send_request(request, callback);

    std::unique_lock<std::mutex> lock(mtx);
    if (!cv.wait_for(lock, 10s, [&]{ return done; }))
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
