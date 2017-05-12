# 11.x constexpr if再考

「第8章 定数」で`constexpr if`文について触れましたが、テンプレートの知識をある程度得たところで、再度`constexpr if`について触れて起きます。

`constexpr if`文はコンパイル時の条件分岐を行う事ができる文法です。構文は以下の通りです。

```cpp
if constexpr(条件式)
// ...
else if constexpr(条件式)
// ...
else
/// ...
```

`if`キーワードの後、条件式のカッコの前に`constexpr`キーワードを付与します。このように記述した場合、条件式の値は、bool型に文脈変換された定数式になります。

この機能により、今まではSFINAEや再起で分岐していた処理を`if`文の形態で実現できるようになります。サンプルコードを以下に示します。

```cpp
#include<iostream>

void g(){}

template<class T,class... Rest>
void g(T&& p,Rest&&... rs)
{
    std::cout<<std::forward<T>(p)<<std::endl;
    g(std::forward<Rest>(rs)...);
}

int main()
{
    g(42,42,42,42,42,42);
}
```

実行結果は以下となります。

```cpp
42
42
42
42
42
42
```

パラメーターパックを一つずつ減らす事によって、最後の関数`g`の呼び出しは引数がなくなりますから、引数のない関数`g`をオーバーロードして呼び出す事となり、再起が終了します。

しかし上記の通り再起を用いた分岐は、視覚的にとても直感的とは言えません。これを、`constexpr if`文で実現すると以下のように記述できます。

```cpp
#include<iostream>

template<class T,class... Rest>
void g(T&& p,Rest&&... rs)
{
    std::cout<<std::forward<T>(p)<<std::endl;
    if constexpr(sizeof...(rs)>0)
        g(std::forward<Rest>(rs)...);
}

int main()
{
    g(42,42,42,42,42,42);
}
```
実行結果は同じです。また、`constexpr if`文で`if constexpr`~`else`節を用いてその条件に合わないステートメントは実体化されません。しかし、実体化されないだけで文法的な誤りはチェックされます。
```cpp
#include<iostream>
#include<type_traits>

template<class...>
constexpr int g(){return 42;}

template<class... T>
void f(T&&...)
{
    if constexpr(std::conjunction_v<std::is_same<int,T>...>){
        std::cout<<"T is int"<<std::endl;
    }else{
        return g<T...>();
    }
}

int main()
{
    f(42);
}
```
関数`f`の`else`節を見てください。関数`f`は`void`であると宣言していますが、関数`g`は`int`を返しますからコンパイルエラーとなっても可笑しくはなさそうです。しかし、このコードはコンパイルエラーとはなりません。何故ならば、先ほども述べた通り、`else`節は`if constexpr`の条件の合致によって、実体化されないからです。
ここで、例えば引数に与える値を`int`型`以外の値に変えてみるとどのようになるでしょうか。
```cpp
int main()
{
    f(4.2); // double型の値を渡す
}
```
すると、GCC7.1.0では以下のようなエラー文が出力されました。
```cpp
In instantiation of 'void f(T&& ...) [with T = {double}]':
19:10:   required from here
13:24: error: return-statement with a value, in function returning 'void' [-fpermissive]
         return g<T...>();
                        ^
```
`int`型以外の値を渡せば、当然`if constexpr`の条件に合致しないため`else`節の内容が実体化されます。すると、関数`g`が実体化し、関数`g`が値を返してくるため、その旨をエラーとして教えてくれました。if constexpr`は、このような実体化の防止を作用させる事ができる機能なのです。

