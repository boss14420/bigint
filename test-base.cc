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
    typedef std::uint64_t Int;
    typedef typename bigint::internal::IntTrait<Int>::DoubleInt DoubleInt;

    constexpr std::size_t bits = (sizeof (Int) << 3);
    constexpr DoubleInt max = DoubleInt(1) << bits;
//    std::cout << max << " = ";
    auto &&array = bigint::internal::binary_base<char, Int, 10>::data;
//    auto &&array = bigint::internal::base_convert<char, DoubleInt, 10, 256>::to;
    auto max_digit = bigint::internal::max_digit(10, max);
    std::size_t i = 0;
    for (auto c : array)
    {
        std::cout << static_cast<char>(c+'0');
        if (!(++i % max_digit))
            std::cout << '\n';
    }
    std::cout << '\n';

    return 0;
}
