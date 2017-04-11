# 9.4 演算子のオーバーロード

第6章で、関数をオーバーロードできる事を学びましたが、なんと演算子すらもオーバーロードを行う事ができるのです。演算子のオーバーロードを適切に活用する事で、ソースコードに高いセマンティックを与える事ができます。順に学んでいきましょう。


## 9.4.1 operator+
まず、

## 9.4.2 new/delete\(usual new/delete\)

new/delete演算子すらも、なんとオーバーロードすることができるのです。まず本稿では、より一般的なusual new/deleteのオーバーロードについて説明します。

さて、まずはじめに、ここで種明かしのようになりますが、「第7章動的な領域確保」で行ったnew/delete演算子の操作は、**そもそも標準で用意されている、グローバル領域に暗黙的に定義されたoperator new、operator deleteを呼び出していた**のです。

以下らがプログラムの翻訳単位で暗黙的に定義されます。

```cpp
void* operator new(std::size_t);
void* operator new(std::size_t, std::align_val_t);
void operator delete(void*) noexcept;
void operator delete(void*, std::size_t) noexcept;
void operator delete(void*, std::align_val_t) noexcept;
void operator delete(void*, std::size_t, std::align_val_t) noexcept;
void* operator new[](std::size_t);
void* operator new[](std::size_t, std::align_val_t);
void operator delete[](void*) noexcept;
void operator delete[](void*, std::size_t) noexcept;
void operator delete[](void*, std::align_val_t) noexcept;
void operator delete[](void*, std::size_t, std::align_val_t) noexcept;
```

通常のnew/delete演算子を実行すると、これらの暗黙定義された関数が実行されることになっています。

operator new/deleteの第二引数、`std::size_t`にはそのオブジェクトのバイト数が渡されます。

`std::alignval_t`は、アライメントが、標準で定義されている`__STDCPP_DEFAULT_NEW_ALIGNMENT__`というマクロ定数よりも大きいオブジェクトと使われた場合にそのアライメントを送信するために使われます\(アライメントについてはコラムを参照\)。これらと同じシグネチャでユーザーがグローバル領域にnew/deleteを定義した場合、**翻訳段階でその定義によって完全に上書きされます。**

ではこれらを全て独自に定義して、これまで説明してきたものを確認してみましょう。

```cpp
#include<cstdio>
#include<cstdlib>

struct liner{
    static void line()
    {
        for(std::size_t i=0; i<50; ++i)printf("-");
        puts("");
    }

    liner(){line();}
    ~liner(){line();}
};

// Single object operator new
void* operator new(std::size_t x) // (1)
{
    liner _;

    std::printf("1 param op %s std::size_t is %ld\n",__func__,x);
    return std::malloc(x);
}

void* operator new(std::size_t x,std::align_val_t align) // (2)
{
    liner _;

    std::printf("2 param op %s std::size_t is %ld\n",__func__,x); 
    std::printf("align is %ld\n",static_cast<std::size_t>(align));

    return aligned_alloc(static_cast<std::size_t>(align),x);
}

// Array object operator new
void* operator new[](std::size_t x) // (3)
{
    liner _;

    std::printf("1 param op %s std::size_t is %ld\n",__func__,x);
    return std::malloc(x);
}

void* operator new[](std::size_t x,std::align_val_t align) // (4)
{
    liner _;

    std::printf("2 params op %s std::size_t is %ld\n",__func__,x);
    std::printf("align is %ld\n",static_cast<std::size_t>(align));

    return aligned_alloc(static_cast<std::size_t>(align),x);
}

// Single object operator delete
void operator delete(void* ptr)noexcept // (5)
{
    liner _;

    std::printf("1 param op %s\n",__func__);
    std::free(ptr);
}

void operator delete(void* ptr,std::size_t x)noexcept // (6)
{
    liner _;

    std::printf("2 params op %s std::size_t is %ld\n",__func__,x);
    std::free(ptr);
}

void operator delete(void* ptr,std::align_val_t align)noexcept // (7)
{
    liner _;

    std::printf("2 param op %s\n",__func__);
    std::printf("align is %ld\n",static_cast<std::size_t>(align));
    std::free(ptr);
}

void operator delete(void* ptr,std::size_t x,std::align_val_t align)noexcept // (8)
{
    liner _;

    std::printf("3 param op %s std::size_t is %ld\n",__func__,x);
    std::printf("align is %ld\n",static_cast<std::size_t>(align));
    std::free(ptr);
}

// Array object operator delete
void operator delete[](void* ptr)noexcept // (9)
{
    liner _;

    std::printf("1 param op %s\n",__func__);
    std::free(ptr);
}

void operator delete[](void* ptr,std::size_t x)noexcept // (10)
{
    liner _;

    std::printf("2 params op %s std::size_t is %ld\n",__func__,x);
    std::free(ptr);
}


void operator delete[](void* ptr,std::align_val_t align)noexcept // (11)
{
    liner _;

    std::printf("2 params op %s\n",__func__);
    std::printf("align is %ld\n",static_cast<std::size_t>(align));
    std::free(ptr);
}


void operator delete[](void* ptr,std::size_t x,std::align_val_t align)noexcept // (12)
{
    liner _;

    std::printf("3 params op %s std::size_t is %ld\n",__func__,x);
    std::printf("align is %ld\n",static_cast<std::size_t>(align));
    std::free(ptr);
}

struct X{
    X(){std::puts(__func__);}
    ~X(){std::puts(__func__);}    

    char a[200];
};

struct alignas((__STDCPP_DEFAULT_NEW_ALIGNMENT__*2)) Y{
    Y(){std::puts(__func__);}
    ~Y(){std::puts(__func__);}
};


int main()
{
    delete new X();
    delete new Y();

        delete[] new X[3];
        delete[] new Y[3];
}
```