`if constexpr`を使う有用な例として、条件にあった型以外を受け取った場合に`static_assert`を発動する例です。まず`if constexpr`を使わずSFINAEで実現した場合は以下のようになります。
```cpp
#include<iostream>
#include<type_traits>

template<class...>
constexpr bool false_v=std::false_type::value;

template<class... T>
constexpr void f(T&&...)
{
    static_assert(false_v<T...>,"T is not int !");
}

template<class... T,std::enable_if_t<std::conjunction_v<std::is_same<int,T>...>,std::nullptr_t> =nullptr>
void f(T&&...)
{
    std::cout<<"T is int"<<std::endl;
}

int main()
{
    f(42);
}
```
実行結果は以下の通りです。
```cpp
T is int
```
関数`f`で、`false_v`という変数テンプレートを用いているのは、実体化のタイミングを遅延させるためです。渡されるテンプレート型に依存させる文を含む事で、`static_assert`をtwo-phase name lookupのタイミングで実体化させています。
`if constexpr`を用いれば、以下のように記述できます。
```cpp
#include<iostream>
#include<type_traits>

template<class...>
constexpr bool false_v=std::false_type::value;

template<class... T>
void f(T&&...)
{
    if constexpr(std::conjunction_v<std::is_same<int,T>...> and sizeof...(T)){
        std::cout<<"T is int"<<std::endl;
    }else{
        static_assert(false_v<T...>,"T is not int !");
    }
}

int main()
{
    f(42);
}
```
とこのように、`false_v`にテンプレート引数を渡す事で実体化のタイミングを遅らせていますが、要するにテンプレート引数に依存さえすれば実体化のタイミングを遅延させる事ができるという事なので、例えば以下のように`sizeof`演算子に適用する事でも同じ事が可能です。
```cpp
#include<iostream>
#include<type_traits>

template<class... T>
void f(T...)
{
    if constexpr(std::conjunction_v<std::is_same<int,T>...> and sizeof...(T)){
        std::cout<<"T is int"<<std::endl;
    }else{
        static_assert(sizeof...(T)-sizeof...(T),"T is not int!"); // sizeofに適用する事で実体化タイミングをtwo-phase name lookupのタイミングにする
    }
}

int main()
{
    f(42);
}
```
まず、`sizeof...(T)`とする事で`T`というテンプレート引数に依存する事となるためこの段階でtwo-phase name lookupが適用されます。`sizeof...(T)`は、Variadic templatesの引数の数を表しますので、その値同士を減算し、必ず`0`にさせる事で`else`節の実行が確定した段階で`static_assert`の発動させます。上記の場合、`false_v`といったようなヘルパテンプレート変数を定義しなくとも同じ動作を行えるため、コードの記述量を考慮した場合、こちらのコードの方が優秀であると考えられるかもしれませんが、意図をより明確に伝えるために、敢えて`false_v`といったようなヘルパテンプレート変数を定義するなどして利用することは、それもまた1つの良い策と言えるでしょう。
コード量のみ考慮するのであれば、例えばVariadic templateでなかった場合、更に以下のように短く記述できます。
```cpp
#include<iostream>
#include<type_traits>

template<class T>
void f(T)
{
    if constexpr(std::is_same_v<int,T>){
        std::cout<<"T is int"<<std::endl;
    }else{
        static_assert(!sizeof(T),"T is not int!");
    }
}

int main()
{
    f(42);
}
```
sizeofから返ってくる値は必ず`1`以上なので、それを`!`によって反転させることで必ず`0`となります。よって`else`節が確定された段階で`static_assert`が発動します。尚、この手法は、Variadic templatesに対しては適用できません。下記コードは、何も考えずに、先ほどと同じようにVariadic templatesに対して`sizeof...(T)`を用いたものに`!`を適用した例です。
```cpp
#include<iostream>
#include<type_traits>

template<class... T>
void f(T...)
{
    if constexpr(std::conjunction_v<std::is_same<int,T>...> and sizeof...(T)){
        std::cout<<"T is int"<<std::endl;
    }else{
        static_assert(!sizeof...(T),"T is not int!");
    }
}

int main()
{
    f();
}
```
引数に何も設定していないので、`else`節が確定します。これは一見、うまく`static_assert`が発動するように思えますが、実際は何事もなくコンパイルが通ってしまいます。これは当然で、あくまで`sizeof...(T)`が`T...`
それぞれの型のサイズを表すのではなくVariadic templatesの個数を表すものなので、その数が`0`であった場合、反転して`1`となってしまうのです。よって`static_assert`が発動しません。
See also:[http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2016/p0292r2.html](http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2016/p0292r2.html)

