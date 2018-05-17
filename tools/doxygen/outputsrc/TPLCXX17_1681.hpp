/**
* @addtogroup 168	16.8	第16章/16.8	floatのしくみ
* @brief 16.8.1	で利用されたサンプルコードです
*/
/*@{*/
#include <iostream>
#include <type_traits>
#include <climits>
#include <cstdint>
#include <cmath>

//! TPLCXX17 namespace
namespace TPLCXX17 {
////! chapter 16.8.1 namespace
namespace chap16_8_1 {
////! version 1 namespace
namespace v1 {

/**
 * @class simply_fixed_point
 * @brief 簡素化された符号なし固定小数点数型のシミュレートクラス
 * @code
 * int main()
 * {
 *     TPLCXX17::chap16_8_1::v1::simply_fixed_point fp(static_cast<std::uint16_t>(42));
 *     std::cout << fp << std::endl; // 42
 *
 *     fp = TPLCXX17::chap16_8_1::v1::simply_fixed_point::convert_fixed_point(1.23);
 *     std::cout << fp << std::endl; // 1.23047
 * }
 * @endcode
 */
class simply_fixed_point {
    // 1 バイトは 8 ビットでなければならない
    static_assert(CHAR_BIT == 8);
public:
    //! 32 ビット符号なし領域を用います
    typedef std::uint32_t value_type;
private:  
    //! fractional part のビットサイズを表します。
    inline static constexpr value_type fractional_size = 8;
    
    /**
     * @class is_integral
     * @brief コンストラクタで SFINAE を行うためのヘルパーです
     */
    template <class T>
    struct is_integral
        : std::conjunction<std::is_unsigned<T>, std::bool_constant<(sizeof(T) <= (sizeof(value_type) * CHAR_BIT - fractional_size) / CHAR_BIT)>> {};
public:
    /**
     * @brief デフォルトコンストラクタ
     */
    constexpr explicit simply_fixed_point() = default;

    /**
     * @brief Number part コンストラクタ
     * @param x is_integral に沿うデータ型 T。SFINAE で失敗しなかった場合、それが問答無用で 24 ビット以下の整数値であるとみなします。
     */
    template <class T, std::enable_if_t<is_integral<T>::value, std::nullptr_t> = nullptr>
    constexpr explicit simply_fixed_point(T x) : data_(std::move(x))
    {
        data_ <<= fractional_size;
    }

    /**
     * @brief 固定小数点型を受け付けるコンストラクタ
     * @param x is_integral に沿わないデータ型 T。SFINAE で失敗しなかった場合、それが問答無用で固定小数点型の形式にビット列が並んでいるものとみなします。
     */
     template <class T, std::enable_if_t<std::negation<is_integral<T>>::value, std::nullptr_t> = nullptr>
     constexpr explicit simply_fixed_point(T x) : data_(std::move(x)) {}

     /**
      * @brief Number part 代入演算子
      * @param x is_integral に沿うデータ型 T。SFINAE で失敗しなかった場合、それが問答無用で 24 ビット以下の整数値であるとみなします。
      */
     template <class T>
     constexpr std::enable_if_t<is_integral<T>::value, simply_fixed_point&> 
     operator=(T x)
     {
         data_ = std::move(x);
         data_ <<= fractional_size;
         return *this;
     }

     /**
      * @brief 固定小数点型を受け付けるコンストラクタ
      * @param x is_integral に沿わないデータ型 T。SFINAE で失敗しなかった場合、それが問答無用で固定小数点型の形式にビット列が並んでいるものとみなします。
      */
     template <class T>
     constexpr std::enable_if_t<std::negation<is_integral<T>>::value, simply_fixed_point&> 
     operator=(T x)
     {
         data_ = std::move(x);
         return *this;
     }

     /**
      * @brief 浮動小数点数型から固定小数点型の値に変換するヘルパ関数
      * @param x 浮動小数点数型の値
      * @return @a x を固定小数点型に変換した値
      */
     template <typename FloatingPoint>
     static constexpr std::enable_if_t<std::is_floating_point<FloatingPoint>::value, value_type> 
     convert_fixed_point(FloatingPoint x)
     {
         // 2 の fractional_size 乗かけた値
         return static_cast<value_type>(std::round(x * std::pow(2, fractional_size)));
     }
private:
     /**
      * @brief 標準出力ライブラリライクなシフト演算のオーバーロード
      * @param os std::ostream のオブジェクト
      * @param this_ simply_fixed_point のオブジェクト
      * @return @a os
      */
     friend std::ostream& operator<<(std::ostream& os, const simply_fixed_point& this_)
     {
         return os << this_.data_ / std::pow(2, fractional_size);
     }

