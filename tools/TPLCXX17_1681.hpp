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
//! chapter 16.8.1 namespace
namespace chap16_8_1 {
//! version 1 namespace
namespace v1 {
/**
 * @class simply_fixed_point
 * @brief シンプルな符号なし固定小数型をシミュレートしたクラス
 * @code
 * int main()
 * {
 *      std::uint16_t v = 42;
 *      simply_fixed_point fp(42_); // 整数 42 で初期化
 *      std::cout << fp << std::endl; // 42
 *
 *      fp.assign_point(simply_fixed_point::point_compute(1.23)); // 浮動小数点数 1.23 を固定小数点数に変換し代入
 *      std::cout << fp << std::endl; // 1.23047
 * }
 * @endcode
 */
class simply_fixed_point {
public:
    //! 32 ビット符号なし整数型を用いる事とした
    typedef std::uint32_t value_type;

    /**
     * @brief デフォルトコンストラクタ
     */
    constexpr explicit simply_fixed_point() = default;
    /**
     * @brief コンストラクタ
     * @param x 24 ビット以下の符号なしデータ型 T。問答無用でそれが 24 ビット以下の数値であるとみなす
     */
    template <class T, std::enable_if_t<std::conjunction_v<std::is_unsigned<T>, std::bool_constant<(sizeof(T) <= 24 / CHAR_BIT)>>, std::nullptr_t> = nullptr>
    constexpr explicit simply_fixed_point(T x) : data_(std::move(x))
    {
        data_ <<= fractional_part; // 8 ビット分開ける
    }
    /**
     * @brief 代入演算子
     * @param x 24 ビット以下の符号なしデータ型 T。問答無用でそれが 24 ビット以下の数値であるとみなす
     */
    template <class T, std::enable_if_t<(sizeof(T) <= 24 / CHAR_BIT), std::nullptr_t> = nullptr>
    constexpr simply_fixed_point& operator=(T x) noexcept
    {
        data_ = std::move(x);
        data_ <<= fractional_part; // 8 ビット分開ける
        return *this;
    }
    /**
     * @brief 小数代入用の関数
     * @param x 固定小数型の値
     * @return なし
     */
    constexpr void assign_point(value_type x) noexcept
    {
        data_ = std::move(x);
    }
    /**
     * @brief double 型から固定小数型の値に変換するヘルパ関数です
     * @param x 浮動小数点数型
     * @return @a x を固定小数型の値に変換した値
     */
    static constexpr value_type point_compute(double x)
    {
        // 2 の fractional_part 乗をかけた値を入れる
        return static_cast<value_type>(std::round(x * std::pow(2, fractional_part)));
    }
private:
    //! 整数部 24 ビット、小数部 8 ビットの固定小数型とみなす
    value_type data_ = 0; 
    
    //! 小数部は 8 ビット
    inline static constexpr value_type fractional_part = 8; 

    /**
     * @brief 標準出力ライブラリライクなシフト演算子のオーバーロード
     * @param os std::ostream のオブジェクト
     * @param this_ simply_fixed_point のオブジェクト
     * @return @a os
     */
    friend std::ostream& operator<<(std::ostream& os, const simply_fixed_point& this_)
    {
        return os << this_.data_ / std::pow(2, fractional_part); 
    }
private:
    // CHAR_BIT は 8 ビットでなければならない。
    static_assert(CHAR_BIT == 8);
};

} // namespace v1
} // namespace chap16_8_1
} // namespace TPLCXX17
/*@}*/