実行結果は以下の通りです。

```cpp
--------------------------------------------------
1 param op operator new std::size_t is 200
--------------------------------------------------
X
~X
--------------------------------------------------
2 params op operator delete std::size_t is 200
--------------------------------------------------
--------------------------------------------------
2 param op operator new std::size_t is 32
align is 32
--------------------------------------------------
Y
~Y
--------------------------------------------------
3 param op operator delete std::size_t is 32
align is 32
--------------------------------------------------
--------------------------------------------------
1 param op operator new [] std::size_t is 608
--------------------------------------------------
X
X
X
~X
~X
~X
--------------------------------------------------
2 params op operator delete [] std::size_t is 608
--------------------------------------------------
--------------------------------------------------
2 params op operator new [] std::size_t is 96
align is 32
--------------------------------------------------
Y
Y
Y
~Y
~Y
~Y
--------------------------------------------------
3 params op operator delete [] std::size_t is 128
align is 32
--------------------------------------------------
```
このようにグローバル名前空間にoperator new/deleteをオーバーロードすると、型の内部のメンバ関数としてoperator new/deleteが独自にオーバーロードされていた場合を除いて、全ての型に対するnew/delete操作でこのオーバーロードされたoperator new/deleteが呼び出されるようになります。
実行結果からも、様々なものが表されていますね。

オーバーロードは上から単一のオブジェクト用のoperator new、複数のオブジェクト用のoperator new\[\]、単一のオブジェクトのoperator delete、複数のオブジェクト用のoperator delete\[\]と記述されています（`liner`という型を作っていますが、その名の通り単に罫線を引くためだけのものなので、本項の内容とは特別関係ありません。\)。

`main`関数内の実行過程をここで順に見ていきましょう。

* `main`関数では、まず一番初めに単一の`X`型をnewしています。この時呼び出されるのは`(1)`のoperator newです。その後、出力からわかるように`X`のコンストラクタが呼ばれています。`main`関数ですぐさまそれをdeleteしていますので、その後の即座に`X`のデストラクタが呼ばれていることがわかります。その後、operator deleteが呼び出されます。この時呼び出されるのは`(6)`のoperator deleteです。

