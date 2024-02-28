#include "signal.hpp"
#include <iostream>
#include <string>

class Test {
public:
    OHTOAI_SIGNAL(test_sig, int, int);
    ohtoai::signal::signal<std::string, double> visible_sig;
};

int main() {
    Test a;
    a.test_sig.connect([](int a, int b) {
        std::cout << "test_sig: " << a << " " << b << std::endl;
    });

    a.test_sig.connect([](int a, int b) {
        std::cout << "one more sig: " << a << " " << b << std::endl;
        });

    auto cb = [](int a, int b) {
        std::cout << "will be removed: " << a << " " << b << std::endl;
    };

    auto id = a.test_sig.connect(cb);
    a.test_sig(1, 2);
    a.test_sig.disconnect(id);
    a.test_sig(3, 4);
    a.test_sig.block();
    a.test_sig(5, 6);
    a.test_sig.block(false);
    a.test_sig(7, 8);

    a.visible_sig.connect([](std::string a, double b) {
        std::cout << "visible_sig: " << a << " " << b << std::endl;
    });
    a.visible_sig("test", true);
    a.visible_sig("one more test", 0xcc);

    return 0;
}
