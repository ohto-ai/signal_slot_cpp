#pragma once

#include <tuple>

template <typename ...Args>
struct get_type_index;

template <typename T, typename ...Args>
struct get_type_index <T, T, Args...> {
    constexpr static size_t value = 0u;
};

template <typename T, typename U, typename ...Args>
struct get_type_index <T, U, Args...> {
    constexpr static size_t value = 1u + get_type_index<T, Args...>::value;
};

template <size_t index, typename T, typename ...Args>
struct get_index_type {
    using type = typename get_index_type<index - 1, Args...>::type;
};

template <typename T, typename ...Args>
struct get_index_type<0u, T, Args...> {
    using type = T;
};

// merge 2 tuple type to 1 tuple type
template <typename T1, typename T2>
struct tuple_merge;

template <typename ...Args1, typename ...Args2>
struct tuple_merge<std::tuple<Args1...>, std::tuple<Args2...>> {
    using type = typename std::tuple<Args1..., Args2...>;
};

// reflect tuple type
template <typename T>
struct tuple_reflect;

template <typename T, typename ...Args>
struct tuple_reflect<std::tuple<T, Args...>> {
    using type = typename tuple_merge<typename tuple_reflect<std::tuple<Args...>>::type, std::tuple<T>>::type;
};

template <typename T>
struct tuple_reflect<std::tuple<T>> {
    using type = typename std::tuple<T>;
};

// left and right tuple type
template <size_t count, typename T>
struct tuple_left;

template <typename T, typename ...Args>
struct tuple_left<1u, std::tuple<T, Args...>> {
    using type = std::tuple<T>;
};

template <size_t count, typename T, typename ...Args>
struct tuple_left<count, std::tuple<T, Args...>> {
    using type = typename tuple_merge<std::tuple<T>, typename tuple_left<count - 1u, std::tuple<Args...>>::type>::type;
};

template <size_t count, typename T>
struct tuple_right {
    using type = typename tuple_reflect<typename tuple_left<count, typename tuple_reflect<T>::type>::type>::type;
};
