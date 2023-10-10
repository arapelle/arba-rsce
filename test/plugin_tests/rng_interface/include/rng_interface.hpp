#pragma once

#include <string_view>

class generator_interface
{
public:
    virtual ~generator_interface() = default;
    virtual int generate() const = 0;
};
