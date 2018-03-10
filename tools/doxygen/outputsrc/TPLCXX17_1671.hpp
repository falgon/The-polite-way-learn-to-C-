/**
* @addtogroup 167	16.7	第16章/16.7	アルゴリズムと計算複雑性理論
* @brief 16.7.1	で利用されたサンプルコードです
*/
/*@{*/
//! TPLCXX17 namespace
namespace TPLCXX17 {
//! chapter 16.7.1 namespace
namespace chap16_7_1 {
//! version 1 namespace
namespace v1 {
/**
 * @brief 1 から @a n までの総和を求めます
 * @param n unsigned int の整数値
 * @return 1 から @a n までの総和を返します
 * @code
 * void sum_sample()
 * {
 *      [[maybe_unused]] int r = TPLCXX17::chap16_7_1::v1::sum(10);
 * }
 * @endcode
*/
unsigned int sum(unsigned int n)
{
    int k = 0;
    for (unsigned int i = 1; i <= n; ++i) k += i;
    return k;
}

} // namespace v1
} // namespace chap16_7_1
} // namespace TPLCXX17
namespace TPLCXX17 {
namespace chap16_7_1 {
//! version 2 namespace
namespace v2 {
/**
 * @brief 1 から @a n までの総和を総和の公式を利用して求めます
 * @param n unsigned int 型の整数値
 * @return 1 から @a n までの総和を返します
 * @code
 * void sum_sample()
 * {
 *      [[maybe_unused]] int r = TPLCXX17::chap16_7_1::v2::sum(10);
 * }
 * @endcode
*/
unsigned int sum(unsigned int n)
{
    return n * (n + 1) / 2;
}

} // namespace v2
} // namespace chap16_7_1
} // namespace TPLCXX17
#include <cmath>
namespace TPLCXX17 {
namespace chap16_7_1 {
namespace v1 {
/**
 * @brief @a n より小さい素数値を総当たりによって @a oiter に全て出力します
 * @param n unsigned int 型の整数値
 * @param oiter 出力イテレータ
 * @return 出力イテレータを返します
 * @code
 * #include <vector>
 * void primes_sample()
 * {
 *      std::vector<unsigned int> res;
 *      TPLCXX17::chap16_7_1::v1::primes(42, std::back_inserter(res));
 * }
 * @endcode
*/
template <class OutputIterator>
OutputIterator primes(unsigned int n, OutputIterator oiter)
{
    if (n > 2) *oiter++ = 2;
    for (unsigned int i = 3; i <= n; i += 2) { // (1)
        bool k = true;
        for (unsigned int j = 3, sqrti = std::sqrt(i); j <= sqrti; j += 2) { // (2)
            if (!(i % j)) { // (3)
                k = false;
                break;
            }
        }
        if (k) *oiter++ = i;
    }
    return oiter;
}

} // namespace v1
} // namespace chap16_7_1
} // namespace TPLCXX17
#include <cmath>
#include <vector>
namespace TPLCXX17 {
namespace chap16_7_1 {
namespace v2 {
/**
 * @brief @a n より小さい素数値をエラトステネスの篩によって @a oiter に全て出力します
 * @param n unsigned int 型の整数値
 * @param oiter 出力イテレータ
 * @return 出力イテレータを返します
 * @code
 * #include <vector>
 * void primes_sample()
 * {
 *      std::vector<unsigned int> res;
 *      TPLCXX17::chap16_7_1::v2::primes(42, std::back_inserter(res));
 * }
 * @endcode
*/
template <class OutputIterator>
OutputIterator primes(unsigned int n, OutputIterator oiter)
{
    std::vector<bool> prime(n, 1);
    prime[0].flip();

    for (unsigned int i = 0, end = std::sqrt(n), k = 0; i < end; ++i) {
        if (prime[i]) {
            k = i + 1;
            for (unsigned int j = k * 2 - 1; j < n; j += k) prime[j] = 0;
        }
    }

    for (unsigned int i = 0; i < n; ++i) {
        if (prime[i]) *oiter++ = i + 1;
    }
    return oiter;
}

} // namespace v2
} // namespace chap16_7_1
} // namespace TPLCXX17
#include <algorithm>
#include <functional>
namespace TPLCXX17 {
namespace chap16_7_1 {
namespace v1 {
/**
 * @brief 範囲を選択ソートします
 * @param first 範囲の最初のイテレータ
 * @param last 範囲の最後 + 1 のイテレータ
 * @param comp bool 値へ文脈変換可能な比較関数オブジェクト
 * @return なし
 * @code
 * #include <numeric>
 * #include <random>
 * 
 * void selection_sort_sample()
 * {
 *      std::vector<int> v(10);
 *      std::iota(std::begin(v), std::end(v), mt);
 *      std::random_device seed;
 *      std::mt19937 mt(seed());
 *      std::shuffle(std::begin(v), std::end(v), mt);
 * 
 *      TPLCXX17::chap16_7_1::v1::selection_sort(std::begin(v), std::end(v)); // less than
 *      TPLCXX17::chap16_7_1::v1::selection_sort(std::begin(v), std::end(v), std::greater<>()); // greater than
 * }
 * @endcode
*/
template <class ForwardIterator, class Compare>
void selection_sort(ForwardIterator first, ForwardIterator last, Compare comp)
{
    for (ForwardIterator iter = first; iter != last; ++iter) {
        std::iter_swap(iter, std::min_element(iter, last, comp));
    }
}
#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <class ForwardIterator>
void selection_sort(ForwardIterator first, ForwardIterator last)
{
    selection_sort(first, last, std::less<>());
}
#endif
} // namespace v1
} // namespace chap16_7_1
} // namespace TPLCXX17
#include <algorithm>
#include <functional>
namespace TPLCXX17 {
namespace chap16_7_1 {
namespace v1 {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <class ForwardIterator, class Compare>
void iter_swap_if(ForwardIterator x, ForwardIterator y, Compare comp)
{
    if (comp(*x, *y)) std::iter_swap(x, y);
}
#endif

/**
 * @brief 範囲をバブルソートします
 * @param first 範囲の最初のイテレータ
 * @param last 範囲の最後 + 1 のイテレータ
 * @param comp bool 値へ文脈変換可能な比較関数オブジェクト
 * @return なし
 * @code
 * #include <numeric>
 * #include <random>
 * 
 * void bubble_sort_sample()
 * {
 *      std::vector<int> v(10);
 *      std::iota(std::begin(v), std::end(v), mt);
 *      std::random_device seed;
 *      std::mt19937 mt(seed());
 *      std::shuffle(std::begin(v), std::end(v), mt);
 * 
 *      TPLCXX17::chap16_7_1::v1::bubble_sort(std::begin(v), std::end(v)); // less than
 *      TPLCXX17::chap16_7_1::v1::bubble_sort(std::begin(v), std::end(v), std::greater<>()); // greater than
 * }
 * @endcode
*/
template <class ForwardIterator, class Compare>
void bubble_sort(ForwardIterator first, ForwardIterator last, Compare comp)
{
    for (ForwardIterator i = first; i != last; ++i) {
        for (ForwardIterator j = first; j < i; ++j) {
            iter_swap_if(i, j, comp);
        }
    }
}
#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <class ForwardIterator>
void bubble_sort(ForwardIterator first, ForwardIterator last)
{
    bubble_sort(first, last, std::less<>());
}
#endif

} // namespace v1
} // namespace chap16_7_1
} // namespace TPLCXX17
#include <algorithm>
#include <functional>
#include <iterator>
namespace TPLCXX17 {
namespace chap16_7_1 {
namespace v1 {

/**
 * @class search_insert
 * @brief 線形探索による探索と挿入を行う関数オブジェクト
*/
struct search_insert {
    /**
     * @param first 範囲の最初の BidirectionalIterator コンセプトを満たすイテレータ
     * @param iter イテレート中における現在のイテレータ
     * @param comp bool 値へ文脈変換可能な比較関数オブジェクト
     * @return なし
    */
    template <class BidirectionalIterator, class Compare>
    void operator()(BidirectionalIterator first, BidirectionalIterator iter, Compare comp)
    {
        typedef typename std::iterator_traits<BidirectionalIterator>::value_type value_type;
        const value_type x = *iter;
        do {
            *iter = *std::next(iter, -1);
            std::advance(iter, -1);
        } while (!comp(iter, first) && !comp(*std::next(iter, -1), x));
        *iter = x;
    }
};

/**
 * @brief 範囲を挿入ソートします
 * @param first 範囲の最初のイテレータ
 * @param last 範囲の最後 + 1 のイテレータ
 * @param comp bool 値へ文脈変換可能な比較関数オブジェクト
 * @param search_inserter 挿入位置の検索を行い、挿入を実行する関数オブジェクト
 * @return なし
 * @code
 * #include <numeric>
 * #include <random>
 * 
 * void insertion_sort_sample()
 * {
 *      std::vector<int> v(10);
 *      std::iota(std::begin(v), std::end(v), mt);
 *      std::random_device seed;
 *      std::mt19937 mt(seed());
 *      std::shuffle(std::begin(v), std::end(v), mt);
 * 
 *      TPLCXX17::chap16_7_1::v1::insertion_sort(std::begin(v), std::end(v)); // less than
 *      TPLCXX17::chap16_7_1::v1::insertion_sort(std::begin(v), std::end(v), std::greater<>()); // greater than
 * }
 * @endcode
*/
template <class BidirectionalIterator, class Compare, class SearchInserter>
void insertion_sort(BidirectionalIterator first, BidirectionalIterator last, Compare comp, SearchInserter search_inserter)
{
    for (BidirectionalIterator i = std::next(first, 1); i != last; ++i) {
        if (!comp(*std::next(i, -1), *i)) { // (1)
            search_inserter(first, i, comp);
        }
   }
}
#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <class BidirectionalIterator, class Compare>
void insertion_sort(BidirectionalIterator first, BidirectionalIterator last, Compare comp)
{
    insertion_sort(first, last, comp, search_insert());
}

template <class BidirectionalIterator>
insertion_sort(BidirectionalIterator first, BidirectionalIterator last)
{
    insertion_sort(first, last, std::less<>(), search_insert());
}
#endif

} // namespace v1
} // namespace chap16_7_1
} // namespace TPLCXX17
#include <algorithm>
namespace TPLCXX17 {
namespace chap16_7_1 {
namespace v1 {
/**
 * @brief 範囲をマージソートします
 * @param first 範囲の最初のイテレータ
 * @param last 範囲の最後 + 1 のイテレータ
 * @param comp bool 値へ文脈変換可能な比較関数オブジェクト
 * @return なし
 * @code
 * #include <numeric>
 * #include <random>
 * 
 * void merge_sort_sample()
 * {
 *      std::vector<int> v(10);
 *      std::iota(std::begin(v), std::end(v), mt);
 *      std::random_device seed;
 *      std::mt19937 mt(seed());
 *      std::shuffle(std::begin(v), std::end(v), mt);
 * 
 *      TPLCXX17::chap16_7_1::v1::merge_sort(std::begin(v), std::end(v)); // less than
 *      TPLCXX17::chap16_7_1::v1::merge_sort(std::begin(v), std::end(v), std::greater<>()); // greater than
 * }
 * @endcode
*/
template <class RandomAccessIterator, class Compare>
void merge_sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    if (last - first > 1) {
        RandomAccessIterator middle = first + (last - first) / 2;
        merge_sort(first, middle, comp);
        merge_sort(middle, last, comp);
        std::inplace_merge(first, middle, last, comp);
    }
}
#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <class RandomAccessIterator>
void merge_sort(RandomAccessIterator first, RandomAccessIterator last)
{
    merge_sort(first, last, std::less<>());
}
#endif
} // namespace v1
} // namespace chap16_7_1
} // namespace TPLCXX17
#include <algorithm>
#include <functional>
namespace TPLCXX17 {
namespace chap16_7_1 {
namespace v1 {
/**
 * @brief 範囲の先頭をピボットとしてクイックソートを行います
 * @param first 範囲の最初のイテレータ
 * @param last 範囲の最後 + 1 のイテレータ
 * @param comp bool 値へ文脈変換可能な比較関数オブジェクト
 * @return なし
 * @code
 * #include <numeric>
 * #include <random>
 * 
 * void quick_sort_sample()
 * {
 *      std::vector<int> v(10);
 *      std::iota(std::begin(v), std::end(v), mt);
 *      std::random_device seed;
 *      std::mt19937 mt(seed());
 *      std::shuffle(std::begin(v), std::end(v), mt);
 * 
 *      TPLCXX17::chap16_7_1::v1::quick_sort(std::begin(v), std::end(v)); // less than
 *      TPLCXX17::chap16_7_1::v1::quick_sort(std::begin(v), std::end(v), std::greater<>()); // greater than
 * }
 * @endcode
*/
template <class BidirectionalIterator, class Compare>
void quick_sort(BidirectionalIterator first, BidirectionalIterator last, Compare comp)
{
    if (first == last) return;
    
    BidirectionalIterator l = first, r = std::next(last, -1);
    while (comp(l, r)) {
        for (; comp(*l, *first) && comp(l, r); ++l);
        for (; comp(*first, *r); --r);
        std::iter_swap(l, r);
    }
    std::iter_swap(first, l);
    quick_sort(first, l, comp);
    quick_sort(++l, last, comp);
}
#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <class BidirectionalIterator>
void quick_sort(BidirectionalIterator first, BidirectionalIterator last)
{
    quick_sort(first, last, std::less<>());
}
#endif

} // namespace v1

#ifndef DOXYGEN_SHOULD_SKIP_THIS
namespace v2 {

// std::partition による実装
template <class BidirectionalIterator, class Compare>
void quick_sort(BidirectionalIterator first, BidirectionalIterator last, Compare comp)
{
    if (first == last) return;

    // std::partition() の時間計算量は O(n) です。
    BidirectionalIterator split = std::partition(std::next(first, 1), std::bind(comp, std::placeholders::_1, *first)); 
    std::iter_swap(first, std::next(split, -1));
    quick_sort(first, std::next(split, -1), comp);
    quick_sort(split, last, comp);
}
template <class BidirectionalIterator>
void quick_sort(BidirectionalIterator first, BidirectionalIterator last)
{
    quick_sort(first, last, std::less<>());
}

} //namespace v2
#endif
} // namespace chap16_7_1
} // namespace TPLCXX17
namespace TPLCXX17 {
namespace chap16_7_1 {
namespace v2 {
/**
 * @brief median-of-three を得ます
 * @param x @a operator< によって比較可能な値
 * @param y @a operator< によって比較可能な値
 * @param z @a operator< によって比較可能な値
 * @return median-of-three を返します
 * @code
 * void med3_sample()
 * {
 *      [[maybe_unused]] auto r = TPLCXX17::chap16_7_1::v2::med3(4, 2, 3); // 3
 * }
 * @endcode
 */
template <class T>
const T& med3(const T& x, const T& y, const T& z) // median-of-three を得る
{
    return std::max(std::min(x, y), std::min(std::max(x, y), z)); 
}

/**
 * @brief median-of-three によってピボットを選択し、クイックソートを行います
 * @param first 範囲の最初のイテレータ
 * @param last 範囲の最後 + 1 のイテレータ
 * @param comp bool 値へ文脈変換可能な比較関数オブジェクト
 * @return なし
 * @code
 * #include <numeric>
 * #include <random>
 * 
 * void quick_sort_sample()
 * {
 *      std::vector<int> v(10);
 *      std::iota(std::begin(v), std::end(v), mt);
 *      std::random_device seed;
 *      std::mt19937 mt(seed());
 *      std::shuffle(std::begin(v), std::end(v), mt);
 * 
 *      TPLCXX17::chap16_7_1::v2::quick_sort(std::begin(v), std::end(v)); // less than
 *      TPLCXX17::chap16_7_1::v2::quick_sort(std::begin(v), std::end(v), std::greater<>()); // greater than
 * }
 * @endcode
 */
template <class BidirectionalIterator, class Compare>
void quick_sort(BidirectionalIterator first, BidirectionalIterator last, Compare comp)
{
    if (first == last) return;
    
    typedef typename std::iterator_traits<BidirectionalIterator>::value_type value_type;
    BidirectionalIterator miter = std::next(first, std::distance(first, last) / 2);
    const value_type pivot = med3(*first, *miter, *std::next(last, -1));
    BidirectionalIterator upper = std::partition(first, last, std::bind(comp, std::placeholders::_1, pivot));
    BidirectionalIterator lower = std::partition(upper, last, std::bind(std::not_fn(comp), pivot, std::placeholders::_1));

    quick_sort(first, upper, comp);
    quick_sort(lower, last, comp);
}

// ... v1::quick_sort と同じ comp なしのバージョンのオーバーロード(略)

} // namespace v2
} // namespace chap16_7_1
} // namespace TPLCXX17
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#include <cstdlib>

