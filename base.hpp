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
constexpr std::size_t max_digit(Int1 base_from, Int2 base_to)
{
    return 1 + (base_to ? max_digit(base_from, base_to / base_from) : -1);
}


template <std::size_t sz> struct sizetype 
{
    static const std::size_t value = sz;
};

template <typename Int1, typename Int2, Int1 From, 
          typename IntTrait<Int2>::DoubleInt To = IntTrait<Int2>::base,
          typename IntTrait<Int2>::DoubleInt Quotient = To,
          typename I = sizetype<0>>
struct base_convert : 
    public base_convert<Int1, Int2, From, To, Quotient / From,
                        sizetype<I::value+1>>
{
    static const int dummy;
};

template <typename Int1, typename Int2, Int1 From, Int2 To>
struct base_convert<Int1, Int2, From, To, 0, sizetype<3>>
{
    static const int dummy;
    static Int1 to[3];
};

template <typename Int1, typename Int2, Int1 From, 
          typename IntTrait<Int2>::DoubleInt To,
          typename IntTrait<Int2>::DoubleInt Quotient, typename I>
const int base_convert<Int1, Int2, From, To, Quotient, I>::dummy
    = base_convert<Int1, Int2, From, To, 0, sizetype<3>>
        ::to[I::value] = Quotient % From 
                    + 0 * base_convert<Int1, Int2, From, To, Quotient / From,
                                       sizetype<I::value + 1>>::dummy;

template <typename Int1, typename Int2, Int1 From, Int2 To>
Int1 base_convert<Int1, Int2, From, To, 0, sizetype<3>>
                            ::to[3];


template struct base_convert<char, std::uint8_t, 3>;
//template struct base_convert<char, std::uint16_t, 3>;
//template struct base_convert<char, std::uint32_t, 3>;
//template struct base_convert<char, std::uint64_t, 3>;

}
}

#endif // __BASE_HPP__