* 次に、単一の`Y`型をnewしています。`Y`型の定義部分に注目してください。`struct alignas((__STDCPP_DEFAULT_NEW_ALIGNMENT__*2)) Y`となっています。これは、`Y`型のインスタンスを特定のバイト境界でメモリに置けとコンパイラに対して示しています。バイト境界についてもコラムのアライメントの項目で取り上げますが、簡単に言えば、例えば4バイト境界の位置にアライメントした場合、オブジェクトはメモリ上の4の倍数のアドレスに配置され、8バイト境界の位置にアライメントした場合、オブジェクトはメモリ上の8の倍数のアドレスに配置される事となります。そしてその指定は`alignas`を使う事で可能です。今回の場合、`Y`の宣言部分で`alignas`を指定しているため、インスタンスは全てアライメントされなければなりません。そしてそのアライメント値を`__STDCPP_DEFAULT_NEW_ALIGNMENT__`の二倍としています。`std::align_val_t`版のoperator new/deleteが呼ばれるのは、呼び出しに関連するオブジェクトのアライメントが`__STDCPP_DEFAULT_NEW_ALIGNMENT__`よりも大きかった場合ですから、その二倍を指定しているので`std::align_val_t`版が呼び出されるのは必然と言えます。よって、この時newで呼び出されるのは、`(2)`のoperator newです。`(2)`のoperator newでは、`std::aligned_alloc`が呼び出されていますが、これは任意のアライメント値でアライメントしつつ領域を確保することができる標準関数です。これは、`<cstdlib>`に定義されています。さてその後、先ほどと同じくコンストラクタが呼び出され、
即デストラクタが呼び出されています。その後、`std::align_val_t`版のoperator deleteが呼び出されます、この時呼び出されるのは`(8)`のoperator deleteです。

* 次に、配列の`X`型をnewしています。この時呼び出されるのは、`(3)`のoperator new[]です。今回要請した要素数は3なので、コンストラクタ、デストラクタがそれぞれ三度呼び出されています。operator deleteが呼び出されます。この時呼び出されるのは`(10)`のoperator deleteです。

* 最後に、配列の`Y`型をnewしています。この時呼び出されるのは、`(4)`のoperator new[]です。今回要請した要素数は3なので、コンストラクタ、デストラクタがそれぞれ三度呼び出されています。operator deleteが呼び出されます。この時呼び出されるのは`(12)`のoperator deleteです。

このように、**グローバルに定義したoperator new/deleteは、最もシグネチャの合う、引数の多いoperator deleteが優先的に呼び出されます**。

また、一つ注目して頂きたいのが、全てのoperator deleteに対してnoexcept指定がされているということです。これは、全てのoperator deleteは、例外を投げないという標準の要件によるものです。標準の要件なので、当然ながら、その要件は満たすべきです。

