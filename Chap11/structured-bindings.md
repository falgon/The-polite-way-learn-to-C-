# 11.x Structured bindings

Structured bindings は、複数の変数をタプルのようなオブジェクトから初期化する事ができる言語機能です。従来まではライブラリなどで対応していましたが、言語機能としてサポートする事で、より表現力の高いプログラミングを行う事ができます。


## 11.x.1 概要
Structured bindings は以下のように使います。

```cpp
#include<iostream>
#include<tuple>
#include<functional>

template<class H,class... Ts>
void output(H&& h,Ts&&... ts)
{
        if constexpr(sizeof...(ts)){
                std::cout << std::forward<H>(h) << ' ';
                output(std::forward<Ts>(ts)...);
        }else{
                std::cout << std::forward<H>(h) << std::endl;
        }
}
int main()
{
        auto [x,y,z] = std::make_tuple(42,4.2f,'a'); // Structured bindings

        int i;
        float f;
        char c;
        std::tie(i,f,c) = std::make_tuple(42,4.2f,'a');

        output(x,y,z);
        output(i,f,c);
}
```
実行結果は以下の通りです。
```cpp
42 4.2 a
42 4.2 a
```
`std::tie`とはこのように互換性があり、`x`、`y`、`z`がタプルのそれぞれの要素で初期化されます。尚、`std::tie`では、`std::ignore`などを使ってタプルの要素を無視する事ができますが、 Structured bindings ではそのような機能はありません。
```cpp
std::tie(i,f,std::ignore) = std::make_tuple(42,4.2f,'a');
auto [x,y,std::ignore] = std::make_tuple(42,4.2f,'a'); // このようには書けない
```
しかし、 Structured bindings の効果はタプルだけに止まりません。まず、全ての非静的メンバー変数が公開されている、または全ての public-only 非 statc メンバーが、単一の直接基底クラスにある場合、何の追加コードもなしにこの機能を使う事ができます。
```cpp
#include<iostream>

template<class H,class... Ts>
void output(H&& h,Ts&&... ts)
{
        if constexpr(sizeof...(ts)){
                std::cout << std::forward<H>(h) << ' ';
                output(std::forward<Ts>(ts)...);
        }else{
                std::cout << std::forward<H>(h) << std::endl;
        }
}

struct X{
        int i;
        float f;
        char c;
};

struct Y:X{
        static int integer; // 基底クラスが非 static public-only メンバで、派生クラスが非staticメンバ変数を持たない場合OK
};

int main()
{
        X x{42,4.2f,'a'};
	Y y{42,4.2f,'a'};
        auto [a,b,c] = x; // Structured bindings
	auto [d,e,f] = y; // Structured bindings
        
	output(a,b,c);
	output(d,e,f);
}
```
実行結果は以下の通りです。
```cpp
42 4.2 a
42 4.2 a
```
もしこのような条件に合わない独自のクラスで Structured bindings を使いたければ、ユーザー側でその動作を定義する必要があります。

## 11.x.2 Structured bindings を独自のクラスでサポートする
デモンストレーションのために、簡単なクラスを定義します。これは、`std::string`と`std::vector<int>`を private フィールドに持つ、クラス`X`とします。このクラスに対して Structured bindings をサポートしてみます。
```cpp
struct X{
        template<class InputIterator>
        X(const char* s,InputIterator begin,InputIterator end):str(s),v(begin,end){}

        template<std::size_t s>
        decltype(auto) get()const
        {
                if constexpr(!s){
                        return str;
                }else{
                        return v;
                }
        }
private:
        std::string str;
        std::vector<int> v;
};
```
まずは、`std::tuple_size`を特殊化して、 Structured bindings によって返す要素数を定義します。これには、`std::integral_constant`を使うと便利です。今回返すのは二つの要素なので、値を2とします。
```cpp
namespace std{
        template<>
        struct tuple_size<X> : std::integral_constant<std::size_t,2>{};
}
```
次に、`get`関数を定義します。これは、メンバ関数でも非メンバ関数でも構いませんが、非メンバ関数の場合同名前空間内、もしくはADLの及ぶ名前空間に定義されている必要があります。簡単のため、メンバ関数として定義します。
```cpp
// クラスXのメンバ関数として定義
template<std::size_t s>
decltype(auto) get()const
{
        if constexpr(!s){
                return str;
        }else{
                return v;
        }
}
```
Structured bindings によって返す要素は二つです。これに対して、`x.get<0>()`であれば`X::str`を、`x.get<1>()`であれば`X::v`を返せば良いので、上記のようになっています。もし要素が増えれば、その分分岐が増える事になるでしょう。

最後に`std::tuple_element`を特殊化します。これは、渡されたインデックス値によって返される型を定義すれば良いので、`decltype`を使えば以下のように書けます。
```cpp
namespace std{
        template<std::size_t s>
        struct tuple_element<s,X>{
                using type = decltype(std::declval<X>().get<s>());
        };
}
```
以上でオリジナルのクラス、`X`の Structured bindings へのサポートが完了しました。全体はこのようになります。
```cpp
struct X{
        template<class InputIterator>
        X(const char* s,InputIterator begin,InputIterator end):str(s),v(be
gin,end){}

        template<std::size_t s>
        decltype(auto) get()const
        {
                if constexpr(!s){
                        return str;
                }else{
                        return v;
                }
        }
private:
        std::string str;
        std::vector<int> v;
};

namespace std{
        template<>
        struct tuple_size<X> : std::integral_constant<std::size_t,2>{};
        template<std::size_t s>
        struct tuple_element<s,X>{
                using type = decltype(std::declval<X>().get<s>());
        };
}
```
早速試すために、少し書き足して以下のようにしました。
```cpp
#include<iostream>
#include<vector>
#include<string>
#include<random>
#include<algorithm>
#include<iterator>

struct X{
        template<class InputIterator>
        X(const char* s,InputIterator begin,InputIterator end):str(s),v(begin,end){}

        template<std::size_t s>
        decltype(auto) get()const
        {
                if constexpr(!s){
                        return str;
                }else{
                        return v;
                }
        }
private:
        std::string str;
        std::vector<int> v;
};

namespace std{
        template<>
        struct tuple_size<X> : std::integral_constant<std::size_t,2>{};
        template<std::size_t s>
        struct tuple_element<s,X>{
                using type = decltype(std::declval<X>().get<s>());
        };
}

template<class T>
std::ostream& operator<<(std::ostream& os,const std::vector<T>& v)
{
        std::copy(std::begin(v),std::end(v),std::ostream_iterator<T>(std::cout," "));
        return os;
}

int main()
{
        std::vector<int> v(10);
        std::iota(std::begin(v),std::end(v),0);
        std::shuffle(std::begin(v),std::end(v),std::mt19937());
        auto [x,y] = X("hoge",std::begin(v),std::end(v));

        std::cout << x << std::endl;
        std::cout << y << std::endl;
}
```
ランダムな値をとっているため実行結果は変動しますが、その一例は以下の通りです。
```cpp
hoge
2 9 0 5 4 6 7 1 3 8
```
意図した通りに Structured bindings によって二つのメンバ変数を受け取れている事がわかります。

参照：[https://blog.tartanllama.xyz/c++/2016/07/20/structured-bindings/](https://blog.tartanllama.xyz/c++/2016/07/20/structured-bindings/)
