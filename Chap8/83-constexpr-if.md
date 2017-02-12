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
