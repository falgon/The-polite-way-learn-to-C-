# 6.3 Namespace

C++には名前空間(namespace)という概念があります。名前空間は、これまたスコープなどに関する概念です。まず、名前空間は以下のように使います。

```cpp
namespace Nm{
    int a;
}
int main()
{
    Nm::a=10;
}
```
`namespace 任意の名前空間名{}`とする事で名前空間を定義する事ができます。そしてその名前空間内に定義されたデータには`Nm::`と記述しなければアクセスする事はできません。関数も同様です。

```cpp
namespace Nm{
    void func(){}
}
int main()
{
    Nm::func();
}
```
たった、それだけです。これは一体何に役立つのでしょうか？

### 6.3.1 名前空間の役割

名前空間を定義する事によって、例えば以下のような定義が可能です。

```cpp
namespace Nm1{
    void func(){}
}
namespace Nm2{
    void func(){}
}
int main()
{
    Nm1::func();
    Nm2::func();
}
```
このように名前空間さえ異なれば、関数名とそのシグネチャが同じでも、定義する事ができるのです。また名前空間を名前空間で囲う事も可能です。

```cpp
namespace Nm1{
    namespace Nm2{
        void func(){}
    }
}
int main()
{
    Nm1::Nm2::func();
}
```

この場合、`func`関数を使いたければ、上記のようにそのデータまでのパスを`::`で明示的に示す事でその機能を使う事ができます。また、上記のように名前空間を何重にも内包(ネスト)している名前空間は、以下のように省略して宣言する事もできます。

```cpp
namespace Nm1::Nm2{
    void func(){}
}
int main()
{
    Nm1::Nm2::func();
}
```
この記述法は、Nested Namespace definitionと呼ばれます。

これら名前空間の機能により、例えば自分以外の誰かが作ったライブラリなどを使う際に、自分の定義した識別子がたまたまそのライブラリ内にある識別子名と重なってしまい、自分がその識別子を使う事ができない...といった事を防ぐ事ができます(たまたま同じ名前空間を定義していた場合はダメ)。実はこの機能、今まで触れてきていませんでしたが何気なく使ってきていました。

値を標準出力させるためには、以下のようなコードを書いてきましたね。

```cpp
#include<iostream>
int main()
{
    std::cout<<"hoge"<<std::endl;
}
```
これはつまり、`std`という名前空間に`cout`と`endl`が予め定義されており、それを使っているがための記述だったのです。そしてこれらは、`iostream`ヘッダに定義されています。

### 6.3.2 using宣言

以下のコードで関数`func`を使うためには

```cpp
namespace Nm{
    void func(){}
}
```
以下のように明示的に指定する必要があると述べました。

```cpp
int main()
{
    Nm::func();
}
```
しかし、初めから`Nm`という名前空間内の機能を使うつもりで、識別子の衝突も有り得ないと分かっているようであれば、`Nm::`というように一々明示的に記述するのも面倒です。

そういった場合はusing宣言をすると便利です。

```cpp
namespace Nm{
    void func(){}
}
using namespace Nm;
int main()
{
    func();
}
```
`using namespace 名前空間名`とする事でその名前空間内の全ての宣言/定義を**associated namespace**に加えます。associated namespaceは、直訳すると関連する名前空間という意味です。つまり、明示的に`::`で名前空間を示さなくてもその名前空間内を探索するという事です。名前空間全体ではなく、その名前空間の一部のデータのみを使うというようであれば、以下のように記述します。

```cpp
namespace Nm{
    void func(){}
    void func1(){}
}
using Nm::func;
int main()
{
    func();
    Nm::func1();
}
```
`using Nm::func;`とする事によって、`Nm`名前空間の`func`という関数だけを非修飾的に(名前空間を明示的に示さない事)呼べるようにしています。この時`func1`は`using`宣言されていないため、明示的に名前空間を修飾した記述が必要になります。

### 6.3.3 スコープごとのusing宣言

using宣言にも、スコープという概念はそのまま適用されます。以下のコードを見て見ましょう。

```cpp
namespace Nm{
    void f(){}
}
void func()
{
    using namespace Nm;
    f();
}
int main()
{
    func();
    f();
}
```

`Nm`名前空間の関数`f`を使うために、`func`関数内で`using namespace`宣言をしました。そして`f()`として関数を呼んでいます。しかし、その後`main`関数内で再度名前空間の明示的な修飾なしに関数`f`を呼んでいます。このコードは正しいでしょうか？残念ながら、このコードはコンパイルエラーとなります。何故かというと、簡単です。`using namespace`宣言がされたのは、関数`func`のスコープ内だけだからです。`main`関数のスコープ内では`using namespace`が適用されませんので、非修飾的に呼び出す事はできません。この場合、グローバル空間に`using namespace`宣言をしてあげる事で動きます。

```cpp
namespace Nm{
    void f(){}
}
using namespace Nm; // グローバル空間でusing namespace宣言
void func()
{
    f();
}
int main()
{
    func();
    f();
}
```
または明示的に修飾します。