さて、次にoperator new/deleteをクラススコープでメンバ関数としてオーバーロードしてみましょう。ソースコードは殆ど変わりませんが、挙動が若干変化します。
```cpp
#include<cstdio>
#include<cstdlib>

struct liner{
    static void line()
    {
        for(std::size_t i=0; i<50; ++i)printf("-");
        puts("");
    }
    
    liner(){line();}
    ~liner(){line();}
};

struct X{
	X(){std::puts(__func__);}
	~X(){std::puts(__func__);}	

	char a[200];

    void* operator new(std::size_t x) // (1)
    {
        liner _;
    
	   std::printf("1 param op %s std::size_t is %ld\n",__func__,x);
	   return std::malloc(x);
    }

    void* operator new[](std::size_t x) // (2)
    {
        liner _;
    
        std::printf("1 param op %s std::size_t is %ld\n",__func__,x);
        return std::malloc(x);
    }

    void operator delete(void* ptr)noexcept // (3)
    {
        liner _;
    
	   std::printf("1 param op %s\n",__func__);
	   std::free(ptr);
    }

    void operator delete(void* ptr,std::size_t x)noexcept // (4)
    {
        liner _;
    
	   std::printf("2 params op %s std::size_t is %ld\n",__func__,x);
	   std::free(ptr);
    }
    
    void operator delete[](void* ptr)noexcept // (5)
    {
        liner _;
    
        std::printf("1 param op %s\n",__func__);
        std::free(ptr);
    }

    void operator delete[](void* ptr,std::size_t x)noexcept // (6)
    {
        liner _;
    
        std::printf("2 params op %s std::size_t is %ld\n",__func__,x);
        std::free(ptr);
    }

};

struct alignas((__STDCPP_DEFAULT_NEW_ALIGNMENT__*2)) Y{
	Y(){std::puts(__func__);}
	~Y(){std::puts(__func__);}

    void* operator new(std::size_t x,std::align_val_t align)noexcept // (7)
    {
        liner _;
    
        std::printf("2 param op %s std::size_t is %ld\n",__func__,x); 
	   std::printf("align is %ld\n",static_cast<std::size_t>(align));

	   return aligned_alloc(static_cast<std::size_t>(align),x);
    }
    
    void* operator new[](std::size_t x,std::align_val_t align) // (8)
    {
        liner _;
    
        std::printf("2 params op %s std::size_t is %ld\n",__func__,x);
        std::printf("align is %ld\n",static_cast<std::size_t>(align));
    
        return aligned_alloc(static_cast<std::size_t>(align),x);
    }


    void operator delete(void* ptr,std::align_val_t align)noexcept // (9)
    {
        liner _;
    
        std::printf("2 param op %s\n",__func__);
	   std::printf("align is %ld\n",static_cast<std::size_t>(align));
	   std::free(ptr);
    }

    void operator delete(void* ptr,std::size_t x,std::align_val_t align)noexcept // (10)
    {
        liner _;
    
        std::printf("3 param op %s std::size_t is %ld\n",__func__,x);
	   std::printf("align is %ld\n",static_cast<std::size_t>(align));
	   std::free(ptr);
    }
    
    void operator delete[](void* ptr,std::align_val_t align)noexcept // (11)
    {
        liner _;
    
        std::printf("2 params op %s\n",__func__);
        std::printf("align is %ld\n",static_cast<std::size_t>(align));
        std::free(ptr);
    }

    void operator delete[](void* ptr,std::size_t x,std::align_val_t align)noexcept // (12)
    {
        liner _;
    
        std::printf("3 params op %s std::size_t is %ld\n",__func__,x);
        std::printf("align is %ld\n",static_cast<std::size_t>(align));
        std::free(ptr);
    }
};


int main()
{
	delete new X();
	delete new Y();
    
    delete[] new X[3];
    delete[] new Y[3];
}
```
実行結果は以下の通りです。

```cpp
--------------------------------------------------
1 param op operator new std::size_t is 200
--------------------------------------------------
X
~X
--------------------------------------------------
1 param op operator delete
--------------------------------------------------
--------------------------------------------------
2 param op operator new std::size_t is 32
align is 32
--------------------------------------------------
Y
~Y
--------------------------------------------------
2 param op operator delete
align is 32
--------------------------------------------------
--------------------------------------------------
1 param op operator new [] std::size_t is 608
--------------------------------------------------
X
X
X
~X
~X
~X
--------------------------------------------------
1 param op operator delete []
--------------------------------------------------
--------------------------------------------------
2 params op operator new [] std::size_t is 128
align is 32
--------------------------------------------------
Y
Y
Y
~Y
~Y
~Y
--------------------------------------------------
2 params op operator delete []
align is 32
--------------------------------------------------
```
まず少し不思議に思えるのが、static修飾なしにクラスのメンバとしてoperator new/deleteを定義しているのにも関わらず、まるでstaticであるかのように呼び出せています。これは、一つの決定事項があるために実現できています。その決定事項とは、**全てのクラススコープのoperator new/deleteは、必ずstaticメンバ関数となる**という事です。よって、staticキーワードを付与せずとも、operator new/deleteは特別にstaticメンバ関数となります(staticキーワードを明示的に付与しても良い)。

さて、実行結果から伺えるように、クラスメンバとしてoperator new/deleteをオーバーロードした場合、グローバル領域でオーバーロードした場合と異なるoperator deleteが呼び出されている事が分かります。これは優先度の変化というわけではありません。**operator deleteのすべてのパターンをクラススコープでオーバーロードした場合、**`void*`**一つを受け付けるoperator delete、**`void*`**に加えて**`std::alignval_t`**を受け付けるoperator deleteはusual delete\(通常のdelete\)として、**`void*`**に加えて**`std::size_t`**を受け付けるoperator deleteはplacement deleteとして捉えられるため**です。

