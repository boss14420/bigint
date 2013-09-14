/*
 * =====================================================================================
 *
 *       Filename:  test.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/11/2013 09:44:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#include <cstdint>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <random>
#include <chrono>
#include "bigint.hpp"

#define PRINT_LIMBS(L, MSG) std::cout << MSG; \
                            bigint::internal::limbs_print(std::cout, BEGIN_END(L)); \
                            std::cout << '\n'

using namespace std::chrono;
typedef duration<double,std::ratio<1>> real_seconds;

#define     PROCESS_TIME(COMMAND, STR)  do {                                            \
                                            auto b = high_resolution_clock::now();      \
                                            COMMAND;                                    \
                                            auto time = duration_cast<real_seconds>(    \
                                                high_resolution_clock::now()-b).count();\
                                            std::cout << STR << " : " << time << "s\n"; \
                                        } while(0)

int main(int argc, char *argv[]) 
{
#if defined(__GNUC__) && defined(USE_64BIT_LIMB)
    #ifdef __LP64__
        typedef std::uint64_t Int;
    #else
        #error "Not support __uint128_t"
    #endif
#else
    typedef std::uint32_t Int;
#endif

    typedef std::vector<Int> Container;

    std::size_t l1 = std::atoi(argv[1]);
    std::size_t l2 = std::atoi(argv[2]);

    Container v1 (l1);
    Container v2 (l2);

//    std::random_device rnd;
    std::mt19937 rnd(0);
    std::uniform_int_distribution<Int> dis;

    std::generate(BEGIN_END(v1), std::bind(dis, std::ref(rnd)));
    std::generate(BEGIN_END(v2), std::bind(dis, std::ref(rnd)));

    bigint::BigInt<Int, Container> bi1 { false, v1 };
    if (std::max(l1, l2) < 10)
        std::cout << "bi1 = " << bi1 << '\n';

    bigint::BigInt<Int, Container> bi2 { false, v2 };
    if (std::max(l1, l2) < 10)
        std::cout << "bi2 = " << bi2 << '\n';

    decltype(bi1) bi3;
    PROCESS_TIME(bi3 = bi1 * bi2, "bi3 = bi1 * bi2");

    if (std::max(l1, l2) < 10)
        std::cout << "bi3 = bi1 * bi2 = " << bi3 << '\n';


    // ------- test low level --------------------------------------
//    std::vector<std::uint32_t> v1 { 0xff11ff11U, 0x11U };
//    std::deque<std::uint32_t> v2 { 0xffffffffU, 0x1U };
//    std::vector<std::uint32_t> v4 { 0x0U, 0x2U };
//    std::deque<std::uint32_t> v5 { };
//
//    std::deque<std::uint32_t> res;
//
//    PRINT_LIMBS(v1, "v1 = ");
//    PRINT_LIMBS(v2, "v2 = ");
//    PRINT_LIMBS(v4, "v4 = ");
//    PRINT_LIMBS(v5, "v5 = ");
//
//    bigint::internal::limbs_add(v1, v2, res);
//    PRINT_LIMBS(res, "res = v1 + v2 = ");
//
//    std::cout << "cmp (v1, v2) = " << bigint::internal::limbs_cmp(v1, v2) << '\n';
//
//    bigint::internal::limbs_subtract (v1, v2, res);
//    PRINT_LIMBS(res, "res = v1 - v2 = ");
//
//    bigint::internal::limbs_subtract (v4, v2, res);
//    PRINT_LIMBS(res, "res = v4 - v2 = ");
//
//    bigint::internal::limbs_subtract (v2, v5, res);
//    PRINT_LIMBS(res, "res = v2 - v5 = ");
//
//    bigint::internal::limbs_subtract (v2, v1, res);
//    PRINT_LIMBS(res, "res = v2 - v1 = ");
//
//    std::vector<std::uint32_t> a1 = { 0xffffffffU, 0x1U, 0x1f0e78, 0x22fec01, 0x2354 },
//                               a2 = { 0x27eeU, 0x2U, 0x0U, 0xfff, 0xfe123, 0x321 };
//    PRINT_LIMBS(a1, "a1 = ");
//    PRINT_LIMBS(a2, "a2 = ");
//
//
////    std::uint32_t a1[] = { 0xffffffffU, 0x1U, 0x1f0e78, 0x22fec01, 0x2354 },
////                  a2[] = { 0x27eeU, 0x2U, 0x0U, 0xfff, 0xfe123, 0x321 }, 
////                  a3[11] = { 0 };
//    res.assign(a1.size() + a2.size(), 0);
//    bigint::internal::limbs_multiply_dispatch(BEGIN_END_SIZE(a2), BEGIN_END_SIZE(a1), BEGIN_END(res));
//    PRINT_LIMBS(res, "a3 = ");
//    std::cout << "res = v2 * v4 = 0x " << std::hex << a3[10] << ' '
//                                       << std::hex << a3[9] << ' '
//                                       << std::hex << a3[8] << ' '
//                                       << std::hex << a3[7] << ' '
//                                       << std::hex << a3[6] << ' '
//                                       << std::hex << a3[5] << ' '
//                                       << std::hex << a3[4] << ' '
//                                       << std::hex << a3[3] << ' '
//                                       << std::hex << a3[2] << ' '
//                                       << std::hex << a3[1] << ' '
//                                       << std::hex << a3[0] << '\n';

    // -------- test bigint ------------------------------------
//    bigint::BigInt<std::uint32_t> bi (2000000000);
//    decltype(bi) bi2 (600000000);
////    bi <<= 103;
////    bi >>= 37;
//
//    auto bi3 = bi + (bi2 << 2);
//    std::cout << bi << '\n';
//    std::cout << (bi2 << 2) << '\n';
//    std::cout << bi3 << '\n';
//
//    bigint::BigInt<std::uint32_t> bi4 { false, v2 };
//    bigint::BigInt<std::uint32_t, std::vector<std::uint32_t>> bi5 { false, v4 };
//    decltype(bi4) bi6 = bi4;
//
//    std::cout << "bi4 = " << bi4 << "\nbi5 = " << bi5 << '\n';
//    std::cout << "bi4 - bi5 = " << bi4 - bi5 << '\n';
//    std::cout << "bi5 - bi4 = " << bi5 - bi4 << '\n';
////    std::cout << "1 - bi4 = " << 1 - bi4 << '\n';
////    std::cout << "bi5 - 0xffff = " << bi5 - 0xffffU << '\n';
////
//    
//    bigint::BigInt<std::uint32_t, std::vector<std::uint32_t>> bi7 { false, a1 },
//                                                              bi8 { true, a2 };
//    auto bi9 = bi7 * bi8;
//    std::cout << "bi7 = " << bi7 << "\nbi8 = " << bi8 << '\n';
//    std::cout << "bi7 * bi8 = " << bi7 * bi8 << '\n';
//    std::cout << "bi8 * bi7 = " << bi8 * bi7 << '\n';

    return 0;
}
