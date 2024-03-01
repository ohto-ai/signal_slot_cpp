#include "signal.hpp"
#include "event_loop.hpp"
#include "type.hpp"
#include <iostream>
#include <string>
#include <thread>
#include <iostream>
#include <functional>
#include <chrono>
#include <iomanip>
#include <sstream>

static std::mutex cout_mtx;
template <typename ...Args>
void log_cout_atmoic(Args&&...args) {
    std::scoped_lock lock(cout_mtx);

    auto date_time_str = [] {
        // 获取当前时间点
        auto now = std::chrono::system_clock::now();
        auto now_time = std::chrono::system_clock::to_time_t(now);

        // 获取秒部分的时间
        auto seconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
        auto fraction = now - seconds;

        // 将秒数转换为结构化时间
        std::tm timeinfo;
        gmtime_s(&timeinfo, &now_time);

        // 将时间格式化为字符串
        std::ostringstream ss;
        ss << std::put_time(&timeinfo, "%y/%m/%d %H:%M:%S") << '.' << std::setfill('0') << std::setw(3) << std::chrono::duration_cast<std::chrono::milliseconds>(fraction).count();

        return ss.str();
        } ();
    std::cout << date_time_str << " "
        << "[" << std::this_thread::get_id() << "] ";
    (std::cout << ... << std::forward<Args>(args)) << std::endl;
}

class Test {
public:
    OHTOAI_SIGNAL(on_1sec_timeout);
    void mem_slot_func() {
        log_cout_atmoic(__func__);
    }
    static void static_slot_func() {
        log_cout_atmoic(__func__);
    }
};

void global_slot_func() {
    log_cout_atmoic(__func__);
}


int main() {
    event_loop loop;
    Test test;
    test.on_1sec_timeout.connect([&loop, &test] {
        // print thread id and current time
        log_cout_atmoic(__func__, " time: ", time(nullptr));
        // post event to event loop
        loop.post_event([&] {
            if (loop.is_running())
                log_cout_atmoic("lambda with capture ", __func__);
            });
        });
    test.on_1sec_timeout.connect(&test, &Test::mem_slot_func);
    test.on_1sec_timeout.connect(Test::static_slot_func);
    test.on_1sec_timeout.connect(global_slot_func);

    loop.start();
    loop.wait_running();
    log_cout_atmoic("event loop thread id: ", loop.tid());
    for (int i = 0; i < 10; i++) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        test.on_1sec_timeout.emit();
    }

    return 0;
}
