#pragma once
#include <future>
#include <functional>

// event_loop class
class event_loop {
public:
    using event_handler_t = std::function<void()>;
    void add_event(event_handler_t handler) {
        std::lock_guard<std::mutex> lock(mtx);
        handlers.push_back(handler);
    }
    void run() {
        is_running = true;
        while (is_running) {
            if (!handlers.empty()) {
                std::lock_guard<std::mutex> lock(mtx);
                // execute and remove all handlers
                auto iter = handlers.begin();
                while (iter != handlers.end()) {
                    (*iter)();
                    iter = handlers.erase(iter);
                }
            }
            std::this_thread::yield();
        }
    }

    void start() {
        if (is_running) {
            return;
        }
        fut = std::async(std::launch::async, &event_loop::run, this);
    }

    void stop() {
        is_running = false;
    }

    void wait() {
        if (fut.valid()) {
            fut.wait();
        }
    }

    ~event_loop() {
        stop();
        wait();
    }
private:
    std::vector<event_handler_t> handlers;
    std::atomic_bool is_running;
    std::mutex mtx;
    std::future<void> fut;
};
