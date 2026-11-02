/**
 * @file ports.cpp
 * @brief Instantiates the driver over the documented port, so every member compiles in full.
 */
#include "my_i2c.hpp"

template class pf1550::PF1550<MyI2c>;
