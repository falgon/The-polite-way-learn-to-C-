# 10.2 Attribute
Attribute(アトリビュート)とは、直訳すると属性です。
この昨日は、ソースコードに対して追加の情報をコンパイラに伝え、主にコンパイラの出力する警告文のコントロールに活かされます。
この機能を使う事で、こちらの意図をコンパイラに伝える事ができるため、例えばテストなどを行いたい場合に余計な警告文を抑えるなど、よりプログラマーとコンパイラの親和性を深める事となるでしょう。

## 10.2.1 基本的な構文
Attributeの基本的な構文を示します。といっても、全くもって単純です。
```cpp
[[ attribute ]]
```
`[[`と`]]`で囲み、その内部に任意の`attribute`を記述します。記述箇所は該当する識別子の定義の前です。ここからは、各Attributeとそれらの基本的な使い方を示します。
また、標準化されたAttributeは比較的新しい機能なので、コンパイラやそのバージョンによっては対応していないかもしれません。
対応状況は、標準で定義されるマクロ、`__has_cpp_attribute`で確認する事ができます。各属性ごとに、定義される場合の数値を、同時に示します。

## 10.2.2 deprecated属性
deprecated属性とはその該当する識別子やエンティティがまだ使う事はできるもののその利用を推奨しない事を明示するAttributeです。
これらは、クラス、共有体、typedef、変数、非staticデータメンバー、関数、名前空間、enum、enumerator、テンプレートの特殊化に指定する事ができます。それぞれの記述方法は以下のコードに示されている通りです。
```cpp
class [[deprecated]] X{ // クラスへのdeprecated
    [[deprecated]] void f(); // 非staticデータメンバーへのdeprecated
    [[deprecated]] int i; // 非staticデータメンバーへのdeprecated
};

union [[deprecated]] U; // 共有体へのdeprecated

[[deprecated]] typedef int type1; // typedefへのdeprecated

using type2 [[deprecated]] = int; // エイリアス宣言宣言にもtypedefと同じくdeprecatedできる

[[deprecated]] int i; // 変数へのdeprecated

[[deprecated]] void f(); // 関数へのdeprecated

namespace [[deprecated]] ns{} // 名前空間へのdeprecated

enum [[deprecated]] Y{ // enumへのdeprecated
    enumerator [[deprecated]] = 0 // enumeratorへのdeprecated
};

template<class>
class Z;

template<>
class [[deprecated]] Z<int>; // テンプレートの特殊化へのdeprecated
```
deprecated属性が指定された識別子やエンティティを使用するとコンパイラーによって警告メッセージが出力されます。
deprecated属性には、任意の文字列を指定する事が可能で、指定された文字列は警告メッセージに含まれる場合があります(規定されていません)。
```cpp
// t.cpp
int main()
{
    [[deprecated("i is deprecated.Please do not use this...")]] int i;
    i=42;
}
```
GCC 7.0.1でコンパイルした場合、以下のようなメッセージが出力されました。
```cpp
t.cpp:4:2: warning: 'i' is deprecated: i is deprecated.Please do not use this...
      [-Wdeprecated-declarations]
        i=42;
        ^
t.cpp:3:66: note: 'i' has been explicitly marked deprecated here
        [[deprecated("i is deprecated.Please do not use this...")]] int i;
                                                                        ^
1 warning generated.
```
尚、無名名前空間に対してdeprecated属性を指定すると、無視されます。
```cpp
namespace [[deprecated]]{} // 無視される
```
尚、`__has_cpp_attribute(deprecated)`が定義された場合、その値は201309です。

