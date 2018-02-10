# 16.7 アルゴリズムと計算量理論

この項では、アルゴリズムと実装、またその性能を表すのに多様される O 記法、そしてアルゴリズムの複雑性クラスの分類について説明します。

## 16.7.1 アルゴリズムと O 記法

O 記法はある特定のアルゴリズムの計算量を示すことのできる記法です。計算量とは、主に時間計算量と空間計算量を内包した意味合いです。時間計算量があるアルゴリズムにどれだけの計算を行うか、空間計算量があるアルゴリズムが計算にどれだけの記憶領域を利用するかを示します。
例えば、$$ 1 $$ から $$ n $$($$ n $$ は整数とする)までの整数の総和($$ \sum_{ k = 1 }^{ n } k = 1 + 2 + 3 + ... n $$)を求める時、どのように計算を行うと良いでしょうか？
愚直な実装は次のようになるでしょう。
```cpp
unsigned int sum(unsigned int n)
{
    int k = 0;
    for (unsigned int i = 1; i <= n; ++i) k += i;
    return k;
}
```
これは、次のように利用することを想定としています。
```cpp
int r = sum(n);
```
この実装だと、加算を $$ n - 1 $$ 回行いますから、時間計算量を$$ O(N) $$、空間計算量を $$ O(1) $$ というように書きます。これは、ビッグオー記法(O 記法)と言います。O は order で、ここでは次数を意味するものです。
$$ O(N - 1) $$ ではないのかと思われるかもしれませんが、ビッグオー記法では一番規模の大きな計算だけを残し、さらに係数を $$ 1 $$ とするようにして記述する慣習があります。これは、分かりやすさの他に、相対誤差が殆ど無いことに起因します。
ところで、先ほどの有限級数は高校数学の次の公式で求めることができますね。<br>
$$ (n + 1) \dfrac{n}{2} $$ 
少し話は逸れてしまいますが、念のため説明しておきますと、まず $$ S = 1 + 2 + 3 + ... + n $$ というのは当然ながら $$ S = 1 + 2 + 3 + ... + (n - 1) + n $$ です。
総和の式を逆順にして、$$ S = n + (n - 1) + (n - 2) + ... + 2 + 1 $$ とします。次に、$$ S $$ を二倍します。すると $$ 2S = (n + (n - 1) + (n - 2) + ... + 2 + 1) + (1 + 2 + ... + (n - 2) + (n - 1) + n) $$ ですね。置き換えると、$$ 2S = (1 + n) + (2 + n - 1) + ... + (n - 1 + 2) + (n + 1) $$ です。この式はさらに置き換えることが出来、$$ 2S = (n + 1) + (n + 1) + ... + (n + 1) + (n + 1) $$ であり $$ 2S = \underbrace{(n + 1) + (n + 1) + ... + (n + 1) + (n + 1)}_{n} $$ なので、$$ 2S = n(n + 1) $$ となります。つまり、$$ S $$ は $$ n (n + 1) / 2 $$ となりますね。  
こちらの実装は次のようになるでしょう。
```cpp
int sum(int n)
{
    return n * (n + 1) / 2;
}
```
この実装ではご覧の通り、加算、積算、除算で計算し終えます。ですから時間計算量は、$$ O(3) $$ ... ではなく前述した通り $$ O(1) $$ と書きます。空間計算量も $$ O(1) $$ ですね。<br>
もう一つ例を取り上げてみましょう。例えば $$ n $$ 以下の素数を全て見つけ出す時、どのように計算を行うと良いでしょうか？
愚直な実装は次のようになるでしょう($$ n $$ は下記コード`n`に対応しています)。
```cpp
#include <cmath>

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
```
この実装は $$ 1 $$ から`n`まで順々に素数判定を行うアルゴリズムを少し最適化したものです。尚次のように利用する事を想定しています。
```cpp
#include <vector>

std::vector<unsigned int> res;
primes(n, std::back_inserter(res));
```
$$ 2 $$ 以外の素数は奇数ですから、`(1)`の通り奇数のみを調査対象としています。また、任意の値 $$ a $$ が合成数である場合は、必ず $$ \sqrt{a} $$ より小さい素因数を持つはずです。念のため説明しておきましょう。$$ N $$ が合成数とします。よって $$ N = ab $$(但し $$ 1 \lt a \leq b \land a, b \in \mathbb{Z} $$) となります。つまり、$$ N = ab \geq a^{2} $$ ですから $$ \sqrt{N} \geq a $$ となるのです。これを証明 (a) としておきましょう。さて、という事は、$$ a $$ が素数であるかは $$ \sqrt{a} $$ 以下の素数で mod 演算を行えば良い事となります。
さらに、`(1)`で奇数のみを対象としていますから、$$ 3 $$ から開始して、以降奇数のみで mod 演算を行えば良い事となります。これらは`(2)`に対応しています。`(3)`で mod 演算を行い、割り切れた時点で、`k`に`false`をセットしてループを抜けます。
ところで、このアルゴリズムはどのような計算量となるでしょうか。
$$ a $$ が素数かを判定するのに最大 $$ \sqrt{a} $$ 回くらい(前述したように、ビッグオー記法の慣習からある程度の端数は切り落とされる)で、それを`n`回くらい繰り返しますから、このアルゴリズムの時間計算量は $$ O(n\sqrt{n}) $$、空間計算量は $$ O(1) $$ と表されます。<br>
次に少し賢い方法でこの問題に取り組んでみましょう。以下のコードは、エラトステネスの篩と言われるアルゴリズムを実装したものです。
```cpp
#include <cmath>
#include <vector>

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
```
このアルゴリズムは、先ほどの証明 (a) を併用して $$ \sqrt{n} $$ 以下の素数の倍数(自身を除く)を全て篩い落とすことで、素数を得るアルゴリズムです。例えば、$$ n = 30 $$、$$ A $$ を残っている素数集合とした時、篩い落としの様子は次の通りです。
1. $$ A = \{2, 3 ... , 30\} $$
2. 2 の倍数を篩落とし、$$ A=\{2,3,5,7,9,11,13,15,17,19,21,23,25,27,29\} $$
3. 3 の倍数を篩落とし、$$ A=\{2,3,5,7,11,13,17,19,23,25,29\} $$
4. 5 の倍数を篩落とし、$$ A=\{2,3,5,7,11,13,17,19,23,29\} $$
5. 7 は $$ \sqrt{30} $$ よりも大きいため終了