```cpp
/*                             */
/* []といった配列版を省略...     */
/*                             */

// グローバルスコープ ...
void operator new(std::size_t); // usual new
void operator delete(void*)noexcept; // usual delete
void operator delete(void*,std::align_val_t)noexcept; // usual delete
void operator delete(void*,std::size_t)noexcept; // usual delete

// クラススコープ...
struct X{
    void operator new(std::size_t); // usual new
    void operator delete(void*)noexcept; // usual delete
    void operator delete(void*,std::align_val_t)noexcept; // usual delete
    void operator delete(void*,std::size_t)noexcept; // placement delete
};
```
operator new/deleteは、usualなnewであればusualなdeleteを、placementなnewであればplacementなdeleteを呼ぶため、usual deleteであるかplacement deleteであるかによって、結果として呼び出せるものが変わってくるのです。


...とここまでは全てのパターンのusualなoperator new/deleteをオーバーロードしてきましたが、文法的には全てのパターンを網羅せずとも違法なプログラムとはなりません。しかし、汎用性を考えると、operator new/deleteをオーバーロードするのであれば網羅するのが好ましいです。

## 9.x.x Placement new / delete\(placement/Non-allocating forms new/delete \)

本項ではこれまでで説明してきたusual new/deleteとは少し異なる領域の確保/活用方法を説明します。

まず、`new`演算子には、「7.6 動的な領域確保」で述べたような使い方に加えて、情報を付加するような構文があり、標準で定義されたものは大きく二種類に分類することができます。これらは**placement new\(しばしば配置new\)**と呼ばれます。

### placement-newによる情報負荷

まず一つ目ですが、`new`演算子は、領域の確保に失敗すると`std::bad_alloc`例外を送出しますが、従来の古いしきたりのように、例外ではなく`nullptr`を返すように設定することができます\(しかし、これは現代のC++ではdeprecatedとされているため、`nullptr`を返すようにするべきではありません\)。

```cpp
#include<new>

struct X{};

int main()
{
    X* ptr=new(std::nothrow) X();
    if(p)delete ptr;
}
```

`new()`と記述して、そこに`std::nothrow`を設定することで`nullptr`を返すようになります\(メモリ領域の確保中に起きる例外に限ります。オブジェクト生成中の例外、つまりコンストラクタ実行中の例外が発生した場合はそのまま例外を投げることになります\)。`std::nothrow`は`<new>`ヘッダに定義されている`std::nothrow_t`型の定数です。またこの構文は`new`演算子をオーバーロードすることで使うことができる構文です。上記の場合では、これまた`<new>`ヘッダの中で既にplacement newがオーバーロードされているため、使うことができています。

これらは、`<new>`ヘッダ内で以下のように宣言されています。

```cpp
void* operator new(std::size_t, const std::nothrow_t&)noexcept;
void* operator new[](std::size_t, const std::nothrow_t&)noexcept;
void operator delete(void*, const std::nothrow_t&)noexcept;
void operator delete[](void*, const std::nothrow_t&)noexcept;
```

全てが`noexcept`修飾されていることに注目してください。`std::nothrow`によって例外は投げないと指定していることからその明示性が伺えます。

この機能については、完全に独自的に新たにオーバーロードする事が可能です。以下では例として、領域確保が失敗した場合、ソースコード中の該当行を出力させるといった事をしています。
```cpp
#include<cstdio>
#include<cstdlib>

struct X{
    void* operator new(std::size_t size) // usual new
    {
        return std::malloc(size);
    }

    void* operator new([[maybe_unused]] std::size_t size,std::size_t Line) // placement new
    {
        // void* ptr=std::malloc(size)
        void* ptr=nullptr;
        if(!ptr){
            std::printf("Failed to allocate\n[LINE]:%ld\n",Line);
        }
        return ptr;
    }
    
    void operator delete(void* ptr)noexcept // usual delete
    {
        std::free(ptr);
    }
    
    void operator delete(void* ptr,std::size_t)noexcept // placement delete
    {
        if(ptr)std::free(ptr);
    }
};
int main()
{
    X* ptr=new(__LINE__) X;
    if(ptr)delete ptr;
}
```
実行結果は以下となります。
```cpp
Failed to allocate
[LINE]:32
```
コメントアウトしてあるように、意図的に領域確保失敗のケースをシミュレートしています。コード中の`[[maybe_unused]]`とは、その変数を一切使わない事があっても、コンパイラに警告を出させないようにする抑制命令です。

