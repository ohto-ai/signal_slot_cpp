# C++ 实现信号槽
```cpp
class Test {
public:
    OHTOAI_SIGNAL(test_sig, int, int);
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
}
```
```
test_sig: 1 2
one more sig: 1 2
will be removed: 1 2
test_sig: 3 4
one more sig: 3 4
test_sig: 7 8
one more sig: 7 8
```