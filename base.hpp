/*
 * =====================================================================================
 *
 *       Filename:  base.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/15/201max_digit(From, To) 07:01:56 PM
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


template <typename IntType, IntType I> struct inttype
{
    static const IntType value = I;
};
template <std::size_t sz> using sizetype = inttype<std::size_t, sz>;


template <typename Int1, typename Int2, Int1 From, Int2 To,
          typename Quotient = inttype<Int2, To>,
          typename I = sizetype<0>>
struct base_convert : 
    public base_convert<Int1, Int2, From, To, 
                        inttype<Int2, Quotient::value / From>,
                        sizetype<I::value+1>>
{
    static const int dummy;
};

template <typename Int1, typename Int2, Int1 From, Int2 To>
struct base_convert<Int1, Int2, From, To, inttype<Int2, 0>, 
                    sizetype<max_digit(From, To)>>
{
    static const int dummy;
    static Int1 to[max_digit(From, To)];
};

template <typename Int1, typename Int2, Int1 From, Int2 To,
          typename Quotient, typename I>
const int base_convert<Int1, Int2, From, To, Quotient, I>::dummy
    = base_convert<Int1, Int2, From, To, inttype<Int2, 0>, 
                   sizetype<max_digit(From, To)>>
        ::to[I::value] = Quotient::value % From 
                    + 0 * base_convert<Int1, Int2, From, To, 
                                       inttype<Int2, Quotient::value / From>,
                                       sizetype<I::value + 1>>::dummy;

template <typename Int1, typename Int2, Int1 From, Int2 To>
Int1 base_convert<Int1, Int2, From, To, inttype<Int2, 0>, 
                  sizetype<max_digit(From, To)>>::to[max_digit(From, To)];

template <typename Int1, typename Int2, Int1 From, Int2 To>
const int base_convert<Int1, Int2, From, To, inttype<Int2, 0>, 
                       sizetype<max_digit(From, To)>>::dummy = 0;

template <typename Int1, typename Int2, Int1 From>
struct binary_base : base_convert<Int1, typename IntTrait<Int2>::DoubleInt,
                                  From, IntTrait<Int2>::base>
{};

template struct base_convert<char, std::uint16_t, 
                             10, IntTrait<std::uint8_t>::base>;
template struct base_convert<char, std::uint32_t, 
                             10, IntTrait<std::uint16_t>::base>;
template struct base_convert<char, std::uint64_t, 
                             10, IntTrait<std::uint32_t>::base>;
#if defined(__GNUC__) && defined(__LP64__)
template struct base_convert<char, __uint128_t,
                             10, IntTrait<std::uint64_t>::base>;
#endif

}
}

#endif // __BASE_HPP__