ここで少し不思議なのが、placement newしか使わないはずなのに、何故かusual new/deleteが定義されているということです。これは、このコードが冗長であるわけではなく、絶対的に必要であるがためにusual new/deleteが定義されています。どのように必要なのでしょうか？
まずは、placement new/deleteだけを定義した場合のplacement new/delete両者のシグネチャを見て見ましょう。
```cpp
struct X{
	void* operator new(std::size_t,std::size_t); // placement new
	void operator delete(void*,std::size_t)noexcept; // placement delete ...ではない
};
```
さて、上記のplacement new/deleteはこのようなシグネチャで宣言されていますが、operator deleteに注目してください。実は、このdelete、コメントにもあるようにplacement deleteではなく、usual deleteとして定義されてしまうのです。その理由は、usual new/deleteの項で取り上げたように、`void operator delete(void*,std::size_t)noexcept;`というシグネチャは、usual deleteにもなりうるといった事に関連しています。**このような定義のみの`X`は`void operator delete(void*)`を持ちません。そういった場合、`void operator delete(void*,std::size_t)noexcept;`といったシグネチャのoperator deleteは、placement deleteではなく、usual deleteとして定義されてしまうのです。`void operator delete(void*,std::size_t)noexcept;`をplacement deleteとして定義したい場合、`void operator delete(void*)`といったシグネチャのoperator usual delete(通常とは異なるアライメントに対応させるoperator new/deleteのusualなoperator deleteはvoid operator delete(void*,std::align_val_t)noexcept;となる)が定義されなければなりません**。
よって、下記のように追加する必要があります。
```cpp
struct X{
	void* operator new(std::size_t,std::size_t); // placement new
	void operator delete(void*); // usual delete
	void operator delete(void*,std::size_t)noexcept; // placement delete
}
```
...しかし、これだけは不十分です。`void* operator delete(void*,std::size_t)noexcept;`をplacement deleteにするために定義した`void operator delete(void*)`というusual deleteですが、**usual deleteが定義されているのならば、それの対となるusual newが定義されなければなりませんね**。ということで、最終的に、operator new/deleteの宣言は以下のようにならなければならないのです。
```cpp
struct X{
	void* operator new(std::size_t); // usual deleteの対となるusual new
	void* operator new(std::size_t,std::size_t); // placement new
	void operator delete(void*)noexcept; // 下記のdeleteをplacement deleteとするためのusual delete
	void operator delete(void*,std::size_t)noexcept; // placement delete
};
```
さて、少しややこしいところを終えたところで、引き続きプログラムの流れを追って見ましょう。
`main`関数内のplacement newを見てください。この部分で、型`X`で独自に定義した引数らにデータを与えています。`__LINE__`と`__FILE__`は、予め標準で定義されてあるマクロであり、それぞれ該当する行数と、自身のファイル名が格納されており、それらの情報を転送する事で、このようなカスタマイズを達成しています。ただ今回は簡略化するために、領域確保に失敗した場合に単に文字列を出力するようにしていますが、本来であれば例外を投げるのが適切でしょう。例外についての詳細は後術しています。

### Non-allocating forms

次に二つ目です。

さて、ここまでで説明してきた動的な領域確保は、主に**ヒープ領域から**使うぶんの領域を確保していました。ここで、ヒープ領域から単純に領域を確保する際の短所を考察してみます。

* ヒープ領域から領域を確保するのは一般的にスタック領域から確保するよりも低速です。
* new演算子、malloc系関数を用いたヒープ領域からのリソース確保の方法は処理系のメモリ管理アルゴリズムに依存するため、ユーザー定義な独自の管理を行う事ができない

