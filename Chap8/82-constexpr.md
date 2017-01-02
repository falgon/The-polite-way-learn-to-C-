# 8.2 constexpr

`constexpr`は、コンパイル時の定数を表したり、コンパイル時の処理を表す事のできるキーワードです。`constexpr`は、C++のとても強力な言語機能で、コンパイル時の様々な演算や処理を実現します。順を追って見ていきましょう。

## 8.2 constexpr variable
constexprな変数を定義する事ができます。書式は以下の通りです。
```cpp
constexpr int value=0;
```
`constexpr`は`const`と同じように、定義時になんらかの値で初期化しなければなりません。`constexpr`は定数という性質上、なんらかの値で初期化しなければ意味のないシーンが多いため、この挙動は自然と言えます。ここまで見ると、`const`との違いがあんまり分からないかもしれませんが、`const`と`constexpr`、両者の最たる相違点は**コンパイル時**定数であるか否かという点です。

例えば、実は`constexpr`は代入操作が行えるのです。
```cpp
int main()
{
    const int a=10,b=20;
    constexpr int a_=10,b_=20;
    
    a=b; // エラー！const指定された変数への代入は許されない
    a_=b_; // ok
}
```