### 10.2.3 fallthrough属性
fallthrough属性はswitch文中の`case`内で`break`が行われなかったとしても、それは意図しているものであるという旨をコンパイラや他のプログラマに伝え、警告を抑止するための属性です。
例えば、`switch`文を以下のように記述したとします。
```cpp
void f(int i)
{
    switch(i){
        case 1:
            // do something
            break;
        case 2:
            // do something
            break;
        case 3:
            // do something
        case 4:
            // do something
            break;
        default:
            // do something
            ;
    };
}

int main()
{
    f(3);
}
```
賢いコンパイラーは`case 3:`の`case`ラベル内での警告を発するかもしれません。これは、`break`や`return`によって`case`ラベル内で処理が完結せず、上記の場合は`case 3`に差し掛かった後、`case 4`がそのまま実行される事となります。これは、誤って`break`文を忘れたのか、意図したものなのかはコンパイラーにも自分以外の他のプログラマーにも伝える事ができません(コメントを記述すれば他のプログラマーに伝える事はできるかもしれませんが、言語機能を用いる事が明確性を高めます)。
もし上記のコードが、プログラマーの意図した記述であるとすれば、fallthrough属性を用いる事でコンパイラからの警告を抑止できます。
```cpp
void f(int i)
{
    switch(i){
        case 1:
            // do something
            break;
        case 2:
            // do something
            break;
        case 3:
	   // do something
            [[fallthrogh]] // フォールスルーは意図的である事を示す
        case 4:
            // do something
            break;
        default:
            // do something
            ;
    };
}

int main()
{
    f(3);
}
```
尚、`__has_cpp_attribute(fallthrough)`が定義される場合、その値は201603です。

## 10.2.4 noreturn属性
noreturn属性は、関数が決して返らないことを示すための属性です。

これは、関数が返らないという情報を元にコンパイラが任意の最適化のチャンスを与えると共に、返らない処理をラップした関数にnoreturn属性を付けることで、「関数が返らないパスが存在する」というコンパイラからの警告を抑制するために用います。
例えば以下のようなケースが考えられます。
```cpp
// t.cpp
#include<stdexcept>

void report_error()
{
      throw std::runtime_error("error");
}

int f(int x)
{
    if (x>0)return x;
    
    report_error();
}

int main()
{
      f(1);
}
```
`if`文の条件によっては`return x`が処理されますが、そうでない場合は`report_error`関数が実行されます。しかし、report_error関数は`void`であり、且つそれに対する`return`文もないため、何も値が返されない、とコンパイラは捉えます。よって、警告文が出力される事があります。
GCC 7.0.1では以下のような警告文が出力されました。
```cpp
t.cpp: In function ‘int f(int)’:
t.cpp:13:1: warning: control reaches end of non-void function [-Wreturn-type]
 }
 ^
```
確かに何も値は返されないのですが、`report_error`内部で行われているのは例外のスローです。よって、これは返却される必要はないのです。
その旨を明示できるのが、noreturn属性です。
```cpp
#include<stdexcept>

[[noreturn]] void report_error()
{
      throw std::runtime_error("error");
}

int f(int x)
{
    if (x>0)return x;
    
    report_error();
}

int main()
{
      f(1);
}
```
尚、一つの翻訳単位でnoreturn属性が指定された関数は他の翻訳単位でも一括してnoreturn属性付きで宣言されなければなりません。
また、noreturn属性が指定された関数から返却された場合、その動作は未定義ですので、noreturn属性の付いた関数から何かを返したりしてはなりません。

尚、`__has_cpp_attribute(noreturn)`が定義された場合、その値は200809です。

## 10.2.5 maybe_unused属性
以下のコードをコンパイルすると警告文が発せられるかもしれません。
```cpp
// t.cpp
void f(int x)
{}

int main()
{
    f(42);
}

```
GCC 7.0.1では以下のような警告文を出力しました。
```cpp
t.cpp: In function ‘void f(int)’:
t.cpp:1:12: warning: unused parameter ‘x’ [-Wunused-parameter]
 void f(int x)
            ^
```
これは、関数`f`の仮引数で`x`と名付けているのにも関わらず一切その識別子を使わないために出力されます。しかし、意図してこのようにしていた場合は誤った警告文となってしまいます。
maybe_unused属性は、このような名前の付いた識別子を利用しない事が意図したものであるという事をコンパイラに伝えて警告を抑制するための属性です。
```cpp
void f([[maybe_unused]] int x)
{}

int main()
{
    f(42);
}
```
これらは、クラスの、typedef、変数、非静的メンバ変数、関数、enum、enumeratorの宣言時に使用する事ができます。
尚、GCC 7.0.1では規格とは異なりますが、`[[maybe_unused]]`の代替として`[[gnu::unused]]`を使います。
また、`__has_cpp_attribute(maybe_unused)`が定義される場合の値は201603で、`__has_cpp_attribute([[gnu::unused]])`が定義される場合の値は1です。
