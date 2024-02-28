#pragma once

#ifndef _OHTOAI_SIGNAL_HPP_
#define _OHTOAI_SIGNAL_HPP_

#include <memory>
#include <list>
#include <functional>

namespace ohtoai{
    namespace signal {
        template <typename...Args>
        class signal {
            using slot_t = std::function<void(Args...)>;
            using slot_ptr_t = std::shared_ptr<slot_t>;
            struct slot_id_t {
                slot_id_t(slot_ptr_t _slot) : slot(_slot) {}
                bool operator==(const slot_id_t& _id) const {
                    return slot == _id.slot;
                }
                bool operator!=(const slot_id_t& _id) const {
                    return slot != _id.slot;
                }
                bool operator==(const slot_ptr_t& _slot) const {
                    return slot == _slot;
                }
                bool operator!=(const slot_ptr_t& _slot) const {
                    return slot != _slot;
                }
            private:
                const slot_ptr_t slot;
            };
        public:
            template <typename Callback>
            slot_id_t connect(Callback _fun) {
                auto f = std::make_shared<slot_t>(_fun);
                slots.push_back(f);
                return f;
            }
            void disconnect(const slot_id_t& slot_id) {
                for (auto it = slots.begin(); it != slots.end(); ++it) {
                    if (slot_id  == *it) {
                        slots.erase(it);
                        break;
                    }
                }
            }

            void operator()(Args...args) {
                for (auto& p_slot : slots) {
                    auto&slot = *p_slot;
                    slot(std::forward<Args>(args)...);
                }
            }
        private:
            std::list<std::shared_ptr<slot_t>> slots;
        };
    }
}

#ifndef OHTOAI_SIGNAL
#define OHTOAI_SIGNAL(name, ...) ohtoai::signal::signal<__VA_ARGS__> name
#endif

#endif // !_OHTOAI_SIGNAL_HPP_