template <class T>
struct less {
    auto operator()() const noexcept
    {
        return [](const void* a, const void* b) -> int {
            const auto x = *static_cast<const T*>(a);
            const auto y = *static_cast<const T*>(b);
            return x < y ? -1 : x > y  ? 1 : 0;
        };
    }
};

typedef decltype(v)::value_type value_type;
std::qsort(std::data(v), v.size(), sizeof(value_type), less<value_type>()());
#endif
#include <iterator>
namespace TPLCXX17 {
namespace chap16_7_1 {
namespace v1 {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <class ForwardIterator, class T, class Compare>
ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& val, Compare comp)
{
    typedef typename std::iterator_traits<ForwardIterator>::difference_type diff_type;
    
    for (diff_type len = std::distance(first, last); len > 0;) {
        diff_type half = len >> 1; // == len / 2
        ForwardIterator mid = first;
        std::advance(mid, half);

        if (comp(*mid, val)) {
            first = mid;
            ++first;
            len -= half - 1;
        } else {
            len = half;
        }
    }
    return first;
}
#endif

/**
 * @brief 指定された要素以上の値が現れる最初のイテレータを取得します。この関数は std::lower_bound と同等です
 * @param first 範囲の最初のイテレータ
 * @param last 範囲の最後 + 1 のイテレータ
 * @param val 検索対象の値
 * @return @p [first, last] 内のイテレータが @a val 以上の要素のうち最初のものを指すイテレータを返します。@a val 以上の要素がない場合 @a last を返します
 * @code
 * #include <vector>
 * #include <numeric>
 *
 * void lower_bound_sample()
 * {
 *      std::vector<int> v(10);
 *      std::iota(std::begin(v), std::end(v), 0);
 *      auto iter = TPLCXX17::chap16_7_1::v1::lower_bound(std::begin(v), std::end(v), 4);
 * }
 * @endcode
*/
template <class ForwardIterator, class T>
ForwardIterator lower_bound(ForwardIterator first, ForwardIterator last, const T& val)
{
    return v1::lower_bound(first, last, val, std::less<T>());
}

/**
 * @brief 二分探索によって要素が範囲内に存在するかどうか @a comp を利用して判定します。この関数は std::binary_search と同等です
 * @param first 範囲の最初のイテレータ
 * @param last 範囲の最後のイテレータ
 * @param val 検索対象の値
 * @param comp bool 値へ文脈変換可能な比較関数オブジェクト
 * @return @a val と同等の値が @p [first, last] にある場合は true 、そうでない場合は false を返します
 * @code
 * #include <vector>
 * #include <numeric>
 *
 * void binary_search_sample()
 * {
 *      std::vector<int> v(10);
 *      std::iota(std::begin(v), std::end(v), 0);
 *      bool res = TPLCXX17::chap16_7_1::v1::binary_search(std::begin(v), std::end(v), 4);
 * }
 * @endcode
*/
template <class ForwardIterator, class T, class Compare>
bool binary_search(ForwardIterator first, ForwardIterator last, const T& val, Compare comp)
{
    ForwardIterator iter = v1::lower_bound(first, last, val, comp);
    return iter != last && !comp(val, *iter); // == 見つかった && それは val よりも大きくないまたは小さくない
}
#ifndef DOXYGEN_SHOULD_SKIP_THIS
template <class ForwardIterator, class T>
bool binary_search(ForwardIterator first, ForwardIterator last, const T& val)
{
    return TPLCXX17::chap16_7_1::v1::binary_search(first, last, val, std::less<T>());
}
#endif

} // namespace v1
} // namespace chap16_7_1
} // namespace TPLCXX17
#include <algorithm>
namespace TPLCXX17 {
namespace chap16_7_1 {
namespace v2 {

/**
 * @class search_insert
 * @brief 二分探索による探索と挿入を行う関数オブジェクト。@a v1::insertion_sort に対して利用します。
 * @code
 * #include <vector>
 * #include <numeric>
 * #include <random>
 * 
 * void insertion_sort_sample()
 * {
 *      std::vector<int> v(10);
 *      std::iota(std::begin(v), std::end(v), mt);
 *      std::random_device seed;
 *      std::mt19937 mt(seed());
 *      std::shuffle(std::begin(v), std::end(v), mt);
 * 
 *      TPLCXX17::chap16_7_1::v1::insertion_sort(std::begin(v), std::end(v), std::less<>(), v2::binary_search_insert()); // less than
 *      TPLCXX17::chap16_7_1::v1::insertion_sort(std::begin(v), std::end(v), std::greater<>(), v2::binary_search_insert()); // greater than
 * }
 * @endcode
*/
struct search_insert {
    /**
     * @param first 範囲の最初の BidirectionalIterator コンセプトを満たすイテレータ
     * @param iter イテレート中における現在のイテレータ
     * @param comp bool 値へ文脈変換可能な比較関数オブジェクト
     * @return なし
    */
    template <class ForwardIterator, class Compare>
    void operator()(ForwardIterator first, ForwardIterator iter, Compare comp)
    {
        std::rotate(std::upper_bound(first, iter, *iter, comp), iter, std::next(iter, 1));
    }
};

} // namespace v2
} // namespace chap16_7_1
} // namespace TPLCXX17
/*@}*/
