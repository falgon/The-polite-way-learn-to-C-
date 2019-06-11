# 11.x Structured bindings

Structured bindings(構造化束縛) は、複数の変数をタプルのようなオブジェクトから初期化する事ができる言語機能です。従来まではライブラリなどで対応していましたが、言語機能としてサポートする事で、より表現力の高いプログラミングを行う事ができます。

## 11.x.1 概要
Structured bindings は宣言の機能の一種であり、構文は以下の通りです。
```
属性リスト(optional) cv-auto &or&&(optional) [ 識別子リスト ] = expression ;
属性リスト(optional) cv-auto &or&&(optional) [ 識別子リスト ] { expression } ;
属性リスト(optional) cv-auto &or&&(optional) [ 識別子リスト ] ( expression ) ;
```

識別子リストの変数は、左から順にコピー初期化、または直接初期化がされます。つまり、ムーブのみの型もサポートします。識別子リストに示された変数の型は、分解された各要素を`decltype`したものになります。尚宣言の際、型は必ず`auto`(cvr修飾は許可される)でなければなりません。

Structured bindings が対応する型は、以下のような三つです。

1. 組み込みの配列
2. 非静的データメンバーの全てがパブリックアクセスレベルにある単一のクラス、またはそのクラスから直接派生したクラス
3. ある型を`T`、アクセスするインデックス値を`n`とした時、`std::tuple_size<T>::value`と`std::tuple_element<n,T>::type`が成り立ち、そのクラスのメンバを、メンバ関数または ADL の到達する非メンバ関数 `get` によって取得できるクラス

`1.`と`2.`は言語機能のサポートによって、`3.`は`std`名前空間内への特殊化等を利用する事で実現する事ができます。順に見ていきましょう。


## 11.x.2 組み込み配列に対する Structured bindings
組み込み配列に対しては、言語機能として Structured bindings がサポートされています。識別子リストの個々の名前は配列要素に対応します。宣言の際、識別子リストの要素数は、配列要素数と同一でなければなりません。
```cpp
int ar[]={1,2,3};
auto [a,b,c] = ar;
```

