/*
 * =====================================================================================
 *
 *       Filename:  limbs.hpp
 *
 *    Description:  low-level bigint operation
 *
 *        Version:  1.0
 *        Created:  09/12/2013 11:47:07 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#ifndef __LIMB_HPP__
#define __LIMB_HPP__

#include <type_traits>
#include <algorithm>
#include <iostream>
#include <iomanip>

#include "traits.hpp"

#define BEGIN_END_SIZE(C) C.begin(), C.end(), C.size()
#define BEGIN_END(C) C.begin(), C.end()

namespace bigint {
namespace internal {

// -------------------------------------------------------------------------
enum { LIMBS_GT, LIMBS_EQ, LIMBS_LT };

// -------------------------------------------------------------------------
template <typename Iterator>
std::ostream&
limbs_print (std::ostream& os, Iterator b, Iterator e)
{
    std::size_t const width = sizeof(*b) << 1;

    auto rb = std::reverse_iterator<Iterator>(e),
         re = std::reverse_iterator<Iterator>(b);

    for (; rb != re; ++rb) 
        os << std::hex << std::setw(width) << std::setfill('0') 
           << *rb << ' ';
    return os;
}

// -------------------------------------------------------------------------
template <typename Container1, typename Container2>
Container2
limbs_from_base (Container1 const& source, Container2 &dest)
{
    typedef typename Container2::value_type Int;
    typedef typename Container1::value_type Int2;
    std::size_t const bits = IntTrait<Int>::bits;
    std::size_t const base = 10;
    typename IntTrait<Int>::DoubleInt const dest_base = 1LLU << bits;
    Int2 const dest_base_in_src_base[] = { 6, 9, 7, 6, 9, 4, 9, 2, 4 };

    std::ceil(std::log(dest_base) / std::log(base));
}


// -------------------------------------------------------------------------
template <typename Iterator1, typename Iterator2, typename OutputIterator>
bool
limbs_add (Iterator1 b1, Iterator1 e1, std::size_t sz1,
           Iterator2 b2, Iterator2 e2, std::size_t sz2,
           OutputIterator b3)
{
    typedef typename std::iterator_traits<Iterator1>::value_type Int;
    typedef typename IntTrait<Int>::DoubleInt DoubleInt;

    DoubleInt _carry = 0;
    Iterator1 _e1 = b1 + std::min(sz1, sz2);

    for (; b1 != _e1; ++b1, ++b2, ++b3) {
        (_carry += *b1) += *b2;
        *b3 = _carry;
        _carry >>= IntTrait<Int>::bits; // carry
    }

    Int carry = _carry;
    if (sz1 >= sz2) {
        for (; carry && b1 != e1; ++b1, ++b3) {
            *b3 = *b1 + carry;
            carry = *b3 < *b1;
        }
        for (; b1 != e1; ++b1, ++b3)
            *b3 = *b1;
    } else {
        for (; carry && b2 != e2; ++b2, ++b3) {
            *b3 = *b2 + carry;
            carry = *b3 < *b2;
        }
        for (; b2 != e2; ++b2, ++b3)
            *b3 = *b2;
    }

    return carry;
}

// -------------------------------------------------------------------------
template <typename Iterator1, typename Iterator2>
bool
limbs_add_inplace (Iterator1 b1, Iterator1 e1, std::size_t sz1,
                   Iterator2 b2, Iterator2 e2, std::size_t sz2,
                   Iterator1 e1out)
{
    typedef typename std::iterator_traits<Iterator1>::value_type Int;
    typedef typename IntTrait<Int>::DoubleInt DoubleInt;

    DoubleInt _carry = 0;
    Iterator1 const _e1 = b1 + std::min(sz1, sz2);

    for (; b1 != _e1; ++b1, ++b2) {
        (_carry += *b1) += *b2;
        *b1 = _carry;
        _carry >>= IntTrait<Int>::bits; // carry
    }

    Int carry = _carry;
    if (sz1 >= sz2) {
//        Int oldb1;
        for (; carry && b1 != e1; ++b1) { 
//            oldb1 = *b1;
            *b1 += carry;
            carry = !*b1 && carry; // carry if overflow
//            carry = *b1 < oldb1;
        }
        if (carry && b1 != e1out)
            *b1 = 1;
    } else {
        for (; carry && b2 != e2 && b1 != e1out; ++b2, ++b1) {
            *b1 = *b2 + carry;
            carry = *b1 < *b2; // carry if overflow
        }
    }

    return carry;
}

template <typename Container1, typename Container2, typename Container3>
static typename std::enable_if<std::is_same<typename Container1::value_type,
                                            typename Container2::value_type
                              >::value, void>::type
limbs_add (Container1 const &c1, Container2 const &c2, Container3 &c3)
{
    c3.resize(std::max(c1.size(), c2.size()));

    // if has carry
    if (limbs_add(BEGIN_END_SIZE(c1), BEGIN_END_SIZE(c2), c3.begin()))
        c3.push_back(1);
}


// -----------------------------------------------------------------------
template <typename Iterator1, typename Iterator2, typename OutputIterator>
bool
limbs_subtract (Iterator1 b1, Iterator1 e1, std::size_t sz1,
                Iterator2 b2, Iterator2 e2, std::size_t sz2,
                OutputIterator b3)
{
    typedef typename std::iterator_traits<Iterator1>::value_type Int;
    typedef typename IntTrait<Int>::DoubleInt DoubleInt;

    std::size_t const bits = IntTrait<Int>::bits;

    DoubleInt _carry = 0;
    DoubleInt const exponent = DoubleInt(1) << bits;
    for (; b2 != e2; ++b1, ++b2, ++b3) {
        _carry = (*b1 | exponent) - _carry - *b2;
        *b3 = _carry;
        _carry = (_carry >> bits) ^ DoubleInt(1); // _carry
    }

    Int carry = _carry;
    for (; carry && b1 != e1; ++b1, ++b3) {
        *b3 = *b1 - carry;
        carry = (*b3 > *b1); // carry if overflow
    }

    for (; b1 != e1; ++b1, ++b3) 
        *b3 = *b1;

    return carry;
}

// -----------------------------------------------------------------------
template <typename Iterator1, typename Iterator2>
bool
limbs_subtract_inplace (Iterator1 b1, Iterator1 e1, std::size_t sz1,
                        Iterator2 b2, Iterator2 e2, std::size_t sz2,
                        Iterator1 e1out)
{
    typedef typename std::iterator_traits<Iterator1>::value_type Int;
    typedef typename IntTrait<Int>::DoubleInt DoubleInt;

    std::size_t const bits = IntTrait<Int>::bits;

    DoubleInt _carry = 0;
    DoubleInt const exponent = DoubleInt(1) << bits;
    for (; b2 != e2; ++b1, ++b2) {
        _carry = (*b1 | exponent) - _carry - *b2;
        *b1 = _carry;
        _carry = (_carry >> bits) ^ DoubleInt(1); // _carry
    }

    
    Int carry = _carry;
//    Int oldb1;
    for (; carry && b1 != e1out; ++b1) {
//        oldb1 = *b1;
        *b1 -= carry;
        carry = (*b1 == std::numeric_limits<Int>::max()) && carry;
//        carry = *b1 > oldb1;
    }

    return carry;
}

// -----------------------------------------------------------------------
template <typename Container>
void limbs_trim (Container &c)
{
    auto ic = c.rbegin();
    while (ic != c.rend() && !*ic) ++ic;
    c.erase(ic.base(), c.end());
}

template <typename Container1, typename Container2, typename Container3>
static typename std::enable_if<std::is_same<typename Container1::value_type,
                                            typename Container2::value_type
                              >::value, void>::type
limbs_subtract (Container1 const &c1, Container2 const &c2, Container3 &c3)
{
    c3.resize(c1.size());
    limbs_subtract(BEGIN_END_SIZE(c1), BEGIN_END_SIZE(c2), c3.begin());

    limbs_trim(c3);
}


// ------------------------------------------------------------------------
template <typename Iterator1, typename Iterator2, typename OutputIterator>
void
limbs_multiply_dispatch (Iterator1 b1, Iterator1 e1, std::size_t sz1,
                         Iterator2 b2, Iterator2 e2, std::size_t sz2,
                         OutputIterator b3, OutputIterator e3);

// ------------------------------------------------------------------------
template <typename Iterator1, typename Iterator2, typename OutputIterator>
void
limbs_multiply_karatsuba (Iterator1 b1, Iterator1 e1, Iterator2 b2, 
                         Iterator2 e2, OutputIterator b3, OutputIterator e3,
                         std::size_t n)
{
    typedef typename std::iterator_traits<Iterator1>::value_type Int;
    typedef typename IntTrait<Int>::DoubleInt DoubleInt;
    typedef std::vector<Int> Limbs;

    if (n == 1) {
        DoubleInt product = *b1;
        product *= *b2;

        *b3 = product;
        *++b3 = product >> IntTrait<Int>::bits;

        return;
    } 
#if defined(__GNUC__) && defined(__LP64__) && !defined(USE_64BIT_LIMB)
//#if 0
    else if (n == 2) {
        std::size_t const bits = IntTrait<Int>::bits;
        typename IntTrait<Int>::QuadInt product = *b1, tmp = *b2;
        product |= DoubleInt(*(b1+1)) << bits;
        tmp |= DoubleInt(*(b2+1)) << bits;
        product *= tmp;
        *b3 = product, *(b3+1) = product >> bits,
        *(b3+2) = product >> (2*bits), *(b3+3) = product >> (3*bits);

        return;
    }
#endif // __GNUC__


    /*
     * C1 = A * 2^X + B
     * C2 = C * 2^X + D
     * C3 = C1 * C2 = AC * 2*2X + ((A+B)(C+D) - AC - BD) * 2^X + BD
     */

    auto const half = n-n/2;
    // res = bd
    limbs_multiply_karatsuba(b1, b1 + half, b2, b2 + half,
                            b3, b3 + 2 * half, half); 
    // res = ac * 2^half + bd
    limbs_multiply_karatsuba(b1 + half, e1, b2 + half, e2,
                            b3 + 2*half, e3, n - half); 

    // (a+b)(c+d) - ac - bd
    Limbs ad_add_bc, a_add_b, c_add_d;
