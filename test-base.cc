/*
 * =====================================================================================
 *
 *       Filename:  test-base.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/15/2013 09:19:29 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <iostream>
#include <cstdint>
#include <typeinfo>
#include "base.hpp"

int main(int argc, char *argv[]) {
    typedef std::uint8_t Int;
    typedef std::uint16_t DoubleInt;

    constexpr std::size_t bits = (sizeof (Int) << 3);
    constexpr DoubleInt max = DoubleInt(1) << bits;
    std::cout << max << " = ";
    auto &&array = bigint::internal::base_convert<char, Int, 10>::to;
    for (auto c : array)
    {
        std::cout << static_cast<char>(c+'0') << ' ';
    }
    std::cout << '\n';

    std::cout << bigint::internal::max_digit(10, max) << '\n';

    return 0;
}