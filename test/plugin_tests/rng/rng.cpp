#include "rng.hpp"
#include <vector>
#include <iostream>

int generator::generate() const
{
    return rand() % 10 + 1;
}


extern "C" std::unique_ptr<generator_interface> make_unique_instance()
{
    return std::make_unique<generator>();
}
