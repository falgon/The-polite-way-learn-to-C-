# 8.1 enum
enumは列挙型とも言われます。enumを用いることである一定の値しかとることのできない型を作る事ができるのです。文法を以下に示します。
```cpp
enum 識別子名 スコープ{
    enumerator-list
}オブジェクト識別子;
```
このように記述する事で、その識別子名のenum型を作る事ができます。まあまあ取り敢えず、以下のコードを見てみましょう。
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
`Color`という名前の`enum`型を定義し、そのenumerator-listに`Red`、`Green`、`Blue`と定義しています。この時、enumerator-listに定義した順に、0 1 2 3 4...と各enumeratorに定義されます。よって、`Red`は`0`、`Green`は`1`、`Blue`は`2`となります。そして関数`print`の引数型には、`Color`というenum型を指定しています。よってこの関数に渡せるデータは、`Color`型のオブジェクトのみとなります。
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