```cpp
namespace Nm{
    void f(){}
}
void func()
{
    using namespace Nm;
    f();
}

int main()
{
    Nm::func(); // 明示的に修飾
    f();
}
```
これは、`using namespace`の他に、`using`宣言でも同様です。



### 6.3.4 using宣言による衝突

このコードは文法的に正しいでしょうか。

```cpp
namespace Nm{
    void f(){}
}
using namespace Nm; // or using Nm::f;
void f(){}
int main()
{
    f();
}
```

正解は正しくない、です。何故ならば`using namespace Nm;`とした事で、名前空間`Nm`をassociated namespaceに加えました。しかし、グローバルスコープにも同名、同シグネチャの関数`f`が既に定義されているのです。この場合、同じスコープに全く同じ定義が二つある事になってしまいますのでどちらの関数`f`を呼び出すべきなのか判別する事ができません。当然ですね。

このような事から、あまり不必要に`using`宣言を多用するべきではないと考えられます。あるプロジェクトの中にも、`using`宣言を禁止とするチームも存在しました。しかし、筆者としては全ての機能において言える事ですが、その機能による作用を完全に理解しているのであれば、それを利用する事自体、特に悪い事ではないと考えます。本書では、何の機能を使っているのか視覚的に理解しやすくするため、予め名前空間が設定されている機能を用いる場合は、明示的に完全修飾をした上でその機能を利用するスタイルのサンプルコードが書かれていますが、全ての事象において、名前空間の完全修飾をしなければならないというようには思いません。



### 6.3.5 inline namespace

`inline`キーワードには、関数に`inline`をキーワードを付与する事でインライン展開してほしい旨をコンパイラに伝えられる機能がありましたが、`inline`というキーワード自体は同じであるものの、これはそれとは全く別の機能です。inline namespaceはまず、以下のように使う事ができます。

```cpp
inline namespace Nm{
    void f(){}
}
int main()
{
    f();
}
```
非修飾的に関数`f`を呼び出せています。`inline namespace`は、その名前空間をassociated namespaceに含めるという意味を持ちます。そのため、非修飾的にその内部機能を呼び出す事ができるのです。しかし、これではグローバル定義されたものとなんら変わらないのでは？と思うかもしれません。実はまさにその通りで、このように`inline namespace`をグローバル領域に宣言した場合は機能として全くの意味を持ちません。`inline namespace`の機能が発揮されるのは、名前空間が二重以上にネストされた場合です。

```cpp
namespace Awesome_Lib{
    inline namespace feautures{
        void f(){}
    }
    void g(){}
}
int main()
{
    {
        using namespace Awesome_Lib; // Awesome_Lib名前空間の全ての機能を非修飾的に呼び出す
        f();
        g();
    }
    {
        using namespace Awesome_Lib::feautures; // Awesome_Lib::feautures名前空間のみを非修飾的に呼び出す
        f();
        Awesome_Lib::g(); // 修飾が必要
    }
}

```
`Awesome_Lib`という名前空間内に、`feautures`という名前空間がinline指定されています。このように機能ごとにどこまでを非修飾的に呼び出せるようにするのかをユーザーに指定させる事ができます。またその他に、バージョンマネージメントなどに利用する事ができます。

```cpp
namespace Awesome_Lib{
    inline namespace release{
        void f(){}
    }
    namespace experimental{
        void f(){}
    }
}

int main()
{
    Awesome_Lib::f(); // releaseバージョンを使用する。
    Awesome_Lib::experimental::f(); // 実験的バージョンを使用する。
}
```
`release`名前空間を`inline namespace`にする事で、単に`Awesome_Lib`名前空間だけを修飾すると、`release`をデフォルト設定として機能を使うように出来、明示的に`experimental`名前空間を指定した場合のみ、まだ実験的なバージョンを使えるようにするというような、バージョンごとのマネージメントをする事ができます。

### 6.3.6 名付けてはならない名前空間名

名前空間にも名付けてはならない名前が存在します。基本的には識別子につけてはならない名前と同等ですが、名前空間名を名付けるにおいてはそれに一つ条件が加わります。それは、`std`名前空間を独自に定義してはならないという要件です。`std`名前空間には、規格で厳密に定められた言語機能のみが定義されていなければなりません。`std`名前空間に、独自に機能を追加した場合は規格の保証外(未定義)となりますので絶対にしてはならないのです。しかし、ある条件の場合のみ、新たに`std`名前空間に機能を追加して良い事ともなっています。それについては、「第11章テンプレート」にて説明します。

### 6.3.7 無名名前空間
無名の名前空間を定義する事もできます。

```cpp
namespace{
 void f(){}
}
int main()
{
    ::f();
    f();
}
```
無名の名前空間にアクセスするためには`::`を記述する事で無名名前空間を明記する事ができます。この機能は「6.2.6 static指定子の別のセマンティック」で紹介した機能と同じです。無名名前空間内に定義されたオブジェクトなどへはそのファイル外からアクセスする事はできなくなります。

```cpp
// test.hpp
namespace{
    void f(){}
}
```

```cpp
#include"test.hpp"
int main()
{
    f(); // 不可
    ::f(); // 不可
}
```