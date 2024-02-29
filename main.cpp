#include "signal.hpp"
#include "event_loop.hpp"
#include <iostream>
#include <string>
#include <thread>

class Test {
public:
    OHTOAI_SIGNAL(on_1sec_timeout);
};

int main() {
    event_loop loop;
    Test test;
    test.on_1sec_timeout.connect([&loop, &test] {
        // print thread id and current time
        std::cout << "thread id: " << std::this_thread::get_id() << ", time: " << time(nullptr) << std::endl;
        // post event to event loop
        loop.add_event([&] {
            std::cout << "thread id: " << std::this_thread::get_id() << ", event loop event" << std::endl;
            });
        });

    loop.start();
    for (int i = 0; i < 10; i++) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        test.on_1sec_timeout.emit();
    }

    return 0;
}