## 11.x.3 Structured bindings を独自のクラスでサポートする
非静的データメンバーの全てがパブリックアクセスレベルにある単一のクラス、またはそのクラスから直接派生したクラスである場合、何の追加コードもなしにこの機能を使う事ができます。
```cpp
#include<iostream>

struct X{
    int a,b;
    static int c; // static メンバーがあっても問題はない
};

struct Y:X{
    static int d; // static メンバーがあっても問題はない
};

int main()
{
    auto [a,b] = Y{10,20}; // a は X::a で、 b は X::b で初期化される。Structured binding では static メンバーは無視される。
    std::cout << a << " " << b << std::endl;
}
```
実行結果は以下の通りです。
```cpp
10 20
```
もしこの要件を満たさない独自のクラスで Structured bindings を使いたければ、ユーザー側でその動作を定義する必要があります。デモンストレーションのために、簡単なクラスを定義します。
```cpp
struct X{};
    
struct Y:X{
    Y(int a,float b,double c):a(a),b(b),c(c){}
    
    int a;
    float b;
    double c;
};
```
これに対して
```cpp
auto [a,b,c] = Y(1,2.0f,3.0);
```
とする事はできません。Structured binding の要件を満たさないためです。要件を満たすようにして、Structured bindings が行えるようにしましょう。<br>
まず、`std::tuple_size`を特殊化します。
```cpp
namespace std{
    template<>
    struct tuple_size<ns::Y> : integral_constant<size_t,3> {};
}
```
Yへの Structured bindings によって分解される変数は、`Y::a`、`Y::b`、`Y::c`の三つですから、`std::tuple_size`の返す値は 3 です。それを`value`で定義します。次に、`std::tuple_element`を特殊化します。
```cpp
namespace std{
    template<>
    struct tuple_size<ns::Y> : integral_constant<size_t,3> {};
    
    template<>
    struct tuple_element<0,ns::Y> : enable_if<true,int>{};
    template<>
    struct tuple_element<1,ns::Y> : enable_if<true,float>{};
    template<>
    struct tuple_element<2,ns::Y> : enable_if<true,double>{};
}
```
与えられたインデックス値を`Y::a`、`Y::b`、`Y::c`それぞれの型と関連づけ、それを`type`で定義します。次に、`Y`関数に適応な、`get`テンプレート関数を定義します。これは、メンバ関数、もしくは ADL の到達する名前空間に定義する事で実現します。どちらでも良いのですが、便宜上まずは非メンバ関数に定義します。
```cpp
namespace ns{

struct X{};
    
struct Y:X{
    Y(int a,float b,double c):a(a),b(b),c(c){}
    
    int a;
    float b;
    double c;
};

template<std::size_t n>
constexpr decltype(auto) get(const Y& y)
{
    if constexpr(!n)return y.a;
    else if constexpr(n==1)return y.b;
    else if constexpr(n==2)return y.c;
}
    
} // namespace ns
```
`get`は、与えられたインデックス値によって対応するメンバーを返します。ここまでの記述と、動作確認を行うコードを追記して以下のようにします。
```cpp
#include<iostream>
#include<tuple>

namespace ns{

struct X{};
    
struct Y:X{
    Y(int a,float b,double c):a(a),b(b),c(c){}
    
    int a;
    float b;
    double c;
};

template<std::size_t n>
constexpr decltype(auto) get(const Y& y)
{
    if constexpr(!n)return y.a;
    else if constexpr(n==1)return y.b;
    else if constexpr(n==2)return y.c;
}
    
} // namespace ns

namespace std{
    template<>
    struct tuple_size<ns::Y> : integral_constant<size_t,3> {};
    
    template<>
    struct tuple_element<0,ns::Y> : enable_if<true,int>{};
    template<>
    struct tuple_element<1,ns::Y> : enable_if<true,float>{};
    template<>
    struct tuple_element<2,ns::Y> : enable_if<true,double>{};
}

int main()
{
    auto [a,b,c] = ns::Y(1,2,3);
    
    ns::Y y(1,2,3);
    
    std::cout << a << " " << b << " " << c << std::endl;
}
```
実行結果は以下の通りです。
```cpp
1
2
3
```
尚、`std::tuple_element`の特殊化は、`decltype`を用いる事で以下のようにリファクタリングできます。
```cpp
namespace std{
    template<>
    struct tuple_size<ns::Y> : integral_constant<size_t,3> {};
    
    template<size_t n>
    struct tuple_element<n,ns::Y>{
        using type = decltype(ns::get<n>(std::declval<ns::Y>()));
    };
}
```
尚、標準ライブラリである`std::tuple`は、標準で`std::tuple_size`、`std::tuple_element`に対応し、`std::get`によって要素を取得できますから、 Structured bindings の機能を使う事ができます。
```cpp
auto [x,y,z] = std::make_tuple(42,4.2f,'a');
```
`std::tuple`には似たようなライブラリ機能として、`std::tie`がありますが、要素を無視する場合に`std::ignore`を利用する事ができます。Structured bindings ではそのような機能はありません。
```cpp
std::tie(i,f,std::ignore) = std::make_tuple(42,4.2f,'a');
auto [x,y,std::ignore] = std::make_tuple(42,4.2f,'a'); // このようには書けない
```
さらに、標準ライブラリである`std::array`、`std::pair`は、標準で`std::tuple_size`、`std::tuple_element`、`std::get`に対して特殊化が行われており、同様に Structured binding する事ができます。
```cpp
auto [a,b] = std::array<std::size_t,2>{1,2};
auto [c,d] = std::make_pair(1,2);  
```

