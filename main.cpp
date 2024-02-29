#include "signal.hpp"
#include <iostream>
#include <string>

class Test {
public:
    OHTOAI_SIGNAL(test_sig, std::string, int);
    OHTOAI_SIGNAL(test_sig2, std::string, int);
    OHTOAI_SIGNAL(empty_sig);
};

int main() {
    Test a;
    a.empty_sig.connect([&] {
        std::cout << __LINE__ << " " << "No args func" << std::endl;
        });
    a.test_sig.connect([](std::string a, int b) {
        std::cout << __LINE__ << " " << a << " " << b << std::endl;
    });

    a.test_sig2.connect([](std::string a, int b) {
        std::cout << __LINE__ << " " << a << " " << b << std::endl;
        });

    auto temp_slot = a.test_sig.connect([](std::string a, int b) {
        std::cout << __LINE__ << " " << "temp lambda: " << a << " " << b << std::endl;
        });
    a.test_sig("Test sig", __LINE__);
    auto sig_id = a.test_sig.connect(a.test_sig2);
    a.test_sig("Connect sig to sig2", __LINE__);
    a.test_sig.disconnect(temp_slot);
    a.test_sig("Disconnect temp slot", __LINE__);
    a.test_sig.block();
    a.test_sig("Test block sig", __LINE__);
    a.test_sig.block(false);
    a.test_sig("Test unblock sig", __LINE__);
    a.empty_sig.emit();

    return 0;
}
