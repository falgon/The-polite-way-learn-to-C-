
## 11.x.x SFINAEと型特性(type\_traits)
[11.1.6](/Chap11/111-関数テンプレート.md#1116-sfinae)にて、`int`を`T::value_type`に変換できる場合のみ関数が呼び出されるようにする事ができると述べました。それについて、本項では説明していきます。まず、導入として以下のコードを見ていただきましょう。
```cpp
template<bool,class=void>
struct enable_if;

template<class T>
struct enable_if<true,T>{
        using type = T;
};

template<class T>
struct enable_if<false,T>{};
```
`enable_if`というクラスを特殊化して作っています。特殊化は`true`と`false`で行われていますが、`false`だった場合、`type`が定義されません。つまり、テンプレートで推論される型の特性に従って、その値を`bool`の引数部分に適用し、この`enable_if`の`type`を必要要件とするようにシグネチャに対して埋め込めば、SFINAEを意図的に発動させて、プログラマの思った通りにオーバーロード解決が行われるよう操る事ができるのです。<br>
例えば、以下のようなテンプレート関数`f`があったとしましょう。
```cpp
template<class T>
void f(T){}
```
このままでは、どんな型でも関数`f`の引数に与える事ができます。これを、例えば推論される型が`int`だった場合のみ当関数を呼び出すように指定してみるとしましょう。まずは二つの型を比較して、それらが同じ型だった場合に`true`を、そうでない場合に`false`を保持するクラスを作る事から考えます。
```cpp
template<class,class>
struct is_same{
        static constexpr bool value = false;
};

template<class T>
struct is_same<T,T>{
        static constexpr bool value = true;
};
```
二つのクラスが同じ型だった場合に、`is_same`は`value`に`true`を、そうでなければ`false`を持ちます。もうこれで、準備はできました。あとは、テンプレート関数`f`のシグネチャに加えるだけです。やってみましょう。
```cpp
template<class T,typename enable_if<is_same<T,int>::value>::type* =(void*)0>
void f(T){}

int main()
{
        f(42); // OK
        f(4.2f); // エラー！
}
```
テンプレート関数`f`に対して`int`型のみ許可するという要件を付与する事ができている事がわかります。`typename enable_if<...>::type`はデフォルトでは`void`なので、それをポインタ型にしています。尚、テンプレート引数に対してこのような要件を記述していますが、関数のシグネチャであればどこに書いても構いません。例えば、以下のようなバリエーションが考えられます。
```cpp
template<class T>
typename enable_if<is_same<T,int>::value>::type f(T){} // 戻り型部分にenable_if
```
```cpp
template<class T>
auto f(T) -> typename enable_if<is_same<T,int>::value>::type {} // 後置の戻り方部分にenable_if
```
```cpp
template<class T>
void f(T,typename enable_if<is_same<T,int>::value>::type* =0){} // 引数にenable_if
```
筆者個人の見解としては、そのスタイルが見合うのであればどれでも良いと考えますが、テンプレートパラメータ部分や、引数の部分で`enable_if`を用いるのであれば、それなりの工夫をした方が好ましいでしょう。というのも、実際にわざとその部分に引数を与えた場合、確かに動作自体には何の影響も与えませんが、それぞれの関数呼び出しで別々のインスタンス化が発生するため、これは少々無駄と言えるのです。例えば、以下のような場合を考えましょう。
```cpp
template<class T,typename enable_if<is_same<T,int>::value>::type* =(void*)0>
void f(T){}

int main()
{
        f(42); // これと
        f<int,nullptr>(42); // これは別の実体化
}
```
わざわざここまでして関数テンプレートを呼び出す事も中々考えにくいですが、こういった事が<u>できてしまう</u>というのは、あまり良い事ではありません。これは、`enable_if`を引数部分に記述しても同じ事がいえます。
```cpp
template<class T>
void f(T,typename enable_if<is_same<T,int>::value>::type* =(void*)0){}

int main()
{
        f(42); // これと
        f(42,nullptr); // これは別の実体化
}
```
この問題は、何が原因なのか考えてみると、型に対して値が2種類以上存在する事に原因がある事がわかります。すなわち、型に対して値が1種類しか有りえない型を使えば良い事となるのです。その要件を満たす型として、`std::nullptr_t`があります。`std::nullptr_t`は、`nullptr`以外の一切の値を受け付けませんのでこの要件にぴったりなのです。早速使ってみましょう。
```cpp
template<class T,typename enable_if<is_same<T,int>::value,std::nullptr_t>::type = nullptr>
void f(T){}

int main()
{
        f(42); // これと
        f<int,nullptr>(42); // これは同じ実体化
}
```
もし渡すにしても、`nullptr`しか渡す事ができないので、両者の実体は同じになるのです。よって、テンプレートパラメータ部分や、引数部分に`enable_if`を記述する場合は、`std::nullptr_t`を使う事が推奨されます。

最後に、もう少しだけ変更を加えます。この`enable_if`や`is_same`、一々`::type`や`::value`と記述しなければならなく、特に`::type`の方は、コンパイラが型である事を推測できるように`typename`キーワードを事前に付与する必要があり、やや冗長に感じられます。これらは、エイリアステンプレート、テンプレート変数を使う事でやや解消できます。
```cpp
template<bool,class=void>
struct enable_if;

template<class T>
struct enable_if<true,T>{
        using type = T;
};

template<class T>
struct enable_if<false,T>{};

template<bool b,class T=void>
using enable_if_t = typename enable_if<b,T>::type; // エイリアステンプレート

template<class,class>
struct is_same{
        static constexpr bool value = false;
};

template<class T>
struct is_same<T,T>{
        static constexpr bool value = true;
};

template<class L,class R>
constexpr bool is_same_v = is_same<L,R>::value; // テンプレート変数

template<class T,enable_if_t<is_same_v<T,int>,std::nullptr_t> = nullptr>
void f(T){}

int main()
{
        f(42);
        f<int,nullptr>(42);
}
```
さらに、`is_same`の実装部分で、`value`を定義してそれぞれに`true`と`false`を設定していますが、標準には`std::true_type`と`std::false_type`が用意されており、これらを継承すると一々`value`を宣言して...などとする必要はなくなります。`std::true_type`と`std::false_type`は`<type_traits>`ヘッダーに定義されており、それぞれ`value`という値を持ち、`std::true_type`は`true`が、`std::false_type`は`false`が設定されています。
```cpp
template<class,class>
struct is_same:std::false_type{};

template<class T>
struct is_same<T,T>:std::true_type{};
```
このように、とても便利な`enable_if`、`is_same`ですが、標準で用意されていないのかとお思いかもしれません。まさにその通り`<type_traits>`ヘッダーをインクルードする事でこれらの機能を使う事ができます。`<type_traits>`には、`is_same`も含め、他の様々な型特性に対するテンプレートクラスが用意されています。
```cpp
#include<type_traits>

template<class T,std::enable_if_t<std::is_same_v<T,int>,std::nullptr_t> = nullptr>
void f(T){}

int main()
{
        f(42);
        f<int,nullptr>(42);
}
```
