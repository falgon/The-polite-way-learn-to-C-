# 9.x static member

## 9.x.x 名前解決に纏わる注意

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

