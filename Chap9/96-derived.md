# 9.6 継承/派生

この項では、今まで学習してきたクラスの概念に加えて継承という新しい概念を取り入れていきます。継承という概念によって既存のクラスの再利用性を高めたり、継承させたクラスに対して一定の強制力を持たせる事ができます。しかしむやみやたらに継承すれば、ただ複雑になるだけで利点を活かす事ができません。効果的に継承を使えるように、しっかりと本項目で学んでいきましょう。

## 9.6.1 継承の基本構文とアクセスレベル
まずは継承をどのように記述するのか見てしまいましょう。以下のコードを見てください。
```cpp
struct X{
    void f(){std::cout<<__func__<<std::endl;}
};

struct Y:public X{}; // クラスXを継承したクラスYを定義
```
継承を行うには、上記の通り、`struct`もしくは`class`でクラス名を宣言した後に`:`を記述し、その後アクセスレベルを付与し、継承するクラス名を付与します。継承を行ったクラスをインスタンス化して使って見ましょう。
```cpp
#include<iostream>

struct X{
    void f(){std::cout<<__func__<<std::endl;}
};

struct Y:public X{};

int main()
{
    Y y;
    y.f();
}
```
クラス`Y`には`f`という関数を定義していませんが、呼び出せています。継承によって、`X`から`f`を引き継いでいるので、`f`を利用する事ができるのです。以下、用語の紹介まで上記の場合のクラス`X`を派生元クラス、クラス`Y`を派生先クラスと呼称します。尚、派生先クラスの定義を記述する場合、その記述する位置から派生元クラスの定義が見えていなければなりません。つまり、派生先クラスの定義において、派生元クラスの前方宣言だけではプログラムとしては不適合となります。
```cpp
struct X; // クラスの前方宣言

struct Y:public X{ // NG
	// ...
};
```
ところで、継承した際に指定したアクセスレベルは、一体どのような効果を発揮するのでしょうか。
```cpp
struct X{
    int x=10;
};

struct Y:private X{}; // 継承時のアクセスレベルをprivateにしてみる

int main()
{
    Y y;
    y.x=20;
}
````
このコードはコンパイルに失敗します。エラー文では内容として派生元のクラスに対して`private`である事が示されています。つまり、**継承時のアクセスレベルの設定は、継承先から継承元を見た時に、どのようなアクセスレベルとして捉えるかを設定するものである**かという事です。この場合、`private`に設定しているため、継承先のクラス`Y`からは`X`のメンバーが全て`private`アクセスレベルであると認識するのです。よってクラス`Y`のインスタンスからは、`X`の内部メンバにアクセスする事ができないのです。しかし、`private`継承を行なっても、以下のように、継承先の内部で、継承元のメンバにアクセスする事は可能です。
```cpp
struct X{
    int x=10;
};

struct Y:private X{
    constexpr int get()const noexcept{return x;} // 継承先のメンバからはアクセス可
};

int main()
{
    Y y;
    y.get();
}
```
ところでアクセスレベルは、`public`、`private`の他に、`protected`というものがあります。`protected`は、自身と、継承先にのみメンバを公開する事を意味するアクセスレベルです。
```cpp
struct X{
    int a; // 全てに公開
protected:
    int b; // 自身と継承先に公開
private:
    int c; // 自身にのみ公開

    // X::a,X::b,X::cの全てにアクセスできる
};

struct Y:public X{
    // X::a、X::bにアクセスできる
};

int main()
{
    // X::aを操作できる    
}
```
継承先に対するアクセスレベルにも`protected`を指定する事ができます。`protected`を指定した場合、派生先からは派生元の`public`、`protected`は`protected`に、`private`は`private`アクセスレベルとして捉えます。
```cpp
struct X{
    int a;
protected:
    int b;
private:
    int c;
};

struct Y:protected X{
    // X::a,X::bにアクセスできる
};

int main()
{
    X x;
    x.a;  // Xのインスタンスからはaにアクセスできる

    Y y;
    y.a: // エラー！Yのインスタンスからはaはprotectedとして捉えられるためアクセスできない
}
```
尚、継承時にアクセスレベルを指定しない事もできます。
```cpp
struct W{};
struct X:W{}; // public継承

