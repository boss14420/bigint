/*
 * =====================================================================================
 *
 *       Filename:  traits.hpp
 *
 *    Description:  integer traits
 *
 *        Version:  1.0
 *        Created:  09/12/2013 11:57:53 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __TRAITS_HPP__
#define __TRAITS_HPP__

#include <cstdint>

namespace bigint {
namespace internal {

template <typename T> struct IntTrait {};

#if defined( __GNUC__) && defined(__LP64__)
template <>
struct IntTrait<std::uint64_t> {
    typedef std::uint64_t Int;
    typedef __uint128_t DoubleInt;

    static const std::size_t size = 8;
    static const std::size_t bits = 64;
    static const DoubleInt base = DoubleInt(1) << bits;
};
#endif

template <>
struct IntTrait<std::uint32_t> {
    typedef std::uint32_t Int;
    typedef std::uint64_t DoubleInt;
#if defined(__GNUC__) && defined(__LP64__)
    typedef __uint128_t QuadInt;
#endif
    static const std::size_t size = 4;
    static const std::size_t bits = 32;
    static const DoubleInt base = DoubleInt(1) << bits;
};

template <>
struct IntTrait<std::uint16_t> {
    typedef std::uint16_t Int;
    typedef std::uint32_t DoubleInt;
    typedef std::uint64_t QuadInt;
    static const std::size_t size = 2;
    static const std::size_t bits = 16;
    static const DoubleInt base = DoubleInt(1) << bits;
};

template <>
struct IntTrait<std::uint8_t> {
    typedef std::uint8_t Int;
    typedef std::uint16_t DoubleInt;
    typedef std::uint32_t QuadInt;
    static const std::size_t size = 1;
    static const std::size_t bits = 8;
    static const DoubleInt base = DoubleInt(1) << bits;
};


} // namespace internal
} // namespace bigint

#endif // __TRAITS_HPP__