     //! Number part とfractional part にわけた固定小数点の領域として使います
     value_type data_ = 0;
};

} // namespace v1
} // namespace chap16_8_1
} // namespace TPLCXX17
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include <iostream>
#include <bitset>
#include <cstring>
#include <climits>
#include <limits>

int main()
{
    typedef float floating_point;
    static_assert(std::numeric_limits<floating_point>::is_iec559);
    
    floating_point a = 25.625f;
    std::uint32_t dst;
    std::memcpy(&dst, &a, sizeof dst);
    std::cout << std::bitset<sizeof(std::uint32_t) * CHAR_BIT>(dst) << std::endl;
}
#endif
#ifndef DOXYGEN_SHOULD_SKIP_THIS

float a = 25.625f;
std::cout << std::bitset<sizeof(float) * CHAR_BIT>(*reinterpret_cast<unsigned long*>(&a)) << std::endl; // strict aliasing ルール違反

#endif
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <limits>

constexpr std::float_denorm_style b = std::numeric_limits<float>::has_denorm;
if constexpr (a == std::denorm_indeterminate) {
    // ...
} else if constexpr (a == std::denorm_absent) {
    // ...
} else if constexpr (a == std::denorm_present) {
    // ...
}

#endif
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include <iostream>
#include <bitset>
#include <limits>
#include <cstring>
#include <climits>
#include <cmath>

struct print_bit {
    constexpr explicit print_bit(float f) : data_(std::move(f)) {}
private:
    float data_;

    friend std::ostream& operator<<(std::ostream& os, const print_bit& this_)
    {
        static_assert(std::numeric_limits<float>::is_iec559);
        std::uint32_t ui;
        std::memcpy(&ui, &this_.data_, sizeof ui);
        return os << std::bitset<sizeof ui * CHAR_BIT>(ui);
    }
};

std::string round_style()
{
    if constexpr (constexpr std::float_round_style fs = std::numeric_limits<float>::round_style; fs == std::round_toward_zero) {
        return "toward zero";
    } else if constexpr (fs == std::round_to_nearest) {
        return "to nearest";
    } else if constexpr (fs == std::round_toward_infinity) {
        return "toward infinity";
    } else if constexpr (fs == std::round_toward_neg_infinity) {
        return "toward negative infinity";
    } else {
        return "indeterminate";
    }
}

int main()
{
    // 丸め方式を出力
    std::cout << round_style() << std::endl; // 筆者の環境では to nearest

    const float f1 = std::pow(2, 23); // 2 の 23 乗
    // 1 は正常に足される
    std::cout << print_bit(f1 + 1.f) << std::endl;  // 01001010100000000000000000000001
    // 丸め方式によって実際の値は変わり得るものの正常には足されない
    std::cout << print_bit(f1 + 0.5f) << std::endl; // 01001011000000000000000000000000

    const float f2 = std::pow(2, 24); // 2 の 24 乗
    // 単に出力
    std::cout << print_bit(f2) << std::endl; // 01001011100000000000000000000000
    // 丸め方式によって実際の値は変わり得るものの正常には足されない
    std::cout << print_bit(f2 + 1) << std::endl; // 01001011100000000000000000000000
}
#endif
#ifndef DOXYGEN_SHOULD_SKIP_THIS

#include <cfenv>
#   pragma STDC FENV_ACCESS ON
std::fesetround(FE_DOWNWARD); // 負の無限方向への丸めに設定する
std::fesetround(FE_UPWARD); // 正の無限方向への丸めに設定する
std::fesetround(FE_TOWARDZERO); // 0 方向への丸めに設定する
std::fesetround(FE_TONEAREST); // 最近接偶数丸めに設定する

#endif
#ifndef DOXYGEN_SHOULD_SKIP_THIS

float f = 0.f;
for (int i = 0; i < 10000; ++i) f += 0.01f;

std::cout << std::fixed << std::setprecision(std::numeric_limits<float>::max_digits10) << f << std::endl;

#endif
#ifndef DOXYGEN_SHOULD_SKIP_THIS

float f = 0.f;
for (int i = 0; i < 100; ++i) {
    float g = 0.f;
    for (int j = 0; j < 100; ++j) g += 0.01f;
    f += g;
}

#endif
#ifndef DOXYGEN_SHOULD_SKIP_THIS

float f = 0.f, r = 0.f, t;
for (int i = 0; i < 10000; ++i) {
    r += 0.01f; // (1) (3) で加えられることができなかった値と、次に加える値の和を取る。
    t = f;      // (2) 現在値を適用前にとっておく。
    f += r;     // (3) (1) の値を加える。このとき、r が誤差幅よりも小さい値であると加えることができず、無視される。
    r -= f - t; // (4) r から (3) で加えることができた分を取り除く。
}

#endif
/*@}*/
