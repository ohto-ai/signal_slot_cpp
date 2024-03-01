#pragma once
#include <future>
#include <functional>

// event_loop class
class event_loop {
public:
    using event_handler_t = std::function<void()>;
    void post_event(event_handler_t handler) {
        std::lock_guard<std::mutex> lock(mtx_);
        handlers_.push_back(handler);
    }
    void run() {
        if (is_running_)
            return;
        tid_ = std::this_thread::get_id();
        is_running_ = true;
        while (is_running_) {
            if (!handlers_.empty()) {
                std::lock_guard<std::mutex> lock(mtx_);
                // execute and remove all handlers
                auto iter = handlers_.begin();
                while (iter != handlers_.end()) {
                    (*iter)();
                    iter = handlers_.erase(iter);
                }
            }
            std::this_thread::yield();
        }
        tid_ = std::thread::id();
    }

    void start() {
        if (is_running_) {
            return;
        }
        fut_ = std::async(std::launch::async, &event_loop::run, this);
    }

    void stop() {
        is_running_ = false;
    }

    void wait_running() {
        while (!is_running_) {
            std::this_thread::yield();
        }
    }

    bool is_running() {
        return is_running_;
    }

    void wait() {
        if (fut_.valid()) {
            fut_.wait();
        }
    }

    auto tid() {
        return tid_;
    }

    ~event_loop() {
        stop();
        wait();
    }
private:
    std::vector<event_handler_t> handlers_;
    std::atomic_bool is_running_;
    std::mutex mtx_;
    std::future<void> fut_;
    std::thread::id tid_;
};
