/*
 * =====================================================================================
 *
 *       Filename:  bigint.hh
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/11/2013 08:59:10 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef __BIGINT_HH__
#define __BIGINT_HH__

#include <deque>
#include <type_traits>
#include <limits>
#include <string>
#include <iostream>

#include "traits.hpp"
#include "limbs.hpp"

namespace bigint {

template <typename T = std::uint32_t, typename Container = std::deque<T>>
class BigInt
{
public:
    typedef typename internal::IntTrait<T>::Int Int;
    typedef typename internal::IntTrait<T>::DoubleInt DoubleInt;

public:
    BigInt() = default;

    // -----------------------------------------------------------------------
    BigInt(bool sign, Container const &value)
        : sign(sign), value(value)
    {}

    BigInt(bool sign, Container&& value)
        : sign(sign), value(std::move(value))
    {}

    // ---------- Constructor for small unsigned integer ---------------------
    template <typename IntType, 
              typename std::enable_if<((sizeof(IntType) <= sizeof(Int)) && 
                                            std::is_unsigned<IntType>::value), 
                                      int>::type = 0>
    BigInt(IntType i)
        : sign(false)
    {
        value.push_back(i);
    }


    // ---------- Constructor for small signed integer ---------------------
    template <typename IntType, 
              typename std::enable_if<(sizeof(IntType) <= sizeof(Int)) && 
                                            std::is_signed<IntType>::value, 
                                      int>::type = 0>
    BigInt(IntType i)
    {
        sign = i < 0;
        value.push_back(std::abs(i));
        if (i == std::numeric_limits<decltype(i)>::lowest())
            value.push_back(1);
    }


    // ----------- Constructor for big unsigned integer --------------------
    template <typename IntType, int Limbs = sizeof(IntType) / sizeof(Int),
              typename std::enable_if<((sizeof(IntType) > sizeof(Int)) && 
                                            std::is_unsigned<IntType>::value), 
                                      IntType>::type = 0>
    BigInt(IntType i)
        : sign(false)
    {

    }


    // ----------- Constructor for big signed integer ----------------------
    template <typename IntType, int Limbs = sizeof(IntType) / sizeof(Int),
              typename std::enable_if<((sizeof(IntType) > sizeof(Int)) && 
                                            std::is_signed<IntType>::value), 
                                      IntType>::type = 0>
    BigInt(IntType i)
    {

    }

    // ----------- "Copy" constructor for other type of container ----------

    template <typename T2, typename Container2>
    friend class BigInt;

    template <typename Container2, 
              typename std::enable_if<
                    std::is_same<T, typename Container2::value_type >::value, 
                    int>::type = 0>
    BigInt(BigInt<T, Container2> const &bi)
        : sign(bi.sign), value(bi.value.begin(), bi.value.end())
    {}

    // ---------------------------------------------------------------------
    BigInt (std::string const &number);

public:
    // ----------------------------------------------------------------
    BigInt& operator<<= (std::size_t bits)
    {
        const std::size_t bitsPerLimb = bits & ((sizeof(Int) << 3) - 1);
        const std::size_t additionLimb = bits / (sizeof(Int) << 3);

        // shift
        Int rem = 0, tmprem;
        for (Int &limb : value) {
            tmprem = limb >> ((sizeof(Int) << 3) - bitsPerLimb);
            limb <<= bitsPerLimb;
            limb |= rem;
            rem = tmprem;
        }
        if (rem) value.push_back(rem);

        value.insert(value.begin(), additionLimb, 0);

        return *this;
    }

    // ----------------------------------------------------------------
    BigInt operator<< (std::size_t bits) const {
        auto tmp = *this;
        tmp <<= bits;
        return tmp;
    }

    // ----------------------------------------------------------------
    BigInt& operator>>= (std::size_t bits)
    {
        const std::size_t bitsPerLimb = bits & ((sizeof(Int) << 3) - 1);
        const std::size_t removal = bits / (sizeof(Int) << 3);

        value.erase(value.begin(), value.begin() + removal);

        DoubleInt rem = 0, tmprem;
        for (auto il = value.rbegin(); il != value.rend(); ++il) {
            tmprem = *il & ((1 << bitsPerLimb) - 1);
            *il >>= bitsPerLimb;
            *il |= rem << ((sizeof(Int) << 3) - bitsPerLimb);
            rem = tmprem;
        }
        if (!value.back()) value.pop_back();

        return *this;
    }

    // ----------------------------------------------------------------
    BigInt operator>> (std::size_t bits) const {
        auto tmp = *this;
        tmp >>= bits;
        return tmp;
    }

    template <typename Container2>
    BigInt& operator+= (BigInt<T, Container2> const &bi)
    {
        if (sign == bi.sign) {
            internal::limbs_add(value, bi.value, value);
        } else {
            switch (internal::limbs_cmp(value, bi.value)) {
                case internal::LIMBS_GT:
                    internal::limbs_subtract(value, bi.value, value);
                    break;
                case internal::LIMBS_EQ:
                    value.clear();
                    sign = false;
                    break;
                case internal::LIMBS_LT:
                    internal::limbs_subtract(bi.value, value, value);
                    sign = bi.sign;
                    break;
            }
        }

        return *this;
    }

    template <typename Container1, typename Container2>
    friend BigInt<T, Container1> 
    operator+ (BigInt<T, Container1> const &bi1, 
               BigInt<T, Container2> const &bi2)
    {
        auto tmp = bi1;
        tmp += bi2;
        return tmp;
    }


    template <typename Container2>
    BigInt& operator-= (BigInt<T, Container2> const &bi)
    {
        if (sign == bi.sign) {
            switch(internal::limbs_cmp(value, bi.value)) {
                case internal::LIMBS_GT:
                    internal::limbs_subtract(value, bi.value, value);
                    break;
                case internal::LIMBS_EQ:
                    value.clear();
                    sign = false;
                    break;
                case internal::LIMBS_LT:
                    internal::limbs_subtract(bi.value, value, value);
                    sign = !bi.sign;
                    break;
            }
        } else {
            internal::limbs_add(value, bi.value, value);
        }

        return *this;
    }

    template <typename Container1, typename Container2>
    friend BigInt<T, Container1> 
    operator- (BigInt<T, Container1> const &bi1, 
               BigInt<T, Container2> const &bi2)
    {
        auto tmp = bi1;
        tmp -= bi2;
        return tmp;
    }


    template <typename Container2>
    BigInt& operator*= (BigInt<T, Container2> const &bi) {
        std::vector<Int> tmp;
        internal::limbs_multiply(value, bi.value, tmp);
        value.assign(BEGIN_END(tmp));
        sign ^= bi.sign;

        return *this;
    }

    template <typename Container1, typename Container2>
    friend BigInt<T, Container1> 
    operator* (BigInt<T, Container1> const &bi1, 
               BigInt<T, Container2> const &bi2)
    {
        auto tmp = bi1;
        tmp *= bi2;
        return tmp;
    }

    
    // -----------------------------------------------------------------------
    friend std::ostream& operator<< (std::ostream& os, BigInt const &bi)
    {
        if (bi.sign)
            os << '-';
        os << "0x ";

        return internal::limbs_print(os, BEGIN_END(bi.value));
    }

private:
    bool sign;
    Container value;

};

} // namespace bigint

#endif // __BIGINT_HH__