//    auto maxsz = half;
    a_add_b.reserve(half+1), a_add_b.resize(half);
    if (limbs_add(b1, b1+half, half, b1+half, e1, n-half, a_add_b.begin()))
        a_add_b.push_back(1);//, maxsz = half + 1;

    c_add_d.reserve(half+1), c_add_d.resize(half);
    if (limbs_add(b2, b2+half, half, b2+half, e2, n-half, c_add_d.begin()))
        c_add_d.push_back(1);//, maxsz = half+1;
    
    // TODO: use limbs_multiply_dispatch instead
//    ad_add_bc.resize(maxsz * 2);
//    a_add_b.resize(maxsz), c_add_d.resize(maxsz);
//    limbs_multiply_karatsuba(BEGIN_END(a_add_b), BEGIN_END(c_add_d), 
//                            BEGIN_END(ad_add_bc), maxsz);
    ad_add_bc.resize(a_add_b.size() + c_add_d.size());
    if (a_add_b.size() > c_add_d.size())
        limbs_multiply_dispatch(BEGIN_END_SIZE(a_add_b), 
                                BEGIN_END_SIZE(c_add_d), BEGIN_END(ad_add_bc));
    else if (a_add_b.size() < c_add_d.size())
        limbs_multiply_dispatch(BEGIN_END_SIZE(c_add_d), 
                                BEGIN_END_SIZE(a_add_b), BEGIN_END(ad_add_bc));
    else
        limbs_multiply_karatsuba(BEGIN_END(a_add_b), BEGIN_END(c_add_d),
                                BEGIN_END(ad_add_bc), a_add_b.size());

