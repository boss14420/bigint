/*
 * =====================================================================================
 *
 *       Filename:  base.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/15/2013 07:01:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __BASE_HPP__
#define __BASE_HPP__

#include "traits.hpp"

namespace bigint {
namespace internal {


template <typename Int1, typename Int2>
constexpr std::size_t max_digit(Int1 base_from)
{
    auto base_to = IntTrait<Int1>::base;
    std::size_t digits = 0;
    do {
        ++digits;
        base_to /= base_from;
    } while (base_to);
    return digits;
}


template <std::size_t sz> struct sizetype 
{
    static const std::size_t value = sz;
};

template <typename Int1, typename Int2, Int2 From,
          typename IntTrait<Int1>::DoubleInt To = IntTrait<Int1>::base,
          typename I = sizetype<0>>
struct base_convert : 
    public base_convert<Int1, Int2, From, To / From,
                        sizetype<I::value+1>>
{
    static const int dummy;
};

template <typename Int1, typename Int2, Int2 From>
struct base_convert<Int1, Int2, From, 0, sizetype<IntTrait<Int1>::bits>>
{
    static const int dummy;
//    static const std::size_t max_digit = IntTrait<Int1>::bits;
    static Int2 to[IntTrait<Int1>::bits];
};

template <typename Int1, typename Int2, Int2 From, 
          typename IntTrait<Int1>::DoubleInt To, typename I>
const int base_convert<Int1, Int2, From, To, I>::dummy
    = base_convert<Int1, Int2, From, 0, sizetype<IntTrait<Int1>::bits>>
        ::to[I::value] = To % From 
                    + 0 * base_convert<Int1, Int2, From, To / From,
                                       sizetype<I::value + 1>>::dummy;

template <typename Int1, typename Int2, Int2 From>
Int2 base_convert<Int1, Int2, From, 0, sizetype<IntTrait<Int1>::bits>>
                            ::to[IntTrait<Int1>::bits];


template struct base_convert<std::uint16_t, char, 10>;
template struct base_convert<std::uint32_t, char, 10>;
template struct base_convert<std::uint64_t, char, 10>;

}
}

#endif // __BASE_HPP__