上記コードでは、`prime`にビット単位でその値が素数かどうかのフラグを管理させています。この処理によって、$$ \sqrt{n} $$ 以下の素数の個数分だけこの篩落としが行われます。つまり篩落とすのには $$ n / $$`k` かかる事が言えますから、$$ \sum_{p \lt \sqrt{n}} \dfrac{n}{p} = \dfrac{n}{2} + \dfrac{n}{3} + \dfrac{n}{5} + ... $$ (但し、和は $$ \sqrt{n} $$ 以下の素数全体に対して取る) であると言えます。
素数の逆数和は $$ \log\log \sqrt{n}=\log\log n+\log \dfrac{1}{2} $$ ですので、時間計算量は $$ O(n \log \log n) $$ となります。
また、空間計算量はそれが素数であるか判定するビットフラグが必要なので、$$ O(N) $$ となります。<br>
さて、愚直な方法とエラトステネスの篩で時間計算量を比較してみると、$$ n $$ が十分に大きい場合にエラトステネスの篩の方が圧倒的に少なくなる事が分かります。しかしながら、空間計算量は愚直なアルゴリズムの方が少なくなる事も分かります。このようにビックオー記法を用いると、各アルゴリズムの比較や考察において、どれぐらいの計算時間、記憶領域を必要とするのかを、簡潔に理解する事ができるのです。

最後に、よく利用される計算量オーダーを以下にまとめます。