//    limbs_subtract(BEGIN_END_SIZE(ad_add_bc), 
//                   b3, b3 + 2*half, 2*half, ad_add_bc.begin());
    limbs_subtract_inplace(BEGIN_END_SIZE(ad_add_bc),
                           b3, b3 + 2*half, 2*half, ad_add_bc.end());
//    limbs_subtract(BEGIN_END_SIZE(ad_add_bc), 
//                   b3 + 2*half, e3, 2*(n-half), ad_add_bc.begin());
    limbs_subtract_inplace(BEGIN_END_SIZE(ad_add_bc), 
                           b3 + 2*half, e3, 2*(n-half), ad_add_bc.end());

    //
    limbs_trim(ad_add_bc);
//    limbs_add(b3+half, e3, 2*n-half, BEGIN_END_SIZE(ad_add_bc), b3+half);
    limbs_add_inplace(b3 + half, e3, 2*n-half, BEGIN_END_SIZE(ad_add_bc), e3);
}

// ------------------------------------------------------------------------
template <typename Iterator1, typename Iterator2, typename OutputIterator>
void
limbs_multiply_dispatch (Iterator1 b1, Iterator1 e1, std::size_t sz1,
                         Iterator2 b2, Iterator2 e2, std::size_t sz2,
                         OutputIterator b3, OutputIterator e3)
{
    typedef typename std::iterator_traits<Iterator1>::value_type Int;

#ifndef NDEBUG
    std::cerr << "limbs_multiply_dispatch " << sz1 << " * " << sz2 << '\n';
#endif

    std::vector<Int> ctmp;

    std::size_t offset = 0;
    for (auto i = sz1 / sz2; i; --i, offset += sz2) {
        ctmp.assign(2 * sz2, 0);
        limbs_multiply_karatsuba (b1 + offset, b1 + offset + sz2,
                                 b2, e2, BEGIN_END(ctmp), sz2);
#ifndef NDEBUG
        std::cerr << "i = " << i << "\nctmp = ";
        limbs_print(std::cerr, BEGIN_END(ctmp)) << '\n';
#endif

//        limbs_add (b3 + offset, b3 + offset + sz2, sz2, 
//                   BEGIN_END_SIZE(ctmp), b3 + offset);
        if (limbs_add_inplace(b3 + offset, b3 + offset + sz2*2, sz2*2,
                              BEGIN_END_SIZE(ctmp), b3 + offset + sz2*2))
            *(b3 + offset + sz2*2) = 1;

#ifndef NDEBUG
        std::cerr << "c3 = ";
        limbs_print(std::cerr, b3, e3) << "\n\n";
#endif
    }

    if (offset < sz1) {
        ctmp.assign(sz2 + (sz1 - offset), 0);
        limbs_multiply_dispatch(b2, e2, sz2, b1 + offset, e1, sz1 - offset, 
                                BEGIN_END(ctmp));
#ifndef NDEBUG
        std::cerr << "extra\nctmp = ";
        limbs_print(std::cerr, BEGIN_END(ctmp)) << '\n';
#endif

                                
//        limbs_add (b3 + offset, e3, sz1 - offset, 
//                   BEGIN_END_SIZE(ctmp), b3 + offset);
        limbs_add_inplace(b3 + offset, e3, sz1+sz2 - offset,
                          BEGIN_END_SIZE(ctmp), e3);
#ifndef NDEBUG
        std::cerr << "c3 = ";
        limbs_print(std::cerr, b3, e3) << "\n\n";
#endif
    }

#ifndef NDEBUG
    std::cerr << "\n\n";
#endif
}