class Y{};
struct Z:Y{}; // private継承
```
その場合、派生元が`struct`キーワードを用いてクラスを定義しているか`class`キーワードを用いてクラスを定義しているかでその意味は異なります。**派生元クラスが`struct`キーワードで宣言されたクラスであり、そのクラスに対してアクセスレベルを指定せずに継承した場合、`public`継承となり**、**派生元クラスが`class`キーワードで宣言されたクラスであり、そのクラスに対してアクセスレベルを指定せずに継承した場合`private`継承となります**。


ところで、継承したクラスのコンストラクタ、デストラクタはどのように動作するのでしょうか。継承元にデータメンバを保持していた場合、継承元のデータメンバも正しく生成、破棄される必要がありますから継承元のクラスのコンストラクタやデストラクタも動作しそうです。実際に確認してみましょう。
```cpp
#include<iostream>

struct X{
    explicit X(){std::cout<<__func__<<std::endl;}
    ~X(){std::cout<<__func__<<std::endl;}
};

struct Y:X{
    explicit Y(){std::cout<<__func__<<std::endl;}
    ~Y(){std::cout<<__func__<<std::endl;}
};

int main()
{
    Y();
}
```
実行結果は以下の通りです。
```cpp
X
Y
~Y
~X
```
まず派生元のクラスのコンストラクタ、次に派生先のクラスのコンストラクタ、破棄時にはまず派生先のクラスのコンストラクタ、次に派生元のクラスのデストラクタが呼ばれます。このように、**コンストラクタとデストラクタの呼び出し順序は生成と破棄で逆順になります**。

最後に用語をまとめておきましょう。継承元となるクラスのことを**スーパークラス（親クラス、基本クラスなど）**と言い、継承を行うクラスを**サブクラス（子クラス、派生クラスなど） **といいます。


## 9.6.2 継承コンストラクタ
以下のコードを見てください。
```cpp
#include<utility>

struct X{
    explicit X()=default;
    constexpr X(int x):a(std::move(x)){}
protected:
    int a;
};

struct derived:X{
    derived(int x)
    {
        a=std::move(x);
    }
};

int main()
{
    derived d=42;
}
```
クラス`X`は`a`というメンバを`protected`アクセスレベルで持っています。それを継承して利用する`derived`クラスを後に定義していますが、ここで`derived`のコンストラクタを見てください。`derived`のコンストラクタで`X`内のメンバ`a`を初期化したいところですが、上記のコードでは初期化ではなく、代入操作を行なっています。
ここで、
```cpp
derived(int x):a(std::move(x))
{
// ...
}
```
とすれば初期化できるのではないかと思うかも知れませんがこのように記述する事はできません。何故ならば`a`は`derived`クラスのメンバーではないからです。

では継承元のクラスのメンバーは初期化が行えないのでしょうか。勿論、そんな事はありません。継承元のコンストラクタを派生クラスから呼び出せば実現できます。
```cpp
#include<utility>

struct X{
    explicit X()=default;
    constexpr X(int x):a(std::move(x)){}
protected:
    int a;
};

struct derived:X{
    derived(int x):X(std::move(x)){} // 継承元であるXのコンストラクタに引数xを受けわたす。
};

int main()
{
    derived d=42;
}
```
この時、継承元のコンストラクタで継承元の内部にあるメンバについての初期化が行えるコンストラクタが定義されていない場合継承、元のメンバをその派生クラスから初期化する事はできません。例えば、継承元のクラスが以下のような実装であった場合、継承先から継承元のクラス`X`におけるメンバ変数`a`の初期値を設定する事はできません。
```cpp
// aに初期値を与える術がない
struct X{
    explicit X()=default;
protected:
    int a;
};
```
さて、上記の通り継承元のクラスのコンストラクタを明示的に呼び出す事で、継承元のコンストラクタに値を受け渡す事で継承元のメンバに初期値を与えていましたが、継承先へ初期値を受けわたすこの記述はとてもよく行うものであるので、以下のように初期値を受け渡して初期化させるコンストラクタを暗黙宣言させる事ができます。この機能を**継承コンストラクタ**と言います。
```cpp
#include<utility>

struct X{
    explicit X()=default;
    constexpr X(int x):a(std::move(x)){}
protected:
    int a;
};

struct derived:X{
    using X::X; // 継承コンストラクタ
    // derived(),constexpr derived(int)が暗黙宣言される
};

