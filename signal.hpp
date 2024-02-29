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
            template<typename...Params>
            using global_func_t = void (*)(Params...);
            template<typename T, typename ...Params>
            using mem_func_t = void (T::*)(Params...);

            using callback_t = global_func_t<Args...>;
            using slot_func_t = std::function<void(Args...)>;

            using slot_t = std::variant<const signal*, std::weak_ptr<slot_func_t>>;

            struct slot_handle_t {
                slot_handle_t(std::shared_ptr<slot_func_t> _slot_fun) : slot(std::weak_ptr<slot_func_t>(_slot_fun)) {}
                slot_handle_t(const signal* sig) : slot(sig) {}
                bool operator==(const slot_handle_t& _id) const {
                    if (std::holds_alternative<const signal*>(slot)
                        && std::holds_alternative<const signal*>(_id.slot)) {
                        return std::get<const signal*>(slot) == std::get<const signal*>(_id.slot);
                    }
                    else if (std::holds_alternative<std::weak_ptr<slot_func_t>>(_id.slot)){
                        return operator==(id.slot.lock());
                    }
                }
                bool operator==(const slot_ptr_t& _slot) const {
                    if (!std::holds_alternative<std::weak_ptr<slot_func_t>>(slot))
                        return false;
                    auto& _slot = std::get<std::weak_ptr<slot_func_t>>(slot);
                    return _slot && _slot.lock() == _slot;
                }
            private:
                friend class signal;
                const slot_t slot;
            };
        public:
            slot_handle_t connect(callback_t _fun) {
                auto f = std::make_shared<slot_func_t>(_fun);
                slot_funcs.insert(f);
                return f;
            }
            void disconnect(const slot_handle_t& slot_id) {
                if (std::holds_alternative<const signal*>(slot_id.slot)) {
                    slot_sigs.erase(slot_sigs.find(std::get<const signal*>()));
                }
                else {
                    slot_funcs.erase(slot_funcs.find(std::get<std::weak_ptr<slot_func_t>>(slot).lock());
                }
            }

            void emit(Args...args) {
                if (blocked) return;
                for (auto& p_slot : slot_funcs) {
                    std::invoke(*p_slot, std::forward<Args>(args)...);
                }
                for (auto& p_slot : slot_sigs) {
                    std::invoke(*p_slot, std::forward<Args>(args)...);
                }
            }
                

            void operator()(Args...args) {
                return emit(std::forward<Arg>(args)...);
            }

            void block(bool _block = true) {
                blocked = _block;
            }

            bool is_blocked() const {
                return blocked;
            }

        private:
            std::atomic_bool blocked;
            std::unordered_set<std::shared_ptr<slot_func_t>> slot_funcs;
            std::unordered_set<const signal*> slot_sigs;
        };
    }
}

#ifndef OHTOAI_SIGNAL
#define OHTOAI_SIGNAL(name, ...) ohtoai::signal::signal<__VA_ARGS__> name
#endif

#endif // !_OHTOAI_SIGNAL_HPP_