// ------------------------------------------------------------------------
template <typename Container1, typename Container2, typename Container3>
static typename std::enable_if<std::is_same<typename Container1::value_type,
                                            typename Container2::value_type
                              >::value, void>::type
limbs_multiply (Container1 const &c1, Container2 const &c2, Container3 &c3)
{
    c3.assign(c1.size() + c2.size(), 0);

    if (c1.size() > c2.size())
        limbs_multiply_dispatch(BEGIN_END_SIZE(c1), BEGIN_END_SIZE(c2), 
                                BEGIN_END(c3));
    else if(c1.size() < c2.size())
        limbs_multiply_dispatch(BEGIN_END_SIZE(c2), BEGIN_END_SIZE(c1), 
                                BEGIN_END(c3));
    else
        limbs_multiply_karatsuba(BEGIN_END(c1), BEGIN_END(c2), BEGIN_END(c3),
                                c1.size());

    limbs_trim(c3);
}



// ------------------------------------------------------------------------
template <typename Container1, typename Container2>
static typename std::enable_if<std::is_same<typename Container1::value_type,
                                            typename Container2::value_type
                              >::value, int>::type
limbs_cmp (Container1 const &c1, Container2 const &c2)
{
    if (c1.size() > c2.size()) return LIMBS_GT;
    else if (c1.size() < c2.size()) return LIMBS_LT;

    for (std::size_t i = c1.size() - 1; i; --i) {
        if (c1[i] > c2[i]) return LIMBS_GT;
        else if (c1[i] < c2[i]) return LIMBS_LT;
    }
    return LIMBS_EQ;
}


} // namespace internal
} // namespace bigint

#endif // __LIMB_HPP__
