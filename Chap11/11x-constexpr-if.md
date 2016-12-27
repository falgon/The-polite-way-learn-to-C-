# 11.x constexpr if

`constexpr if`文はコンパイル時の条件分岐を行う事のできる文法です。構文は以下の通りです。
```cpp
if constexpr(条件式)
// ...
else if 
// ...
else
/// ...
```
`if`キーワードの後、条件式のカッコの前に`constexpr`キーワードを付与します。このように記述した場合、条件式の値は、bool型に文脈変換された定数式にとなります。

この機能により、今まではSFINAEや再起で分岐していた処理を`if`文の形態で実現できるようになります。
```cpp
#include<iostream>

void g(){}

template<class T,class... Rest>
void g(T&& p,Rest&&... rs)
{
	std::cout<<std::forward<T>(p)<<std::endl;
	g(std::forward<Rest>(rs)...);
}

int main()
{
	g(42,42,42,42,42,42);
}
```
実行結果は以下となります。
```cpp
42
42
42
42
42
42
```
しかし、上記の通り再起を用いた分岐は、視覚的にとても直感的とは言えません。これを、`constexpr if`文で実現すると以下のように記述できます。
```cpp
#include<iostream>

template<class T,class... Rest>
void g(T&& p,Rest&&... rs)
{
	std::cout<<std::forward<T>(p)<<std::endl;
	if constexpr(sizeof...(rs)>0)
		g(std::forward<Rest>(rs)...);
}

int main()
{
	g(42,42,42,42,42,42);
}
```
実行結果は同じです。

このように、`constexpr if`文は直感的な記述ができるだけでなく、例えばSFINAEであればオーバーロードするもう一つの関数、つまりElseの役割となるものが必要となります。
```cpp
code
```
しかしconstexpr if文ではElseの役割となるものは必要なくなるので、SFINAEでの条件分岐と比べて無駄な記述を省く事ができます。
```cpp
code
```

See also:http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2016/p0292r2.html