これらを解消するためには、ヒープ領域でない部分から領域を取ってくると良いかもしれません。

placement new構文は、それを可能にします。まずは領域をスタック領域から確保して、それを使うという事をしてみましょう。それをするためには、new/deleteをplacement newの形でオーバーロードしなければなりません。やっていることは、確保済みの領域から領域を取って来るといったところでしょうか。コードを見てしまった方が理解に容易いでしょう。以下に、方法を示します。

```cpp
#include<iostream>

struct X{
    X(){std::cout<<"ctor"<<std::endl;}
    ~X(){std::cout<<"dtor"<<std::endl;}

    void* operator new(std::size_t,void* ptr){return ptr;}
    void operator delete(void*,void*)noexcept{}
};

int main()
{
    char resource[sizeof(X)];
    X* ptr=new(resource) X;
    ptr->~X();
}
```

実行結果は以下の通りです。
```cpp
ctor
dtor
```
まず`main`内では、char型の配列を用意しています。そのサイズは、`X`型と同じバイト長です。char型は必ず1 byteであることが定められていますから、その領域ぶんのサイズを確保できれば、当然ながら互換性がある事になります。今回は、あらかじめ用意した領域は`X`型1つ分ということになります。

次に`X`型内部を見て見ましょう。new/delete演算子をオーバーロードしていることが伺えます。先ほどのusual new/deleteとは異なり、第二引数で`void*`を受け取っていますね。
placement new/deleteをオーバーロードする際はシグネチャが以下のようにならなくてはなりません。
```cpp
void* operator new(std::size_t,Args...);
void* operator delete(std::size_t,Args...)noexcept;
```
`Args...`というのは、任意の型で任意のパラメータ数であるという意味です。usual new/deleteと同じく、placement new/deleteも互いに対となるplacement new/deleteを適切に定義しなければなりません。

最後に注目して欲しいのが、`main`関数内でデストラクタを明示的に呼び出していること、またplacement newに対するplacement deleteを呼び出していないです。

まずplacement newは、**placement newによって新たに領域を確保しているわけではない**ので、deleteする必要がないのです。ただ、placement deleteを呼び出すことはできます。しかし呼び出したところで何も起こりません。
```cpp
char resource[sizeof(X)];
X* ptr=new(resource) X;
X::operator delete(ptr,ptr);
```
しかし、このままだと、placement operator newが行う、オブジェクトの生成、つまりコンストラクタの呼び出しに対して行われるべき、デストラクタの呼び出しが行われなくなってしまうため、デストラクタを明示的に呼び出す必要があるのです。
しかし、placement operator deleteは呼び出す必要もないのになぜ定義しておかなければならないのでしょうか。それは、placement operator newの後、コンストラクタで例外が投げられた場合にまず対となるoperator deleteが呼び出されるためです。

尚上記のコードは、`<new>`ヘッダーに予め以下のように定義されている、グローバルなoperator placement new/deleteを使えば、上記のように、独自にクラス内でオーバーロードせずとも同様のののことが実現できます(`<new>`ヘッダには下記の他にも様々なoperator new/deleteに対するoperator overloadingがされていますが、placement new/deleteを特出して掲載しています)。
```cpp
// <new>ヘッダ内
void* operator new (std::size_t size, void* ptr) noexcept;
void* operator new[](std::size_t size, void* ptr) noexcept;
void operator delete (void* ptr, void*) noexcept;
void operator delete[](void* ptr, void*) noexcept;
```
これらを使ってみましょう。

```cpp
#include<iostream>
#include<new>

struct X{
    X(){std::cout<<"ctor"<<std::endl;}
    ~X(){std::cout<<"dtor"<<std::endl;}
};

int main()
{
    char resource[sizeof(X)];

    X* ptr=new (resource) X();
    ptr->~X();
}
```

実行結果は以下の通りです。

