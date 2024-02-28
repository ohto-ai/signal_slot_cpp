#pragma once

#ifndef _OHTOAI_SIGNAL_HPP_
#define _OHTOAI_SIGNAL_HPP_

#include <memory>
#include <unordered_set>
#include <atomic>
#include <functional>

namespace ohtoai{
    namespace signal {
        template <typename...Args>
        class signal {
            using slot_t = std::function<void(Args...)>;
            using slot_ptr_t = std::shared_ptr<slot_t>;
            struct slot_handle_t {
                slot_handle_t(slot_ptr_t _slot) : slot(_slot) {}
                bool operator==(const slot_handle_t& _id) const {
                    return slot == _id.slot;
                }
                bool operator!=(const slot_handle_t& _id) const {
                    return slot != _id.slot;
                }
                bool operator==(const slot_ptr_t& _slot) const {
                    return slot == _slot;
                }
                bool operator!=(const slot_ptr_t& _slot) const {
                    return slot != _slot;
                }
            private:
                friend class signal;
                const slot_ptr_t slot;
            };
        public:
            template <typename Callback>
            slot_handle_t connect(Callback _fun) {
                auto f = std::make_shared<slot_t>(_fun);
                slots.insert(f);
                return f;
            }
            void disconnect(const slot_handle_t& slot_id) {
                slots.erase(slots.find(slot_id.slot));
            }

            void operator()(Args...args) {
                if (blocked) return;
                for (auto& p_slot : slots) {
                    auto&slot = *p_slot;
                    slot(std::forward<Args>(args)...);
                }
            }

            void block(bool _block = true) {
                blocked = _block;
            }

            bool is_blocked() const {
                return blocked;
            }

        private:
            std::atomic_bool blocked;
            std::unordered_set<std::shared_ptr<slot_t>> slots;
        };
    }
}

#ifndef OHTOAI_SIGNAL
#define OHTOAI_SIGNAL(name, ...) ohtoai::signal::signal<__VA_ARGS__> name
#endif

#endif // !_OHTOAI_SIGNAL_HPP_