int main()
{
    derived d=42;
}
```
継承コンストラクタの機能によって暗黙宣言されるコンストラクタは上記コメントの通り、継承元クラスのコンストラクタが`constexpr`で宣言されている場合、継承先もまた`constexpr`で暗黙宣言されます。つまり、これら全ての機能を受けつぐ形で暗黙宣言されます。
また、継承元コンストラクタが`delete`指定されており、それを継承コンストラクタの機能で暗黙宣言させた場合、それもまた同じく受け継ぐ形で、`delete`指定されて暗黙宣言されます。

因みに、このような文法を**糖衣構文(Syntax sugar)**と言ったりします。ある機能がなくとも実現は可能であるが、文法によるサポートで簡易的に記述できるような構文を、このように言います。

## 9.6.3 隠蔽
基底クラスにあるメンバ関数と同じ名前のメンバ関数を派生クラスで定義すると、 基底クラスのメンバ関数は隠されてしまいます。 これを、隠蔽と呼びます。
```cpp
#include<iostream>

struct X{
    void f(){std::cout<<"X::"<<__func__<<std::endl;}
};

struct derived:X{
    void f(){std::cout<<"derived::"<<__func__<<std::endl;}
};

int main()
{
    derived d;
    d.f();
}
```
実行結果は以下の通りです。
```cpp
derived::f
```
また、Derivedクラスのメンバ関数内から何のスコープ解決を行わずに呼び出した場合も、自分自身が持っている derived::fメンバ関数を呼び出します。
```cpp
#include<iostream>

struct X{
    void f(){std::cout<<"X::"<<__func__<<std::endl;}
};

struct derived:X{
    void f(){std::cout<<"derived::"<<__func__<<std::endl;}
    void g(){f();} // スコープ解決なしでfを呼び出し
};

int main()
{
    derived d;
    d.g();
}
```
実行結果は以下の通りです。
```cpp
derived::f
```
ただし、スコープ解決を行う事で隠蔽された基底クラスのメンバを呼び出す事は可能です。
```cpp
#include<iostream>

struct X{
    void f(){std::cout<<"X::"<<__func__<<std::endl;}
};

struct derived:X{
    void f(){std::cout<<"derived::"<<__func__<<std::endl;}
    void g(){X::f();} // スコープ解決によってX::fを呼び出し
};

int main()
{
    derived d;
    d.g();
}
```
実行結果は以下の通りです。
```cpp
X::f
```
尚、隠蔽しても例えばそれらの関数がオーバーロード可能な関数であれば以下のようにして継承元から引き出す事も可能です。
```cpp
#include<iostream>

struct X{
    void f(int)
    {
        std::cout<<"X::"<<__func__<<std::endl;
    }
};

struct derived:X{
    using X::f; // 隠蔽されたX::fを宣言させる
    void f(double)
    {
        std::cout<<"derived::"<<__func__<<std::endl;
    }
};

int main()
{
    derived d;
    d.f(42);
    d.f(4.2);
}
```
実行結果は以下の通りです。
```cpp
X::f
derived::f
```
`using X::f`という記述は継承コンストラクタの記述と同じです。継承コンストラクタの説明では、コンストラクタだけを`using`させて引き出していましたが、このようにメン関数も`using`して機能を引き出す事ができます。

さて、このような隠蔽の機能を使うか否かは果たしたい機能によって適切であるかどうかを考慮する必要がありますが、上記のように`public`継承している場面では、隠蔽してしまうとその意味を成さない事になってしまう事が多いです。その理由は、本章を順に読み進めていく事で理解できるようになるでしょう。


## 9.6.4 継承を禁止する
継承を禁止させる事も可能です。以下のように記述します。
```cpp
struct X final{
// ...
};
```
クラスの宣言時に`final`キーワードを用いる事でそのクラスを継承する事が禁じられます。クラスの宣言時に`final`キーワードが付与されたクラスを継承しようとすると
```cpp
struct X final{};

struct derived:X{};
```
GCC 7.1.0では以下のようなエラー文を出力しました。
```cpp
error: cannot derive from 'final' base 'X' in derived type 'derived'
 struct derived:X{};
        ^~~~~~~
```

## 9.6.5 スライシング
まず定義として、派生クラスは、基底クラスよりサイズが大きくなります。これは、派生クラスは、基底クラスのサイズに追加のメンバ変数うを持っている可能性があるため、断定できる条件なのです。つまりクラス`Base`という基底クラスからクラス`Derived`が派生クラスとして定義されていた場合、両者の関係は必ず`sizeof(Base) <= sizeof(Derived)`が`true`となります。

継承関係にあるクラス同士は、型変換が効くので以下のようなコードを記述する事もできます。
```cpp
struct Base{};

struct Derived:Base{};

