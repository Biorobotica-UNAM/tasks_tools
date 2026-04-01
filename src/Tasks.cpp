#include "tasks_tools/Tasks.hpp"
using namespace std::chrono_literals;

Tasks::Tasks(const rclcpp::Node::SharedPtr& node)
: node_(node)
{
    human_follower_client_ = node_->create_client<std_srvs::srv::SetBool>("/hri/human_tracking/enable");
}

// ---------------------------------------------------------
// Find
// ---------------------------------------------------------
bool Tasks::Find(const std::string& text)
{
    if (!talker_client_->wait_for_service(1000ms)) {
        RCLCPP_ERROR(node_->get_logger(), "Talk service unavailable (/talk)");
        return false;
    }

    auto request = std::make_shared<talker_interfaces::srv::Talk::Request>();
    request->text = text;

    std::mutex mtx;
    std::condition_variable cv;
    bool done = false;
    bool success = false;
    std::string message;

    auto callback = [&](rclcpp::Client<talker_interfaces::srv::Talk>::SharedFuture future){
        auto response = future.get();
        success = response->success;
        message = response->message;
        done = true;
        cv.notify_one();
    };

    talker_client_->async_send_request(request, callback);

    std::unique_lock<std::mutex> lock(mtx);
    if (!cv.wait_for(lock, 10s, [&]{ return done; })) {
        RCLCPP_ERROR(node_->get_logger(), "Talk service timed out");
        return false;
    }

    if (success)
        RCLCPP_INFO(node_->get_logger(), "TTS OK: %s", message.c_str());
    else
        RCLCPP_ERROR(node_->get_logger(), "TTS ERROR: %s", message.c_str());

    return success;
}
