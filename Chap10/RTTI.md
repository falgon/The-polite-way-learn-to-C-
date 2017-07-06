# 10.3 RTTI(RunTime Type Identification)  / 実行時型情報

RTTI は、プログラムの実行中に参照できる型情報の事です。この機能について順に見ていきましょう。

## 10.3.1 概念
RTTI によって、前述した通り、プログラムの実行中に型情報を参照する事ができます。例えば、あるオブジェクトの型は何なのかを実行中に知る事ができます。よくある例としては、継承関係の親にあたる型`X`のポインター型、つまり`X*`型のポインターが示しているオブジェクトの型は、本当に`X`なオブジェクトを示しているのか調査したいといった場合に使う事ができます。
```cpp
struct X{};
struct Y:X{};

void f(X* ptr); // ptr が指し示すオブジェクトの型は X かもしれないし、Yかもしれない
```
型情報を取得するためには、まず`<typeinfo>`ヘッダーをインクルードする必要があります。`<typeinfo>`ヘッダーの概要は以下の通りです。
```cpp
namespace std {
    class type_info;
    class bad_cast;
    class bad_typeid;
}
```
`<typeinfo>`ヘッダーは、コンパイラによって生成された型情報に関連付けられた型を定義します(`type_info`)。また、動的型識別エラーを報告するための2つのタイプも定義しています(`bad_cast`、`bad_typeid`)。<br>
更に`typeid`演算子を利用します。`typeid`演算子は、`<typeinfo>`ヘッダーで定義されるクラス`type_info`のオブジェクトへの参照を生成します。また、`typeid`演算子には型情報または、式を与える事ができます。型情報を与えた場合はその型として、式を与えた場合はその式の結果の型についての型として情報を返します。

早速使って見ましょう。
```cpp
#include<typeinfo>
#include<iostream>

int main()
{
        const std::type_info& t1 = typeid(int);
        const std::type_info& t2 = typeid(double);

        std::cout << std::boolalpha << (t1 == t2) << std::endl;

        int a;
        int* p = &a;

        std::cout << (typeid(*p) == typeid(int)) << std::endl;
}
```
実行結果は以下の通りです。
```cpp
false
true
```
前述した通り、`typeid`演算子は`std::type_info`のオブジェクトへの参照を返すため、それを`const`参照で受け取っていますね。勿論、ポインターで持つ事もできます。
```cpp
std::type_info const* tp = &typeid(int);
```
`type_info`クラスは、演算子`==`と`!=`に対するオーバーロードがされているため、上記のように2つの型が同じ型か判定する事ができます。
尚、`type_info`クラスは、ユーザーが`typeid`演算子を使用する事によってのみオブジェクトを生成する事ができるため、`type_info`クラスを直接ユーザーがデフォルト構築、コピー、ムーブを行う事はできません。
```cpp
std::type_info t1; // エラー！デフォルト構築は delete 指定されている
std::type_info t2 = typeid(int); // エラー！コピー(ムーブ)は delete 指定されている
```
尚、`typeid`式の返すオブジェクトの寿命は通常のスコープのような概念とは異なり、プログラムが終了するまでとなっています。
```cpp
int main()
{
    std::type_info const* ptr;
    {
        ptr = &typeid(int);
    }
    bool b = *ptr == typeid(int); // true
}
```
また`std::type_info`クラスは、`before`、`hash_code`、`name`というメンバ関数を持っています。<br>
まず、`before`メンバ関数は、2つの型の照合順序を比較し、`*this`が`rhs`よりも先行していれば`true`、さもなくば`false`を返します。しかし`before`メンバ関数の主な用途は内部実装によるものなので、あまり利用する機会も多くないかもしれません。
```cpp
#include<typeinfo>
#include<iostream>

int main()
{
        const std::type_info& t1 = typeid(int);
        const std::type_info& t2 = typeid(double);

        std::cout << std::boolalpha << t1.before(t2) << std::endl;
        std::cout << t2.before(t1) << std::endl;
}
```
GCC 7.1.0 と Clang 4.0.0 の両者の処理系でコンパイルすると、内部実装の違いから出力結果が互いに異なります。<br>
<br>`hash_code`メンバ関数は、型のハッシュコードを取得する関数です。この値は、`std::size_t`型で返されますが、値は末規定です。しかし、一度のプログラム実行中において同じ型に対しては同じ値が返されます。また異なる型に対するハッシュコードは異なる値を返す事が実装に対して推奨されています。
```cpp
#include<typeinfo>
#include<iostream>

int main()
{
        std::cout << typeid(int).hash_code() << std::endl;
        std::cout << typeid(double).hash_code() << std::endl;
}
```
筆者の実行時では、以下のように出力されました。
```cpp
6253375586064260614
14494284460613645429
```
これは、後の第12章 STLで取り上げられる、`map`などと組み合わせたりするような用途が考えられます。<br><br>
`name`メンバ関数は、 その`type_info`が保持する型情報を表す null 終端された文字列を返します。
```cpp
#include<typeinfo>
#include<iostream>

int main()
{
        std::cout << typeid(int).name() << std::endl;
        std::cout << typeid(double).name() << std::endl;
}
```
筆者の環境では以下のように出力されました。
```cpp
i
d
```
この返される文字列は実装依存となっており、上記のように`int`型を指定したからと言って必ず「int\0」という文字列が返ってくる保証はありません。多くの場合、マングリング処理の施された文字列が返ってきます(マングリングについては[16.3 マングリング](Chap16/163-マングリング.md)にて取り上げています)。

RTTI そのものの機能は、これで全てです。この機能がどのように使えるのか、まだ少し想像がつかないかもしれませんが、第12章で取り上げられる、`map`などの機能や、`std::type_index`と併用する事で有用性が見えてきます。<br>
また、上記のように、`name`メンバ関数は実装依存の文字列を返すため、場合によっては元の型情報が判別できない文字列が返ってくる可能性も十分に有りえますが、第15章で取り上げる Boost C++ Libraries の`boost::typeindex`を用いれば、人間にとって分かりやすい文字列を取得する事ができます。