int main()
{
    Base base=Derived(); // スライシング。派生クラスのインスタンスで基底クラスを初期化
}
```
しかし、このように基底クラス型のオブジェクトを派生クラスのオブジェクトで初期化すると、基底クラスは派生クラスの全ての情報を扱う事ができないため(派生クラスは基底クラスよりもサイズが大きい)、派生クラスに固有な情報が失われてしまいます。これを、**スライシング**と言います。

スライシングは、意図せず記述した場合明らかにバグとなりますので防止する必要がありますが、ここでの問題は実態として派生クラスのオブジェクトで初期化するために起きていますから、この場合基底クラスのオブジェクトではなく派生クラスのオブジェクトで派生クラスのオブジェクトを初期化する必要があります。

尚、ポインターや参照の場合は問題ありません。単に指し示すだけなので、データそのものが失われる事はないのです。
```cpp
struct Base{};

struct Derived:Base{};

int main()
{
    Base* ptr=new Derived();
    delete ptr;

    const Base& ref=Derived();
}
```
なぜわざわざ基底クラス型のポインターや参照で派生クラスのオブジェクトをポイントしておくのか、その利点については「9.6.7 仮想関数とオーバーライド」で明らかとなりますのでそれは一旦置いておくとして、実はこのように基底クラス型のポインターや参照として派生クラスをポイントしている場合でも、スライシングとはまた別の問題が発生してしまっているのです。何が起きてしまっているのかは、デストラクタの挙動を追いかける事で判明します。
```cpp
#include<iostream>

struct Base{
    ~Base(){std::cout<<__func__<<std::endl;}
};

struct Derived:Base{
    ~Derived(){std::cout<<__func__<<std::endl;}
};

int main()
{
    Base* ptr=new Derived();
    delete ptr;
}
```
実行結果は以下の通りです。
```cpp
~Base
```
なんと、派生クラスのデストラクタが呼び出されないのです。指し示している実態は`Derive`のインスタンスですが、delete の対象はポインターの型`Base`を元に動作するため、基底クラス側のデストラクタだけが呼び出される動作となってしまうのです。このような問題が起きないためには、基底クラスのポインター型でヒープ上に生成された派生クラスのオブジェクトをそもそも指し示さなければ良いのですが、これから解説する基底クラスのポインターで派生クラスのインスタンスを管理する事の利点を踏まえると、どうも捨てがたい機能ではあるのです。
この問題は、**仮想関数**という機能を用いる事で回避する事が可能です。

## 9.6.6 仮想関数とオーバーライド

まずは、以下のコードを見てみましょう。尚、`virtual ~dog()=default;`については現段階では気にしなくて大丈夫です。
```cpp
#include<iostream>

struct dog{
    void call(){std::cout<<"bowwow"<<std::endl;}
    virtual ~dog()=default;
};

struct cat:dog{
    void call(){std::cout<<"mew"<<std::endl;}
};

int main()
{
    dog* ptr1=new dog(); // #1
    ptr1->call();
    
    dog* ptr2=new cat(); // #2
    ptr2->call();
    
    delete ptr1;
    delete ptr2;
}
```
実行結果は以下の通りです。
```cpp
bowwow
bowwow
```
このコードは全くもってよくありません。#1では`dog`クラスをヒープ上にインスタンス化しその後`call`関数を呼び出しているため、`bowwow`と出力されるのは意図した動作です。しかし、その後の#2で、`cat`クラスで定義されている`cat::call`関数を呼び出したいところですが、出力からわかるように、`dog::call`を呼び出してしまいました。何故このようになってしまうかというと、型はあくまで`dog*`であり、そこには継承先である`cat`の型情報が全くないため、実際のオブジェクトによって呼び分けたりする事ができないためです。これを意図した通りに動作させるためには、基底クラスの`dog::call`を**仮想関数**として定義します。
```cpp
#include<iostream>

struct dog{
    virtual void call(){std::cout<<"bowwow"<<std::endl;} // callを仮想関数として定義する
    virtual ~dog()=default;
};

struct cat:dog{
    void call(){std::cout<<"mew"<<std::endl;}
};

