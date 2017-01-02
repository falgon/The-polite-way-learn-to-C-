# 8.2 constexpr

`constexpr`は、コンパイル時の定数を表したり、コンパイル時の処理を表す事のできるキーワードです。`constexpr`は、C++のとても強力な言語機能で、コンパイル時の様々な演算や処理を実現します。順を追って見ていきましょう。

## 8.2.1 constexpr variable
`constexpr`な変数を定義する事ができます。書式は以下のように、変数定義時の型の前に`constexpr`キーワードを付与する事で可能です。
```cpp
constexpr type identifier = initialize value;
constexpr type identifier ( initialize value );
constexpr type identifier { initialize value };
```
`constexpr`は`const`と同じように、定義時になんらかの値で初期化しなければなりません。`constexpr`は定数という性質上、なんらかの値で初期化しなければ意味がないため、この挙動は自然と言えます。ここまで見ると、`const`との違いがあんまり分からないかもしれませんが、`const`と`constexpr`、両者の最たる相違点は**コンパイル時**定数であるか否かという点です。

例えば、以下のコードを見て見ましょう。
```cpp
int main()
{
    int a=10;
    const int b=a; // ok.実行時定数として定義
    constexpr int c=a; // エラー！aはコンパイル時定数ではない
}
```
コメントにある通り、`a`はコンパイル時定数ではなく、動的なプログラムの処理の中で値が変更される恐れがあります。よって、そのような値はコンパイル時定数として初期化する事はできないのです。

というわけで、この`constexpr`キーワードが付与された変数は、コンパイルの段階で既に結果が決まっている処理/値しか入れる事ができないという事を学びましたが、ところで、コンパイル時にできる事とは何なのでしょうか。

それは、静的なオブジェクト/値の演算です。
```cpp
int main()
{
    constexpr int a = 100 * 100; // コンパイル時演算
    constexpr int b = a * 10; // コンパイル時定数と定数の演算はコンパイル時演算
}
```
このように、コンパイル時定数、または定数同士の演算はコンパイル時に実行されるのです。つまり、`100 * 100`と記述した時、実際に`100 * 100`の演算が実行時に行われるのではなく、`10000`という数値としてコンパイルされるのです。よって、`constexpr`な変数に初期化値として与える事ができます。勿論、これは定数であるがための処理であって、動的な処理の演算は、当然ながらコンパイル時に行う事はできません。
```cpp
#include<iostream>
int main()
{
	int a=10,b=20; 
	constexpr int c=a*b; // コンパイル時定数でないオブジェクト同士の演算結果はconstexprの初期化値にできない。
}
```
上記のコードのように、確かに`a`と`b`は、それぞれ初期化のみの操作でその後値の変更はされていませんが、コンパイラはこれら定数である事を推論する事はできないので、コンパイルエラーとなります。このような事をしたければ、`a`、`b`を`constexpr`もしくは`const`で定義する必要があります。
```cpp
#include<iostream>
int main()
{
    const int a=10,b=20;
    constexpr int c=10,d=20;
    
    constexpr int e=a*b; // ok
    constexpr int f=c*d; // ok
}
```
あれ？`const`は実行時定数を指定するキーワードであって、それをコンパイル時定数である`constexpr`変数の初期値として与える事は出来ないのでは？と思うかもしれません。
そうです、途中までは合っています。実は、`const`というキーワードはコンパイル時定数(`constexpr`)にも、実行時定数にもなれるのです。`const`キーワードがコンパイル時定数になる条件は単純です。**`const`な変数の初期化時に与えた値が定数だった場合は、コンパイル時定数として扱え、そうでない場合は実行時定数として扱う**のです。
```cpp
int main()
{
	const int a=10;
	const int result1=a*10;

	int c=20;
	const int result2=a*c;

	constexpr int resultexpr1=result1; // 演算結果はコンパイル時に得られる。OK
	constexpr int resultexpr2=result2; // result2の演算結果を得るのに定数と断定できない変数cを使っている。それをコンパイル時に得る事はできない。エラー！
}
```
という事で、`constexpr`指定されたデータはコンパイル時に処理を終わらせる事ができるので、処理速度としては何よりも早い点(コンパイル段階で全てが終わっているのだから、実行時には何もする必要がない！)で、ぜひ活用すべき概念と言えるでしょう。可能な限り、全てを`constexpr`にしていくべきです。

## 8.2.2 constexpr function
`constexpr`な変数の使い方をここまでで述べてきましたが、なんと`constexpr`な関数も定義する事ができるのです。これも使うには、単に返却型の前に`constexpr`キーワードを付与するだけです。
```cpp
constexpr return-type functional-name(parameters)exception-keyword
{
    // ...
} 
```
早速使って見ましょう。
```cpp
constexpr int plus(int a,int b)
{
    return a+b;
}
int main()
{
    constexpr int result=plus(10,20);
}
```
このように、`constexpr`な関数は、`constexpr`な変数に入れる事ができます。`constexpr`な関数も、「8.1.1 constexpr variable」で述べた通りの制限が課されます。
```cpp
constexpr int plus(int a,int b)
{
	return a+b;
}

int main()
{
	int a=10;
	constexpr int result=plus(a,a); // エラー！定数でないオブジェクトをconstexprの初期値として使えない
}
```
しかし、`constexpr`関数に、非定数を渡す事自体は可能です。
```cpp
constexpr int plus(int a,int b)
{
	return a+b;
}

int main()
{
	int a=10,b=20;
	const int result=plus(a,b); // OK。constexprな関数を呼び出しているが引数が定数ではないため実行時処理。
}
```
この場合、`plus`関数の演算はコンパイル時ではなく、実行時に実行されるため、結果は上記コードのように、動的な変数を使わなければなりません。

これに加えて、`constexpr`な関数には実行時の通常の関数に比べて少々決まりごとがあったりします。それらを順に説明していきます。

1. `constexpr`関数内で変数宣言を行う場合、必ず初期化しなければならない。ただし、変数宣言時、`constexpr`キーワードを付与しなくても良い。
2. `constexpr`関数内で変数を書き換えても良い
3. 

まず、`constexpr`内で変数を宣言した場合、必ず初期化しなければなりません。また、`constexpr`関数内の変数は、`constexpr`キーワードを付与しなくても良いこととなっています。
```cpp
constexpr void f()
{
	constexpr 