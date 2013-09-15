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
    return 1 + (base_to ? max_digit(base_from, base_to / base_from) : 0);
}

#if defined(__GNUC__) && defined(__LP64__)
typedef __uint128_t IntMax;
#else
typedef std::uintmax_t IntMax;
#endif

// ------------- ArrayHolder ---------------------------------
template<typename Int, Int... args> struct ArrayHolder {
    static const Int data[sizeof...(args)];
};

template<typename Int, Int... args> 
const Int ArrayHolder<Int, args...>::data[sizeof...(args)] = { args... };

// ------------- generate_array_impl -------------------------

template<typename _Int, _Int _SrcBase, IntMax _DstBase, std::size_t _MaxDigit>
struct _Params {
    typedef _Int Int;
    static const Int SrcBase = _SrcBase;
    static const IntMax DstBase = _DstBase;
    static const std::size_t MaxDigit = _MaxDigit;
};

template<std::size_t NIndex, std::size_t DIndex, typename Params, typename Int,
         IntMax Quotient, Int Carry, Int... args> 
struct generate_array_impl {
    enum { SrcBase = Params::SrcBase };
    enum { product = Carry + (Quotient % Params::SrcBase) * (SrcBase-NIndex-1)};
    typedef typename generate_array_impl<NIndex, DIndex-1, Params, Int, 
                                         Quotient/SrcBase, product / SrcBase, 
                                         product % SrcBase, args...
                                        >::result result;
};

template<std::size_t NIndex, typename Params, typename Int, IntMax Quotient, 
         Int Carry, Int... args> 
struct generate_array_impl<NIndex, 0, Params, Int, Quotient, Carry, args...> {
    static const IntMax DstBase = Params::DstBase;
    static const std::size_t MaxDigit = Params::MaxDigit;
    typedef typename generate_array_impl<NIndex-1, MaxDigit - 1, Params, Int, 
                                         DstBase, 0, Carry, args...
                                        >::result result;
};

template<typename Params, typename Int, IntMax Quotient, Int Carry, Int... args>
struct generate_array_impl<0, 0, Params, Int, Quotient, Carry, args...> {
    typedef ArrayHolder<Int, Carry, args...> result;
};

// -------------- generate_array --------------------------------
template<typename Int, Int SrcBase, IntMax DstBase> 
struct generate_array {
    enum { N = max_digit(SrcBase, DstBase) };
    typedef _Params<Int, SrcBase, DstBase, N> Params;
    typedef typename generate_array_impl<SrcBase-1, N-1, Params, Int, DstBase, 0
                                        >::result result;
};

template <typename Int1, typename Int2, Int1 SrcBase>
using binary_base = typename generate_array<Int1, SrcBase, IntTrait<Int2>::base
                                            >::result;

}
}

#endif // __BASE_HPP__
