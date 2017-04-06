# 9.3 static member
この項では、`static`なクラスメンバーについて説明します。

## 9.3.1 static memberを使う
クラス内の特殊メンバ関数以外のメンバーには`static`を付与する事ができます。`static`が付与されたメンバーはその性質通り、静的領域にアドレスが置かれます。つまり、そのクラスのオブジェクトとしてインスタンス化しなくともメモリ上に置かれるため、直接操作を行う事が可能です。
```cpp
#include<iostream>

struct X{
    static int a;
    static void f(){}
};

int X::a=42; // out-lineで定義が必要。初期化を行わなかった場合、静的領域の性質がそのまま適用されるため0で初期化される。


int main()
{
    std::cout<< X::a <<std::endl;
    X::f(); // インスタンス化せずとも直接呼び出せる    
}
```
実行結果は以下となります。
```cpp
42
```
`static`なメンバ変数は一度クラススコープ内で宣言した後、このようにそのクラススコープ外で定義を記述する必要があります。

また、`static`なメンバー関数はインスタンス化後にメモリに配置される領域に対してアクセスする事はできません。
```cpp
struct X{
    int a;
    static void assign(int x){x=a;} // エラー！staticでないメンバ変数をstaticメンバ関数で操作できない
};
```
メモリに配置される順序から考えるとこの挙動は自然ですね。

また、`static`な性質上、`static`データメンバを持つクラスを複数インスタンス化しても、実体は一つしかないので、データメンバは共有されます。
```cpp
#include<iostream>

struct X{
    static int a_;
};

int X::a_=42;


int main()
{
    X x1,x2;
    x2.a_=52;
    std::cout<<x1.a_<<std::endl;
}
```
実行結果は以下の通りです。
```cpp
52
```
`x1.a_`というような操作は一切行っていませんが、`X`の`x1`とは別のインスタンスである`x2`から`a_`を操作した結果、`x1`から`a_`にアクセスしても`x2`による操作が反映されているように見えます。実際は、複数のインスタンス化が行われたところで、`static`なメンバ変数はメモリ上にある同一の一箇所を示していますので、このような動作となります。

`static`なデータメンバを定義するシーンとしては、そのクラススコープで囲む事で、そのコードのセマンティックを高めたり、データの共有をインスタンス化された個体がお互いに必要とする時が主です。
例えば、別々の同じ型からインスタンス化されたオブジェクトが、今自分の型からインスタンス化されたオブジェクトが幾つあるのかカウントするなら、以下のように記述できます。
```cpp
#include<iostream>

struct X{
    explicit X()
    {
        std::cout<<__func__<<":constructed"<<std::endl;
        ++i;
    }
    ~X()
    {
        std::cout<<__func__<<":destructed"<<std::endl;
        --i;
    }
    static unsigned int i;
};

unsigned int X::i;

int main()
{
    X* x1=new X(),*x2=new X();
    std::cout<< x1->i <<std::endl;
    std::cout<< x2->i <<std::endl;

    delete x2;
    x2=nullptr;
    std::cout<< x1->i <<std::endl;

    x2=new X();
    X* x3=new X();

    std::cout<< x1->i <<std::endl;
    std::cout<< x2->i <<std::endl;
    std::cout<< x3->i <<std::endl;

    delete x1;
    delete x2;
    delete x3;
}
```
実行結果は以下の通りです。
```cpp
X:constructed
X:constructed
2
2
~X:destructed
1
X:constructed
X:constructed
3
3
3
~X:destructed
~X:destructed
~X:destructed
```

## 9.3.2 名前解決に纏わる注意

staticメンバーをスコープ外で定義する時、名前解決の際に、コンパイラの少しだけ反直感的な解釈に遭遇する事があるかもしれません。例えば、以下のような場合などです。

```cpp
struct X{};
struct C{
    struct X{};
    static constexpr std::size_t number=50;
    static X ar[number];
};

X C::ar[C::number]={};
```

このコードはコンパイルが通りません。エラー文\(GCC 7.0.1\)は以下のように出力されます。

```
error: redefinition of 'ar' with a different type: 'X [50]' vs 'C::X [50]'
X C::ar[C::number]={};
     ^
note: previous declaration is here
    static X ar[number];
             ^
```

`ar`をredifinition\(再定義\)してしまっている、つまりODR違反を指摘されているわけです。何故でしょうか？`X C::ar[C::number]={};`というところでは、`C`スコープ内の`ar`について定義しているはずで、再定義しているわけではない...ように見えます。

しかし、残念ながら、これは間違い無く再定義なのです。何故ならば、**宣言された識別子が、::によって修飾された宣言では、修飾する型が宣言される前に使用されていた名前が定義されている名前空間のスコープ内で検索され、 修飾された型に続く識別子は、メンバーのクラスまたは名前空間のスコープ内で検索される**からです。

つまり、`X C::ar[C::number]`は以下と同じように解釈されるのです。

```cpp
::X C::ar[C::number]={};
```

決して

```cpp
C::X C::arr[C::number];
```

ではないのです。

つまり、`X C::ar[C::number]`は、グローバルスコープ上の`X`型として`C::ar`を初期化しようとしていますが、初期化対象の`C::ar`は上記の規則に従って、そのクラススコープ内で`X`を検索しようとします。よって、`C::ar`の型は`::X`ではなく`C::X`となります。

```cpp
struct X{};
struct C{
    struct X{};
    static constexpr std::size_t number=50;
    static X ar[number]; // このXはC::X
};

X C::ar[C::number]={}; // このXは::X
```

これでは、`X C::ar[C::number]={};`は`C::ar`の初期化文として捉えることができません。よってそれは、グローバルスコープ上で`C::ar`を型`::X`として定義している文であると捉えられます。その時点で、`C::ar`という識別子名を同じスコープ内で定義しようと解釈され、再定義、つまり、ODR違反となってしまうのです。

この場合、`C::ar`を`::X`型のstaticオブジェクトとして定義できないじゃないか！と思うかもしれませんが、そんなことはありません。以下のようにすれば、目的通りに動きます。

```cpp
struct X {};
struct C {
    struct X{};
    static constexpr int number = 50;
    static ::X ar[number]; // グローバルスコープのXであることを明示
};

X C::ar[C::number]={}; 
```

こうすると、`X C::ar[C::number]={};`は`C::ar`についての正しい初期化文となるため、再定義している?!とコンパイラに解釈されることもなくコンパイルが通ります。

