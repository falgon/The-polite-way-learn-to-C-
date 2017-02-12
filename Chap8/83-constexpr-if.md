# 8.3 constexpr if文

`constexpr`なif文を記述する事ができます。例えば、以下のような場合にこの構文は有効です。
```cpp
#include<iostream>

constexpr int plus(const int x,const int y){return x+y;}

int main()
{
	constexpr int a=10,b=20;
	
	if(plus(a,b)==30){
		std::cout<<"thirty!"<<std::endl;
	}else{
		std::cout<<"not thirty!"<<std::endl;
	}
}
```
実行結果は以下となります。
```cpp
thirty!
```
`constexpr`関数の戻り値によって、処理内容を分岐していますが、通常のif文だと動的な動作という意味になってしまいます。つまり、`constexpr`な変数と、`constexpr`な関数を使ってせっかくコンパイル時に値が定まっていたとしても、if文では`30 == 30`という演算を実行時に処理してしまうのです。(最適化によっては動的に処理しないコードを生成する可能性もあるが、処理系やコンパイルオプションに依存するため、最適化が必ず発生する事は保証できない。C++の規格ではあくまでも動的に実行される一文として解釈される事が制定されている。)

このような場合には、条件式がコンパイル時に定まる事を規定とした、`constexpr if`文を使います。
```cpp
#include<iostream>

constexpr int plus(const int x,const int y){return x+y;}

int main()
{
	constexpr int a=10,b=20;
	
	if constexpr(plus(a,b)==30){
		std::cout<<"thirty!"<<std::endl;
	}else{
		std::cout<<"not thirty!"<<std::endl;
	}
}
```
`if constexpr`というようにする事で、その分岐処理は、必ずコンパイル時に行われるようになります。よって、例えば動的な条件式を`constexpr if`文で使う事はできません。
```cpp
#include<iostream>

int plus(int x,int y){return x+y;}

int main()
{
	int a=10,b=20;
	
	if constexpr(plus(a,b)==30){ // plus関数は静的な関数でないためエラー
		std::cout<<"thirty!"<<std::endl;
	}else{
		std::cout<<"not thirty!"<<std::endl;
	}
}
```
コードを見てお気づきになったかもしれませんが、`if constexpr`の後の`else`では、`constexpr`キーワードを付与する必要はありません。しかし、`else if`節では、以下のように`else constexpr if`とする必要があります。
```cpp
#include<iostream>

constexpr int plus(const int x,const int y){return x+y;}

int main()
{
	constexpr int a=10,b=30;
	
	if constexpr(plus(a,b)==30){
		std::cout<<"thirty!"<<std::endl;
	}else if constexpr(plus(a,b)==40){
		std::cout<<"fourty!"<<std::endl;
	}else{
        std::cout<<"other"<<std::endl;
    }
}
```
実行結果は以下となります。
```cpp
fourty!
```
ここで、動的な`if`文との違いを明示しておきましょう。まず第一には、条件式がコンパイル時に実行できるものでなければならないというものでした。もう一つあります。それは、短略、連続的な条件式の評価のされ方の違いです。以下のコードを見て見ましょう。
```cpp
#include<iostream>

int main()
{
	int a=10;

	if(a || ++a){
		std::cout<<a<<std::endl;
	}
}
```
実行結果は以下となります。
```cpp
10
```
これは通常のif文です。しかし、結果として`11`になりそうなところが、少し予想外の動きになったかもしれません。まず、`if`文では`a`の値を`bool`値に変換した値が評価されます。0以外の値は全て`true`に変換されますから、`if`文の条件式を通過する事は目に見えています(`||`演算子によって)。この時、`if`文は、目に見えた結果を検知した瞬間、次の条件式について全く評価せずに、内部の処理内容へと進むのです。よって、`a`はインクリメントされず、出力は`10`という結果になりました。

一方`constexpr if`文では、このような事はなく、全ての条件式が必ず評価されます。

因みに以下のように、`else if`節で`constexpr`を省くと、それ以降の処理は動的処理となります。
```cpp
a
```