int main()
{
    dog* ptr1=new dog();
    ptr1->call();
    
    dog* ptr2=new cat();
    ptr2->call();
    
    delete ptr1;
    delete ptr2;
}
```
実行結果は以下の通りです。
```cpp
bowwow
mew
```
出力の通り、意図した通りに動作しました。このように、関数宣言時に`virtual`キーワードが付与された関数を**仮想関数**と言います。仮想関数を定義すると何が起きるのかというと、**仮想関数テーブル**というものがコンパイラによって暗黙的に生成されます。仮想関数テーブルは、ある仮想関数を呼び出した時に、実際の呼び出し元のオブジェクトの型によって、どの関数が呼ばれるのかの情報が書き込まれているものです。プログラマには直接的に見えない、クラスの隠しメンバ変数としてその情報を持つようになります。`virtual`キーワードの付与によって仮想関数を宣言し、仮想関数テーブルが作成される事で、実際のオブジェクトによってそのオブジェクトの型が持つメンバ関数を呼び出す事ができるのです。これを関数の**オーバーライド**と言います。尚、基底クラスの関数を仮想関数として宣言し、それを派生クラスの関数でオーバーライドしている時、派生クラスのオーバーライドした関数も仮想関数として定義されます。よって以下のように、オーバーライドする関数の宣言で`virtual`を付与する事もできます。
```cpp
struct dog{
    virtual void call(){std::cout<<"bowwow"<<std::endl;} // callを仮想関数として定義する
    virtual ~dog()=default;
};

struct cat:dog{
    virtual void call(){std::cout<<"mew"<<std::endl;} // virtualを明記。動作と意味は全く変わらない
};
```
また全く動作は変わりませんが、`virtual`キーワードを敢えて明記した方が良いとする考え方もあり>ます。
何故このような考え方があるかといえば、派生クラスの該当関数が仮想関数としてオーバーライドしているのかそのシ
グネチャだけからは判断する事が出来ないためです。しかし、もしそのような目的で`virtual`キーワードを用いるのであれば、`override`というキーワードを用いて関数宣言するべきです。
```cpp
struct Base{
    virtual ~Base(){std::cout<<__func__<<std::endl;}
};

struct Derived:Base{
    ~Derived()override{std::cout<<__func__<<std::endl;} // overrideキーワードによってオーバーライドしてい>る旨を明記する
};
```
`override`キーワードは、基底クラスにある仮想関数をオーバーライドしているという旨を明記するためのキーワードであり、関数の仮引数リストの後に記述します。

ところで、先程、「プログラマには直接的に見えない、クラスの隠しメンバ変数として情報を持つ」と言いました。という事は、その分クラスのサイズが増加するのではと思うかも知れません。まさにその通りで、仮想関数が定義されたクラスはサイズがその分増加します。
```cpp
#include<iostream>

struct X{
    virtual void f(){} // 仮想メンバ関数
};

struct Y{
    void f(){} // 仮想でないメンバ関数
};

int main()
{
    std::cout<<sizeof(X)<<std::endl;
    std::cout<<sizeof(Y)<<std::endl;
}
```
筆者の環境では実行結果は以下の通りです。
```cpp
8
1
```
よって、仮想関数テーブルの機能を特別使わないのであれば、仮想関数を宣言する事は無駄であると言えます。



それでは漸くですが、9.6.5の最後で提示した問題、デストラクタが呼ばれないという問題に対する対処を行いましょう。問題は以下のようなコードでデストラクタが呼ばれないというものでした。
```cpp
#include<iostream>

struct Base{
    ~Base(){std::cout<<__func__<<std::endl;}
};

struct Derived:Base{
    ~Derived(){std::cout<<__func__<<std::endl;}
};

int main()
{
    Base* ptr=new Derived(); 
    delete ptr; // ~Baseのデストラクタしか呼ばれない
}
```
この問題は、デストラクタが仮想関数として定義されておらず、オーバーライドができない事によって起きているのです。つまり、先程まで述べてきたメンバ関数を`virtual`にした場合と同じように、基底クラスのデストラクタを`virtual`宣言する事でこの問題を回避できます。
```cpp
#include<iostream>

struct Base{
    virtual ~Base(){std::cout<<__func__<<std::endl;}
};

struct Derived:Base{
    ~Derived(){std::cout<<__func__<<std::endl;}
};

int main()
{
    Base* ptr=new Derived();
    delete ptr;
}
```
実行結果は以下の通りです。
```cpp
~Derived
~Base
```
このようなデストラクタを**仮想デストラクタ**と呼びます。派生クラスのシグネチャからオーバーライドしている事を明記するために`override`というキーワードを用いて関数を宣言/定義するのも良いでしょう。
```cpp
struct Base{
    virtual ~Base(){std::cout<<__func__<<std::endl;}
};

struct Derived:Base{
    ~Derived()override{std::cout<<__func__<<std::endl;}
};
```


継承の
vtable

## 9.6.7 多重継承、継承パターン、合成
is-a、has-a

## 9.6.8 EBO最適化
