# 8.1 enum
enumは列挙型とも言われます。enumを用いることである一定の値しかとることのできない型を作る事ができます。型を作れるってどういうことだ？！と思うかもしれませんが、とりあえず以下のコードを見てみましょう。
```cpp
enum Color{
    Red,
    Green,
    Blue
};

void print(const Color& cl)
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
```s