### $$ O(K^{n}) $$
指数オーダーです。これは、最悪の計算量です。どのような工夫を凝らしてもこの計算量である場合はその問題解決は不可能です。
よって、より少ない計算量から近似値を得るなどしてある程度結果に妥協をしなければなりません。
このような計算量となる有名な問題には、[巡回セールスマン問題](https://ja.wikipedia.org/wiki/%E5%B7%A1%E5%9B%9E%E3%82%BB%E3%83%BC%E3%83%AB%E3%82%B9%E3%83%9E%E3%83%B3%E5%95%8F%E9%A1%8C)などがよく挙げられます。巡回セールスマン問題とは、あるセールスマンがいくつかの都市を1回ずつ訪問して、スタート地点の都市に戻ってくるとき、総移動距離が最も短くなるように都市の巡回順序を決めるという問題です。この問題は、都市数の増加に対して時間計算量が急速に増加するため、一定以上の都市数となると厳密な解を求める事は現実的でなくなります。

### $$ O(N^{2}) $$
二乗オーダーです。例えば、選択ソートの時間計算量は、このオーダーに匹敵します。選択ソートとは、配列された要素から最小値または最大値を探索して、配列最初または最後の要素と入れ替える事でソートを行うアルゴリズムです。これは、比較的遅い部類のアルゴリズムです。
```cpp
#include <algorithm>
#include <functional>

template <class ForwardIterator, class Compare>
ForwardIterator selection_sort(ForwardIterator first, ForwardIterator last, Compare comp)
{
    for (ForwardIterator iter = first; iter != last; ++iter) {
        std::iter_swap(iter, std::min_element(iter, last, comp));
    }
    return first;
}

template <class ForwardIterator>
ForwardIterator selection_sort(ForwardIterator first, ForwardIterator last)
{
    return selection_sort(first, last, std::less<>());
}
```
これは次のように利用する事を想定しています。
```cpp
#include <numeric>
#include <random>

std::vector<int> v(10);
std::iota(std::begin(v), std::end(v), 0);

std::random_device seed;
std::mt19937 mt(seed());
std::shuffle(std::begin(v), std::end(v), mt);

selection_sort(std::begin(v), std::end(v)); // less than
selection_sort(std::begin(v), std::end(v), std::greater<>()); // greater than
```
選択ソートは、1 つのデータの位置を確定するために、$$ (n - 1) $$ 回の比較(上記コードでは、`std::min_element`がその役割を担っています)が必要です。
1 つのデータが確定していくごとに、次の比較対象のデータが 1 つずつ減っていきます。これは、次のように表す事ができます。<br>
$$ (n - 1) + (n - 2) + \cdots + (n - (n - 2)) + (n - (n - 1)) = (n - 1) + (n - 2) + \cdots + 2 + 1 $$
<br>
これは要するに $$ 1 $$ から $$ n - 1 $$ の総和を求めていることと同義です。つまり、本項の初めで述べた総和の公式を利用して、<br>
$$ \displaystyle \sum_{i=1}^{n-1} i = \frac{n(n-1)}{2} = \frac{n^2-n}{2} $$
と表す事ができます。また、値の交換(上記コードでは、`std::iter_swap`がその役割を担っています)は各ループごとに最大 1 回です。つまり全体としては $$ n - 1 $$ 回の時間計算量です。
これらの和から O 記法の慣用を適用すると計算量は $$ O(n^{2}) $$ である事が分かります。
尚、選択ソートは非安定ソートです。安定ソートとは、ソートアルゴリズムのうち同等なデータのソート前の順序と、ソート後の順序が保持されるソートアルゴリズムを言います。選択ソートはそうではありません。<br>
また、バブルソートもこのオーダーによく分類されるアルゴリズムです。バブルソートとは、隣り合う要素の大小比較を行い、その結果に応じて並び替えるソートアルゴリズムです。
```cpp
#include <algorithm>
#include <functional>

template <class ForwardIterator, class Compare>
void iter_swap_if(ForwardIterator x, ForwardIterator y, Compare comp)
{
    if (comp(*x, *y)) std::iter_swap(x, y);
}

template <class ForwardIterator, class Compare>
ForwardIterator bubble_sort(ForwardIterator first, ForwardIterator last, Compare comp)
{
    for (ForwardIterator i = first; i != last; ++i) {
        for (ForwardIterator j = first; j < i; ++j) {
            iter_swap_if(i, j, comp);
        }
    }
    return first;
}

template <class ForwardIterator>
ForwardIterator bubble_sort(ForwardIterator first, ForwardIterator last)
{
    return bubble_sort(first, last, std::less<>());
}
```
利用は先ほどと同様に行えます。
```cpp
// .... (略)

bubble_sort(std::begin(v), std::end(v)); // less than
bubble_sort(std::begin(v), std::end(v), std::greater<>()); // greater than
```
要素の一番目と二番目を比較して順番が逆であれば入れ替え(上記コードでこの比較処理と入れ替えは、`iter_swap_if`が担っています。)、次に二番目と三番目に対して同様にして比較、入れ替えを行い、次に三番目と... というように最後まで実行すると、最後の値が最小または最大となり、その値の位置が確定します。そして再度確定した値以外で、全ての値の位置が確定するまで同様の処理を繰り返します。至極単純ですが、これがバブルソートというアルゴリズムです。<br>
尚前述した通り、バブルソートはよくこの計算量オーダー(時間計算量)のアルゴリズムとして分類されますが、前述した選択ソートよりも低速です。比較回数はバブルソート、選択ソート共に同じですが、交換回数は選択ソートの方が少ないためです。また、選択ソートは非安定ソートであることに対してバブルソートは安定ソートです。<br>
他にも、挿入ソートと言われるソートアルゴリズムもこのオーダーに分類されます。挿入ソートは、ソート済みの要素列に対して新たな要素を適切な位置へ挿入していく事でソートを行うソートアルゴリズムです。
```cpp
#include <algorithm>
#include <functional>
#include <iterator>

struct linear_search_insert {
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

template <class BidirectionalIterator, class Compare, class SearchInserter>
ForwardIterator insertion_sort(BidirectionalIterator first, BidirectionalIterator last, Compare comp, SearchInserter search_inserter)
{
    for (ForwardIterator i = std::next(first, 1); i != last; ++i) {
        if (!comp(*std::next(i, -1), *i)) { // (1)
            search_inserter(first, i, comp);
        }
   }
    return first;
}

template <class BidirectionalIterator, class Compare>
ForwardIterator insertion_sort(BidirectionalIterator first, BidirectionalIterator last, Compare comp)
{
    return insertion_sort(first, last, comp, linear_search_insert());
}

template <class BidirectionalIterator>
ForwardIterator insertion_sort(BidirectionalIterator first, BidirectionalIterator last)
{
    return insertion_sort(first, last, std::less<>(), linear_search_insert());
}
```
利用は先ほどと同様に行えます。
```cpp
// .... (略)

insertion_sort(std::begin(v), std::end(v)); // less than
insertion_sort(std::begin(v), std::end(v), std::greater<>()); // greater than
```
アルゴリズムの流れを次に示します。$$ a_{0}, a_{1}, a_{2}, \cdots, a_{5} = 3, 1, 4, 1, 5, 9 $$ という数列に対して昇順で挿入ソートを行います。

1. $$ a_{0} $$ をソートします。しかし $$ a_{0} $$ よりも前の値はないのでこれをソート済みとします。$$ a_{0} $$ はソートされました。
2. $$ a_{0} $$ から $$ a_{1} $$ をソートします。$$ a_{0} $$ はソート済みのため、それに対して挿入します。このとき、$$ a_{1} $$ は 1 ですから、$$ a_{0} $$ にこれを挿入します。それによって $$ a_{1} $$ に元々の $$ a_{0} $$ の値すなわち $$ 3 $$ がずれこみ、またそれに応じて全ての値が順にずれこみ、数列は $$ 1, 3, 4, 1, 5, 9 $$ になります。$$ a_{0} $$ から $$ a_{1} $$ はソートされました。
3. $$ a_{0} $$ から $$ a_{2} $$ をソートします。$$ a_{0} $$ から $$ a_{1} $$ はソート済みのため、それに対して挿入します。このとき、$$ a_{2} $$ は $$ 4 $$ ですから、ソート済みのどの値よりも大きいです。これは移動する必要がありません。$$ a_{0} $$ から $$ a_{2} $$ はソートされました。
4. $$ a_{0} $$ から $$ a_{3} $$ をソートします。$$ a_{0} $$ から $$ a_{2} $$ はソート済みのため、それに対して挿入します。このとき、$$ a_{3} $$ は $$ 1 $$ ですから、$$ a_{1} $$ にこれを挿入します(後述しますが、挿入位置が $$ a_{0} $$ とならない点が、このソートアルゴリズムの特徴でもあります)。それによって、2. と同様にして全ての値が順にずれ込み、数列は $$ 1, 1, 3, 4, 5, 9 $$ になります。$$ a_{0} $$ から $$ a_{3} $$ はソートされました。
5. $$ a_{0} $$ から $$ a_{4} $$ をソートします。$$ a_{0} $$ から $$ a_{3} $$ はソート済みのため、それに対して挿入します。このとき、$$ a_{4} $$ は $$ 5 $$ ですから、ソート済みのどの値よりも大きいです。これは移動する必要がありません。$$ a_{0} $$ から $$ a_{4} $$ はソートされました。
6. $$ a_{0} $$ から $$ a_{5} $$ をソートします。$$ a_{0} $$ から $$ a_{4} $$ はソート済みのため、それに対して挿入します。このとき、$$ a_{5} $$ は $$ 9 $$ ですから、ソート済みのどの値よりも大きいです。これは移動する必要がありません。$$ a_{0} $$ から $$ a_{5} $$ はソートされました。
7. 末尾に到達したため、ソートが完了しました。

このようにして選択ソートは行われます。処理内容にもある通り、たまたまソート済みであった場合には一切処理をせず次のステップに進む事ができます。上記コードではコメント`(1)`の判定がそれに対応します。また、このソートは安定ソートです。しかしながら、このソートの一連の流れにある挿入位置の決定処理(4. のような処理)において適切にその位置を決定しなければ、安定ソートでなくなってしまいます。<br>
上記コードでは、この挿入位置の決定に、線形探索を利用しており、コード中`linear_search_insert`関数オブジェクトがそれに値します。この場合の挿入ソートの時間計算量は、$$ O(n^{2}) $$ となります。<br>
しかしながら、この検索部分には少し改善の余地が残されています。検索を行う範囲は、上記処理の通り、すでに挿入ソートの過程内でソート済みです。ソート済みの範囲に対しては、二分探索が有効なのです。二分探索については下記 $$ O(logN) $$ で説明します。

### $$ O(N logN)
念のためこの対数について説明しておくと、$$ N $$ を何回 $$ 2 $$ で割れば $$ 1 $$ になるかという意味で。例えば $$ N = 10^{12} $$($$1$$兆) としても約40回程度割れば $$ 1 $$ になります。このように、$$logN$$ の部分は殆ど増えないため、少し遅い $$ O(N) $$ 程度の時間計算量であると言えます。<br>さて、マージソートといわれるソートはこの時間計算量オーダーとなります。マージソートとは、すでにソート済みである複数個の列を 1 列にマージする際に、小さいもの(大きいもの)から順に新しい列に並べる事でソートします。手順としては、

1. データ列を分割する。
2. 分割されたデータ列内に含まれるデータが 1 つである場合それを返す。そうでなければ再帰的に 1 から 3 を適用する
3. 二つのソートされたデータ列(1 つのデータを含む)を小さいもの(大きいもの)から順に新しい列に並べるようにしてマージする

というように行います。
```cpp
#include <algorithm>

template <class RandomAccessIterator, class Compare>
RandomAccessIterator merge_sort(RandomAccessIterator first, RandomAccessIterator last, Compare comp)
{
    if (last - first > 1) {
        RandomAccessIterator middle = first + (last - first) / 2;
        merge_sort(first, middle, comp);
        merge_sort(middle, last, comp);
        std::inplace_merge(first, middle, last, comp);
    }
    return first;
}

template <class RandomAccessIterator>
RandomAccessIterator merge_sort(RandomAccessIterator first, RandomAccessIterator last)
{
    return merge_sort(first, last, std::less<>());
}
```
次のように利用する事を想定しています。
```cpp
// ... (略)

merge_sort(std::begin(v), std::end(v)); // less than
merge_sort(std::begin(v), std::end(v), std::greater<>()); // greater than
```
再帰によってデータ列を細分化して、マージソートの実際のマージ部分を`std::inplace_merge`に任せています。その名の通り、この関数は追加の作業記憶領域を必要としないため、この実装の空間計算量は $$ 0 $$ です。しかし、$$ O(n) $$ の空間計算量を必要とする実装も一般的に見られます。また、データ列に対するそれぞれのマージ操作は並列化が容易である特徴があります。また、安定なソートを実装できます。

また、クイックソートと言われる比較的高速なソートも、最良時間計算量及び平均計算量がこのオーダーです。クイックソートは、1つ適当な値(これをピボットと言います)を選択し、ピボットより小さい値を前方または後方、大きい値を前方または後方に移動し、二分割されたそれぞれのデータをそれぞれにソートします。
```cpp
#include <algorithm>
#include <functional>

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

/*
 * std::partition による実装
 *

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

 *
 *
 */
 
template <class BidirectionalIterator>
void quick_sort(BidirectionalIterator first, BidirectionalIterator last)
{
    quick_sort(first, last, std::less<>());
}
```
次のように利用する事を想定しています。
```cpp
// ... (略)

quick_sort(std::begin(v), std::end(v)); // less than
quick_sort(std::begin(v), std::end(v), std::greater<>()); // greater than
```
クイックソートは、ランダムなデータに対しては有効的ですが、すでにソート済みであったり、並び順が全く逆だったとき(例えば昇順に並び替えようとするデータ列が降順であった場合)、時間計算量が $$ O(n ^ {2}) $$ になってしまいます。クイックソートの高速さは初めのピボット選択が大きな鍵を握っている特性があるため、ピボットの選択方法を工夫する必要があります。上記の実装では、無条件でデータ列の一番先頭をピボットとしており、ピボットの選択方法としてはあまりにも愚直でよろしくありません。ピボットの選択方法には、乱数から選択する、データ列の中間を選択する、データ列から最初、中間、最後の要素を取り出してその中央値を選択するといった方法があります。特に最後のものは median-of-three と言われます。下記コードは、それを利用したクイックソートの実装です。
```cpp
template <class T>
const T& med3(const T& x, const T& y, const T& z) // median-of-three を得る
{
    return std::max(std::min(x, y), std::min(std::max(x, y), z)); 
}

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

// ... 同じオーバーロード(略)
```
median-of-three によって万事解決のように思えますが、これでも尚最悪の時間計算量は $$ O(N^{2}) $$ のままです。どのような場合でなりうるかというと、多くの等しい値の並ぶデータ列に対するソートです。例えば昇順ソートをするとして、入力データが全て同等のであったとします。ソートの過程で、左側のパーティションが空のまま、そして右側のパーティションから 1 つずつ要素が削除されていきます。これが $$ O(N^{2}) $$ の時間計算量を要する事となってしまうのです。この問題は、[Dutch national flag problem(直訳すると、オランダ国旗問題)](https://en.wikipedia.org/wiki/Dutch_national_flag_problem)とも言われます。
```cpp
// 3way qsort sample code
```
クイックソートは、このようにピボットの選択方法、分割の仕方などの他にも、様々な工夫がされた亜種が存在しています。全てを説明するのは本項の範囲を超えるため特に触れませんが、興味のある方は是非調べてみましょう。<br>
尚、クイックソートは C 標準ライブラリで用意されています。それを次のように利用する事ができます。
```cpp
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
```

### $$ O(N) $$
データ数 N から線形探索を行うといったアルゴリズムがこの計算量オーダーとなります。

### $$ O(logN) $$
挿入ソートの説明の最後で挙げた二分探索がこの計算量オーダーです。二分探索とは、すでにソート済みのデータ列に対して、全体を半分ずつ分けて、別れた片方を検索の対象とするといった処理を反復的に行う探索アルゴリズムです。
```cpp
// draftdraftdraft
```
尚、挿入ソートの検索(探索)で二分探索を行うソートは、二分挿入ソートと呼ばれます。以下はその実装ですが、前述の挿入ソートのコードを利用すると、次の通り探索アルゴリズムを注入できるようにしておいたため、加えるコードは二分挿入の内容しか必要ありません。
```cpp
#include <algorithm>

struct binary_search_insert {
    template <class ForwardIterator, class Compare>
    void operator()(ForwardIterator first, ForwardIterator iter, Compare comp)
    {
        std::rotate(std::upper_bound(first, iter, *iter, comp), iter, std::next(iter, 1));
    }
};

insertion_sort(std::begin(v), std::end(v), std::less<>(), binary_search_insert()); // less than
insertion_sort(std::begin(v), std::end(v), std::greater<>(), binary_search_insert()); // greater than
```
尚、二分挿入ソートそのものの計算量オーダーは $$ O(N^{2}) $$ です。

### $$ O(1) $$
どのようなデータ長、サイズであっても一定時間で計算が完了するオーダーです。配列の添え字アクセスは代表的なものの 1 つです。

## 16.7.2 計算理論におけるクラス