```cpp
ctor
dtor
```
このように、わざわざクラス内に独自的にplacement new/deleteをオーバーロードせずとも同様の内容が達成できます。これで事足りるのであれば、独自的に実装するよりも、`<new>`ヘッダをインクルードしてそのコードを用いる方が、信頼性の面から見ても良いコードであると言えます。
尚、**placement new/deleteに対する定義は標準ライブラリヘッダー(`<new>')の単位で定義されるため、翻訳単位で定義されるグローバルなusual new/deleteとは異なり、上書きするようなことはできません**。もしそうしたいのであれば、上記のように、クラス内でoperator placement new/deleteをオーバーロードすることで実現します。

このようにplacement newを使ってメモリ割当てを行うシステムを、**アロケータ**と言い、アロケータによる管理の基使われる予定である予め確保された領域を**メモリプール**と言います。標準で用意されたライブラリの中でも、独自のアロケータでメモリ管理ができるように、設定の引数が設けられていたりするのですが、詳細は第12章のSTLと標準ライブラリで取り上げることとします。

さて、ここまでスタック領域からリソースを取れるということを売りにしてきたplacement newですが、ヒープ領域、静的領域からも領域を取得することは可能です。

```cpp
#include<new>

struct X{};

char resource[sizeof(X)];

int main()
{
    X* ptr=new X;
    ptr=new(ptr) X; // ヒープから取得してある領域をplacement newで割り当てる。同じアドレスなので上書きしても大丈夫
    ptr->~X();
    delete ptr;

    ptr=new(resource) X; // 静的領域のresourceで割り当てる。
    ptr->~X();
}
```
スタック領域の確保は、どうしても固定長\(コンパイル時に定まった値\)でなければならないため、メモリプールの領域も可変にしたい、しかし連続的に領域の確保/破棄がしたいためにメモリプールは欲しいといった場合では対応できません。そう行った場合は、以下のようにヒープ領域からメモリプールを確保してそこをplacement newするという手段も考えられます。

### メモリリソースの断片化

ここで一度、メモリリソースの断片化について取り上げます。

**リソース領域から、多くの回数領域の確保、解放の操作を繰り返すと、断片化が発生してしまい**、希望する領域サイズ分が、リソース中の空き領域の合計サイズと同等以上あったとしても、連続した領域で確保する事ができないため\(ある程度のサイズのデータ領域を確保しようとした時、私たちに与えられている領域確保の方法は配列であり、配列は連続した領域に確保されるという事を思い出してください。\)、領域の確保に失敗すると行った事が起こり得るようになります。

断片化していくメモリの様子を図に示しました。この領域確保の手法は、**ファーストフィット**と呼ばれるもので、主に一番最初に見つかった確保可能な領域を使用するアルゴリズムです。

![](/assets/firstfit.jpeg)


このような事を防ぐためには、二つの方法が考えられますね。

* 断片化を防止する
* 断片化したリソースを整理し再利用可能にする

まず断片化がおきないように領域確保を行うこと、これは可能であれば最も好ましい解決方法でしょう。後述する断片化したリソースの再整理は、再整理するだけの処理能力をやはり覆わなくてはなりません。それをなしに、初めから効率の良いリソース確保ができるのであれば、それが最も良いパフォーマンスの出し方であることは想像に容易いでしょう。しかし、断片化が起きないようにするリソース確保の手法は一概にこれであるとすることはできません。何故ならば、最適な確保の仕方も、要求される条件や処理系に依存するからです。前述したファーストフィットに加え、ベストフィットといった空き容量中で要求を満たすサイズ以上の最も近いサイズ領域を使うというアルゴリズムなど、単純な手法の他にも様々な活用方法が考案されています。よって、ここでは深く触れませんが、その概要がわかりやすく[こちらのスライド](https://www.slideshare.net/alwei/ss-11521742)で紹介されていますので、興味のある方は調べてみるのも良いでしょう。

次に、断片化を整理する、**デフラグ**という操作を行う事で再利用可能にする方法です。デフラグの手法は様々ですが、内容としては断片化したリソース領域を一方方向に全てずらして詰めてあげる\(メモリコンパクション\)といった具合に行います。デフラグの方法については、まだ説明していませんが、主に演算子オーバーロードなどを用いて領域確保の手法や管理方法を各個人が再定義する事で可能となりますが、これについての詳細はまた一つ大きな分野であるため、本章では取り扱いません。

