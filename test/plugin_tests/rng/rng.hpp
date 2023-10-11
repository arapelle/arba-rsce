#pragma once

#include <rng_interface.hpp>
#include <memory>

class generator : public generator_interface
{
public:
    virtual ~generator() = default;
    virtual int generate() const override;
};
