# 8.1 enum
enumは列挙型とも言われます。enumを用いることである一定の値しかとることのできない型を作る事ができるのです。文法を以下に示します。
```cpp
enum 識別子名 スコープ{
    enumerator-list
}オブジェクト識別子;
```
このように記述する事で、その識別子名のenum型を作る事ができます。

## 8.1.1 enumの基本概念
まあまあ取り敢えず、以下のコードを見てみましょう。
```cpp
enum Color{
    Red,
    Green,
    Blue
};

void print(Color cl)
{
    std::cout<<cl<<std::endl;
}

int main()
{
    print(Red);
    print(Green);
    print(Blue);
}
```
実行結果は以下となります。
```cpp
0
1
2
```
`Color`という名前の`enum`を定義し、そのenumerator-listに`Red`、`Green`、`Blue`と定義しています。この時、enumerator-listに定義した順に、0 1 2 3 4...と各enumeratorに定義されます。よって、`Red`は`0`、`Green`は`1`、`Blue`は`2`となります。そして関数`print`の引数型には、`Color`というenum型を指定しています。よってこの関数に渡せるデータは、`Color`型のオブジェクトのみとなります。
以下のように、`Color`型以外の値は、渡す事ができません。
```cpp
enum Color{
    Red,
    Green,
    Blue
};

void print(Color cl)
{
    std::cout<<cl<<std::endl;
}

int main()
{
    print(42); // 42はColor型の値ではないため呼び出せない。エラー
}
```
さて、上記のコードは`enum`の機能をまだ使い切っていません。次のサンプルコードを見てみましょう。
```cpp
#include<iostream>
enum Param{
	A=1,B,C,D=2,E,F
};

void print(Param cl)
{
	std::cout<<cl<<std::endl;
}

int main()
{
	print(A);
	print(B);
	print(C);
	print(D);
	print(E);
	print(F);
}
```
実行結果は以下となります。
```cpp
1
2
3
2
3
4
```
enumerator-listの中で`=1`だとか`=2`だとか指定されています。実行結果と照らし合わしてみると挙動が分かるかもしれません。`A`には`1`を指定しています。よって`B`は`2`、`C`は`3`です。その後なにも指定されなければ、`D`は`4`になりますが、`D`には`2`と指定されています。よって、`D`は`2`となり、`E`は`D`からの列挙値として`3`、`F`は`4`となるのです。

さてさてどんどん行きましょう。次のサンプルコードを見てください。
```cpp
#include<iostream>
enum Param{
	A=1,B,C
}par; // #1

void print(const Param& cl) // 勿論const参照にする事もできる。
{
	std::cout<<cl<<std::endl;
}

int main()
{
	par=A; print(par);
	par=B; print(par);
	par=C; print(par);
	//par=42; は42がParam型でないため代入できない。エラーとなる。
	
	Param par1=A; print(par1); // #2
	// par1=B; print(par1); ... 略
	
	enum Param1{ // #3
		D,E,F
	};
}
```
実行結果は以下となります。
```cpp
1
2
3
1
```
注視してほしいところに、それぞれマークをつけました。まず　#1についてですが、`enum Param`と定義した`;`の前に、`par`という記述がありますね。ここに識別子を記述すると、その`enum`のオブジェクトをその場で定義する事ができます。この時`par`は、グローバルスコープに定義されます。その`par`に対して`main`関数で`Param`のenumerator-listを順次代入して出力させています。

次に#2ですが、このように`型 識別子;`というような今までの書き慣れた方式で勿論`enum`のオブジェクトを定義する事が可能です。それをenumerator-listにある`A`で初期化し出力させています。

最後に#3です。なんと、関数内で`enum`を定義しています。これも、正しいコードです。このように関数内で`enum`を定義した場合、その`enum`はその関数内でしか使う事ができません。「第5章 スコープと制御文」でも述べた、スコープの概念を基に考えると、自然に思える挙動です。

さて、次がこの項の最後のサンプルコードです。見て行きましょう。
```cpp
#include<iostream>
enum Param{
	A,B,C
};

void print(int p)
{
	std::cout<<p<<std::endl;
}

int main()
{
	print(A);
	print(B);
	print(C);
}
```
実行結果は以下となります。
```cpp
0
1
2
```
`Param`という`enum`を定義しています。`enum`内に定義されたenumerator-listは、その`enum`でしか受け取れないはず...ですが、なんと関数`print`は`int`を受け取る関数であるにも関わらず呼び出せています。何故でしょうか？

この理由は単純で、**スコープなしの`enum`は整数型(`signed`、`unsigned`関わらず)に暗黙的に変換する事が可能だからです**。暗黙的に変換が可能な特徴は、よく覚えておきましょう。`enum`を使った時に、思いもよらぬ関数がオーバーロード解決されたら、もしかするとこのような暗黙変換が働いている可能性があるからです。ところで、**スコープなしの**とはなんなのでしょうか。`enum`にスコープ？そんなものがあるのでしょうか。

実は、スコープを付与する事もできるのです。

