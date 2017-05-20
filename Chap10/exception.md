# 10.1 例外
この項では、今まで謎めいていた例外という概念について学びます。お待たせしました。この項目で今まで先延ばしにしていた`noexcept`の意味がようやく分かるようになります。

さて、例外は平たく言えば想定外の自体が発生した場合の処理です。
C言語ではある関数内でエラーが発生した場合、プログラマが独自的に決めた値などを戻り値にしてその内部でエラーが起きた事を知らせようというようなプログラムがありましたが、プログラマが独自的に決めたルールというのは言語規約ではないので、いつでも破る事ができます。それは困りますよね。
C++でも、戻り値で判定を行う文化はしばしば残っていますが、より重大な内部エラーを孕んでしまった場合は、例外を送出する事が多いです。
この項では、そのように送出された例外の受け取り方と、逆に例外の送出を行う方法、またそれに関連する内容を説明します。

## 10.1.1 try,throw,catch
例外は、主に`try`、`throw`、`catch`の三つのキーワードで構成されます。例外が発生する可能性のある部分を`try`ステートメントで囲み、その後の`catch`ステートメントで受け取る事ができます。例外の送出は`throw`キーワードで行います。まずは簡単なサンプルを見て見ましょう。
```cpp
#include<iostream>

struct Exception{
    const char* what()const noexcept{return "Sended Except";}
};

int main()
{
    try{ // 例外が発生する可能性のある構文を囲む
        throw Exception(); // 例外を発生させる
    }catch(const Exception& exp){ // 送出させた例外を受け取る
        std::cerr<<exp.what()<<std::endl;
    }
}
```
実行結果は以下の通りです。
```cpp
Sended Except
```
上記のコードでは`try`ステートメントの内部で必ず`Exception`が例外送出されるため全く意味はありませんが、基本的な構文や使い方は上記の通りです。`throw`では、上記の通り例外を表現するrvalueオブジェクトを指定します。`catch`の仮引数リストでは、送出されると思われる例外型を記述します。

また`try`~`catch`は、関数内部のスコープに限らず関数の外側に設置することも可能です。
```cpp
int main()try
{
	// ...
}catch(const Exception& exp){
	//...
}
```
ところで、例外が`throw`された時、`catch`を行わなかった場合どうなるのでしょうか。
```cpp
int main()
{
    throw 1;
}
```
この場合、コンパイルには成功しますが、実行すると異常終了します。異常終了の際のエラー内容は環境によって様々なので一概に断定する事はできませんが、筆者の環境では以下のように出力され、異常終了しました。
```cpp
terminate called after throwing an instance of 'int'
zsh: abort      ./a.out
```
よって、正常に動かすためには例外を必ず`catch`しなければならないという事になりますが、その用途によって利用方法は様々であると言えるでしょう。

## 10.1.2 標準で用意された例外型
例外クラスは自身でオリジナルのものを定義する事は勿論できますが、標準に例外クラスが用意されていたりもします。それらは`<stdexcept>`ヘッダに定義されており、実行時エラーと論理エラーの二つのエラーモデルを表現しています。二つのエラーモデルの意味合いとしては以下のようになっています。

* 論理エラー
    * プログラムが論理的に誤っている事に起因するエラー。理論的には、それらはコンパイルの前に予め避けることができる。
* 実行時エラー
    * コンパイル時に検出する事ができないエラー。

標準で用意された例外型は、主に標準で用意された他ライブラリや組み込みから用いられますが、勿論ユーザーが利用する事も可能です。

| クラス名 | 意味 | エラーモデル
| -- | -- | -- |
| logic\_error | 論理エラーを示す | 論理 |
| domain\_error | 定義域エラーを示す | 論理 |
| invalid\_argument | 不正な引数を示す | 論理 |
| length\_error | 長すぎるオブジェクトを作ろうとしたことを示す | 論理 |
| out\_of\_range | 引数が許容範囲外であることを示す | 論理 |
| runtime\_error | 実行時エラーを示す | 実行時 |
| range\_error | 値が範囲外になったことを示す | 実行時 |
| overflow\_error | 数値計算の結果がオーバーフローしたことを示す | 実行時 |
| underflow\_error | 数値計算の結果がアンダーフローしたことを示す | 実行時 |

これらは全てコンストラクタに文字列を受け取り、`what`という仮想メンバ関数を保持しています。`what`はコンストラクタで設定した文字列を返します。仮想メンバ関数となっている理由は、そのメンバ関数をオーバーライドできるようにするためです。

以下にサンプルコードを示します。
```cpp
#include<stdexcept>
#include<iostream>

struct my_exception:std::logic_error{
    using std::logic_error::logic_error;
    const char* what()const noexcept override
    {
        return std::logic_error::what();
    }
};

int main()
{
    try{
        throw std::logic_error("logic_error exception");
    }catch(const std::logic_error& le){
        std::cerr<<le.what()<<std::endl;
    }

    try{
        throw my_exception("my_exception exception");
    }catch(const my_exception& me){
        std::cerr<<me.what()<<std::endl;
    }
}
```
実行結果は以下の通りです。
```cpp
logic_error exception
my_exception exception
```

