#pragma once

#include <new>
#include <string_view>
#include <iostream>
#include <memory>
#include <utility>

#include <cassert>
#include <cstdint>

#include "Platform.h"

#define ASSERT(x, ...) assert(x)

template<typename T>
using Instance = std::shared_ptr<T>;

template<typename T, typename... Args>
static Instance<T> Make(Args&&... args) { return std::make_shared<T>(std::forward<Args>(args)...); }

class Utils
{
private:
#if defined FUNC_SIG_PREFIX
    template<typename Type>
    [[nodiscard]] static constexpr auto GetLongName() noexcept
    {
        std::string_view pretty_function{ FUNC_SIG };
        auto first = pretty_function.find_first_not_of(' ', pretty_function.find_first_of(FUNC_SIG_PREFIX) + 1);
        auto value = pretty_function.substr(first, pretty_function.find_last_of(FUNC_SIG_SUFFIX) - first);
        return value;
    }

public:
    template<typename Type>
    [[nodiscard]] static constexpr auto GetName() noexcept
    {
        std::string_view long_name = GetLongName<Type>();
        auto first = long_name.find_last_of("::");
        if (first == std::string_view::npos)
            first = long_name.find_last_of(' ');
        else
            first++;
        if (first == std::string_view::npos)
            return long_name;
        return long_name.substr(first, long_name.length() - first);
    }
#endif
};