## 8.1.2 スコープ付きenum
enumは以下のように使えると述べました。
```cpp
#include<iostream>
enum Param{
	A,B,C
};
int main()
{
	std::cout<< A <<" "<< B <<" "<< C <<std::endl; // 0 1 2
}
```
ここまでで、既にお気づきになったかもしれませんが、グローバル領域に定義された`enum`はそのenumerator-listもグローバル領域となります。よって、`enum`自体が異なる識別子だとしてもenumerator-listが同名だった場合、ODRに違反してしまうのです。
```cpp
enum Param{
	A,B,C
};
enum Param1{ // Param1というようにenumの識別子は異なるが...
	A,B,C // enumerator-listは同名なため判別出来ずエラー
};
```
これを回避するためには、まず一つ目の策として`namespace`を使うという手があります。
```cpp
namespace nm{
	enum Param{
		A,B,C
	};
}

namespace nm1{
	enum Param1{
		A,B,C
	};
}

int main()
{
	nm::Param pm=nm::A; // 名前空間で完全修飾
	nm1::Param1 pm1=nm1::A; // 名前空間で完全修飾
}
```
しかし、この方法は代替的な案であり、根本的な解決とはなっていません。私たちが示したいのはどの名前空間のenumerator-listかではなく、どの`enum`のenumerator-listかなのです。つまり、以下のように記述したいわけです。
```cpp
enum Param{
	A,B,C
};
enum Param1{
	A,B,C
};
int main()
{
	Param::A; // ParamのA
	Param1::A; // Param1のA
}
```
これを実現させるのはとても簡単です。以下のように記述します。
```cpp
enum class Param{
	A,B,C
};
enum struct Param1{
	A,B,C
};
int main()
{
	Param::A;
	Param1::A;
}
```
このように、`enum`の後に`class`、もしくは`struct`キーワードを付与する事で、その`enum`がスコープを持つ`enum`として定義されます。`class`と`struct`、どちらのキーワードを使っても差異はありません。
このようにスコープを持つ`enum`のことをscoped enumeration type、スコープを持たない`enum`のことをunscoped enumeration typeと言います。そのままですね。

スコープを持つ`enum`、scoped enumeration typeにはいくつか特徴があります。例えば、先ほどまで述べていたunscoped enumeration typeでは整数型への暗黙の変換が許されていました。
```cpp
enum Param{
	A
};
int main()
{
	int a=A; // OK
}
```
しかし、scoped enumeration typeでは、このような暗黙の変換は許されません。
```cpp
enum class Param{
	A
};
int main()
{
	int a=Param::A; // Bad. エラー
}
```
また、unscoped enumeration typeでは、enumerator-listに明示された識別子に対して定義される値の型を明示する事はできませんでした。scoped enumeration typeではその基底型を指定する事ができます。
```cpp
#include<iostream>

enum class Param:unsigned int{ // 基底型をunsigned intにする
	A=-1,B,C // エラー！Aがunsignedではない。
};

void print(Param p)
{
	std::cout<<static_cast<unsigned int>(p)<<std::endl;
}

int main()
{
	print(Param::A);
	print(Param::B);
	print(Param::C);
}
```
このように基底型を指定できる事で、誤った値を設定してしまう事を防ぐ事ができる他に、どのような値を扱うのか型を明記する事でその明示的にする事ができるのです。

scoped enumeration typeの最後の特徴としては、前方宣言ができる事です。unscoped enumeration typeでは、前方宣言をする事ができません。
```cpp
#include<iostream>
enum class Param:unsigned int; // 前方宣言

void print(Param p)
{
	std::cout<<static_cast<unsigned int>(p)<<std::endl; // 整数型への暗黙変換は効かないため明示的にキャストする
}

enum class Param:unsigned int{
	A,B,C
};

int main()
{
	print(Param::A);
	print(Param::B);
	print(Param::C);
}
```
実行結果は以下となります。
```cpp
0
1
2
```
このように、scoped enumeration typeはunscoped enumeration typeに比べて出来ることが多いですし、型についての厳格性や意味論を深めるだけでなく、名前衝突が回避できたりと、とても優れた面が多いです。可能な限り、scopedなenumerationを用いることをお勧めします。

## 8.1.3 無名enum
無名な`enum`を定義する事も勿論可能です。
```cpp
enum{value}; // 無名enum

int main()
{
	value; // 0
}
```
無名`enum`はunscoped enumeration typeの特性に加えて型情報も一切含まない形態です。一般的に無名`enum`は、値が定数である事を強調したい時に用いられるシーンが多いです。例えば、以下のように。
```cpp
int main()
{
	enum{size=42;}
	int ar[size];
}
```
配列の定義時、指定する要素数は定数でなければなりませんが、`enum`はまさに定数ですので、この要件を満たしているのです。

## 8.1.4 enumの文法についてもう一言

### オブジェクト識別子とそのスコープ

ここで、今一度`enum`の文法を見直してみましょう。
```cpp
enum 識別子名 スコープ{
enumerator-list
}オブジェクト識別子;
```
最後のオブジェクト識別子とはどのように使うのでしょうか。
```cpp
enum X{
	A,B,C
}data; // here

int main()
{
	data=C;
}
```
このようにした時、`X`型の`data`というオブジェクトがその`enum`のあるスコープ内で生成されます。上記の場合、`data`はどのスコープにも所属していないため、グローバル変数として定義される事となります。つまり、以下の記述と動作は同じになります。
```cpp
enum X{
	A,B,C
};

X data;

int main()
{
	data=C;
}
```
これは、スコープ付き`enum`でも同様です。
```cpp
enum struct X{
	A,B,C
}data;

int main()
{
	data=X::C;
}
```
オブジェクト識別子を定義するとともに、初期化を行うこともできます。
```cpp
enum X{
	A,B,C
}data=C; // dataをCで初期化
```
スコープ付き`enum`も同様です。

### enumを関数内に定義する
`enum`は、関数内に定義することもできます。
```cpp
int main()
{
    enum X{A,B,C};
    enum Y{D,E,F}y=F;
    X x=C;
}
```