## 10.1.3 noexceptキーワード
`noexcept`は例外に関する情報を制御する構文です。このキーワードは例外仕様と、演算子としての二つの意味があります。二つの意味の使い分けは記述した部位によって区別されます。

### 例外仕様
例外仕様とは、その関数が例外を送出する可能性があるか否かを制御するものです。
例外仕様としての`noexcept`は以下のように記述する事ができます。
```cpp
void f()noexcept;
```
```cpp
void f()noexcept( /* bool値に変換可能な整数定数式 */ );
```
`noexcept`もしくは、`noexcept()`の評価結果`noexcept(true)`となる場合、その関数からは例外が送出してはなりません。もし例外が送出された場合、`std::terminate`関数によってプログラムは異常終了します。よって、`noexcept`または`noexcept()`の結果`noexcept(true)`となる関数は例外が送出されない関数である事を示す事ができます。

逆に`noexcept`例外仕様に対してその評価が`false`に評価される整数定数式を指定した関数は、あらゆる例外を送出する可能性がある事を明示します。
尚、noexcept例外仕様を指定しない関数は、一部の例外を除いて、デフォルトで`noexcept(false)`を意味しますが、デストラクタとdelete演算子は、明示的に`noexcept(false)`に評価される整数定数式を指定しない限り、デフォルトで`noexcept(true)`となります。
```cpp
void f()noexcept; // 関数fは例外を送出しない
void g()noexcept(true); // 関数gは例外を送出しない
void h()noexcept(false); // 関数hは例外を送出する可能性がある
```

### 演算子
演算子としての`noexcept`は、引数に指定した定数式が、例外を送出する可能性があるかをコンパイル時に判定し、`bool`値を返します。演算子としての`noexcept`は、例外仕様として`noexcept(false)`と評価される関数(関数ポインタを含む)を指定した場合、`false`が返されます。
```cpp
void f()noexcept; // 例外を送出しない

void g()noexcept(false); // 例外を送出する可能性がある

int main()
{
    static_assert(noexcept(f())); // true
    static_assert(!noexcept(g())); // !false
}
```
この演算子は、関数が宣言のみで実装がないのにも関わらず`noexcept`に指定できている事から分かる通り、`sizeof`、`decltype`と同じく引数として指定された式は、実行時評価はされません。

これらの両機能を利用して、例えばある`constexpr`でない一文が例外を送出しない場合、その関数自体も例外を送出しない事を示すという使い方ができます。
```cpp
void f()noexcept{}
void g()noexcept(noexcept(f())){} // 内包されたnoexceptは演算子として、その外側のnoexceptは例外仕様としてのnoexcept
```
このように記述した場合、関数`f`が`noexcept`である場合に関数`g`は`noexcept(true)`に、そうでない場合は`noexcept(false)`になります。
```cpp
void f()noexcept{}
void g()noexcept(false){}

void h()noexcept(noexcept(f())){f();}
void i()noexcept(noexcept(g())){g();}

int main()
{
    static_assert( noexcept(h()) ); // noexcept(true) == noexcept(h())
    static_assert( !noexcept(i()) ); // noexcept(false) == noexcept(i())
}
```


## 10.1.4 noexceptは関数型に含まれる
例えば以下のように関数のアドレスを格納しようとした場合、以下のように記述する事が望ましいです。
```cpp
void f()noexcept{};

int main()
{
    void (*f_ptr)()noexcept=f;
}
```
何が望ましい部分なのかというと、関数ポインタの宣言部分です。
関数ポインタの型は`void (*f_ptr)()noexcept`となっています。そうです、`noexcept`は型の一部なのです。
実は、以下のように関数ポインタ側では`noexcept`指定をしなくても規格違反ではありません。
```cpp
void f()noexcept{};

int main()
{
    void (*f_ptr)()=f; // 例外を投げない関数fの関数ポインタをnoexceptにしなくても良い
}
```
しかし、ポイントしている関数が`noexcept`であるのならば、最適化や明示性などを考えると関数ポインタ側にも`noexcept`を付与する事が望ましいです。

尚、`noexcept`でない関数をポイントする型を、`noexcept`指定する事はできません。
```cpp
void f(){};

int main()
{
    void (*f_ptr)()noexcept=f; // エラー！例外を投げるかもしれない関数fのアドレスをnoexceptな関数のポインターで格納できない
}
```

この関数の型に`noexcept`が含まれるという機能は、割と最近に組み込まれた機能なので、対応していないコンパイラに遭遇する可能性もまだまだあるでしょう。
そこで、以下の標準で定義されるマクロを用いる事で正しくこの機能が搭載されているか知る事ができます。
```cpp
#ifdef __cpp_noexcept_function_type
// noexceptが関数の型の一部でなければならないコード
#else
// そうでないコード...
#endif
```
尚、`__cpp_noexcept_function_type`の値は`201510`です。