## 11.x.4 Structured bindings と decltype
Structured bindings によって宣言された変数を`decltype`した型は[P0144R2](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0144r2.pdf)などを見ると、<u>実際には隠された変数へのリファレンスとなるから、リファレンス型に推論されるのではないか(1)</u>、また<u>宣言時に参照を付与した場合、これも同様`decltype`を適用するとリファレンス型に推論されるのではないか(2)</u>と考えられるかもしれませんが、**両者ともそうではありません**。(1)、(2)についてそれぞれ具体的な、コードで示せば、以下のような感じでしょうか。
```cpp
// (1)
auto t = std::make_tuple(1);
auto [x] = t;
auto [y] = std::make_tuple(1);
decltype(x) a; // a の型は int& ?
decltype(y) b; // b の型は int&& ?
```
```cpp
// (2)
auto t = std::make_tuple(1);
auto &[a] = std::make_tuple(t);
decltype(a) b; // b の型は int& ?
```
[\[dcl.struct.bind/3\]](http://eel.is/c++draft/dcl.struct.bind#3)から引用します。

> Given the type T<i>i</i>`std::tuple_element<i, E>::type`, each v<i>i</i> is a variable of type “reference to T<i>i</i>” initialized with the initializer, where the reference is an lvalue reference if the initializer is an lvalue and an rvalue reference otherwise; the referenced type is T<i>i</i>.

`std::tuple_element::type`で指定された型 T<i>i</i> が与えられた場合、各v<i>i</i>は初期化子で初期化された "reference to T<i>i</i>" 型の変数であり、初期化子が左辺値の場合は左辺値参照、それ以外の場合はrvalue参照で、<strong>参照される型(referenced type)はT<i>i</i></strong>です。referenced type は、参照型、つまり lvalue reference だとか rvalue reference だとかを言っているのではなく、全く別の意味である、**参照された型**である事に注意してください。つまり、まず **Structured binding において referenced type とは`std::tuple_element::type`に設定された型であるという事**です。次に、[\[dcl.type.auto.deduct\]](http://eel.is/c++draft/dcl.spec.auto#dcl.type.auto.deduct-5)を見てみると以下のように述べられています。

> If the placeholder is the decltype(auto) type-specifier, T shall be the placeholder alone. The type deduced for T is determined as described in [dcl.type.simple], as though e had been the operand of the decltype.

プレースホルダが`decltype(auto)`型指定子である場合、Tはプレースホルダだけでなければならない。Tが導出される型は、eが宣言型のオペランドであるかのように、[\[dcl.type.simple\]](http://eel.is/c++draft/dcl.type.simple)で説明されているように決定される。との事なので、早速 [dcl.type.simple] を見て見ます。この場合、[\[dcl.type.simple\]/4.1](http://eel.is/c++draft/dcl.type.simple#4.1)に該当します。

> if e is an unparenthesized id-expression naming a structured binding ([dcl.struct.bind]), **decltype(e) is the referenced type** as given in the specification of the structured binding declaration;

よって、(1)、(2)はそれぞれ以下のように推論されます。
```cpp
// (1)
auto t = std::make_tuple(1);
auto [x] = t;
auto [y] = std::make_tuple(1);
decltype(x) a; // 推論される型は typename std::tuple_element<0,decltype(t)>::type,つまり int
decltype(x) b; // 同様 
```
```cpp
// (2)
// 以下同様
```
よって以下の要件を必ず満たす事が言えます。
```cpp
auto t = std::make_tuple(1);
auto [a] = t;
static_assert(std::is_same_v<typename std::tuple_element<0,decltype(t)>::type,decltype(a)>);
```
尚、これは余談になりますが、**GCC 7.1.0 では、この推論を decltype(auto) で行おうとすると、コンパイルエラーとなりますが、この挙動はコンパイラによるバグであり**、[バグレポートが送信されています](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=81176)([参照](https://stackoverflow.com/questions/44698195/is-decltypeauto-for-a-structured-binding-suppose-to-be-a-reference))。
```cpp
auto t = std::make_tuple(1);
auto [a] = t;
    
decltype(a) b = a;
decltype(auto) d = a;
    
static_assert(std::is_same_v<typename std::tuple_element<0,decltype(t)>::type,decltype(a)>);
static_assert(std::is_same_v<decltype(b),decltype(d)>);
```
尚、clang 4.0.0 では、規格通り正常に解釈されます。
