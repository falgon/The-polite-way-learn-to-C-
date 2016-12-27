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

これはどのように使えるかというと、例えば、variadic templateの引数の数で分岐したければ、今までの方法では、前述したSFINAEを用いて同じような事を実現していました。
```cpp
code
```
しかし、SFINAEを用いた条件分岐は、視覚的にとても直感的とは言えません。これを、constexpr if文で実現すると以下のように記述できます。
```cpp
code
```
constexpr if文は直感的な記述ができるだけでなく、例えばSFINAEであればオーバーロードするもう一つの関数、つまりElseの役割となるものが必要となります。
```cpp
code
```
しかしconstexpr if文ではElseの役割となるものは必要なくなるので、SFINAEでの条件分岐と比べて無駄な記述を省く事ができます。
```cpp
code
```

See also:http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2016/p0292r2.html