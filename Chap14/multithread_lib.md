# 14.2 C++17 におけるマルチスレッドプログラミング

この項では、[14.1 イントロダクション](/Chap14/multithread_intro.md)で会得した知識を、実際のライブラリを学習しながらコードに落とし込んでいきます。また、アトミック型におけるメモリフェンスの違いをマスターし、そこから見えてくるメモリモデルについて考えていきます。

## 14.2.1 std::thread
何はともあれ、まずはスレッドを新たに作れなければマルチスレッドプログラミングは始まりません。スレッドは、`std::thread`で作成する事ができます。`<thread>`をインクルードする事で利用できます。
```cpp
#include<thread>
```
スレッドは、コンストラクタに提供された関数(呼び出し可能なオブジェクトなど、つまり Callable コンセプトに合致した型を含む)から開始して関連するスレッドオブジェクトの構築時に即座に(OS によるスケジューリングによって遅延される可能性があります)実行を開始します。この時実行される関数から戻り値は虫され、例外を投げて終了すると`std::terminate`が呼び出されます。`std::thread`から戻り値を得るには、スレッド間で共有な変数、または後に取り上げる`std::promise`を利用して戻り値または例外を呼び出し元に通知する事ができます。早速、まずはスレッドを作成して実行してみましょう。
```cpp
#include<thread>
#include<iostream>
#include<functional>
#include<chrono>
#include<utility>

void f()
{
    for(std::size_t i = 0; i < 5; ++i){
        std::cout << "Thread (" << __func__ << ") executing\n";
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void g(int& n)
{
    for(std::size_t i = 0; i < 5; ++i){
        std::cout << "Thread (" << __func__ << ") executing\n";
        ++n;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

struct X{
    void Xf()
    {
        for(std::size_t i = 0; i < 5; ++i){
            std::cout << "Thread (" << __func__ << ") executing\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
};

int main()
{
    const auto h = []{
        for(std::size_t i = 0; i < 5; ++i){
            std::cout << "Thread (" << __func__ << ") executing\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    };
    int n = 0;
    
    {
        std::thread th1; // 空。実行されない。
    }

    {
        std::thread th2(f); // 関数 f をスレッドの処理内容として設定、開始
        th2.join();
    }
    
    {
        std::thread th3(g,std::ref(n)); // 引数を設定する
        std::thread th4(std::move(th3)); // th4 が g を実行する。ムーブされた th3 は空となる。
        th4.join();
        std::cout << "Final value of n is " << n << "\n";
    }
        
    {
        std::thread th5(h); // ラムダも当然渡せる
        th5.join();
    }

    {
        X x;
        std::thread th6(&X::Xf,x); // メンバ関数をスレッドにする。
        th6.join();
    }
}
```
出力例は以下となります。
```cpp
Thread (f) executing
Thread (f) executing
Thread (f) executing
Thread (f) executing
Thread (f) executing
Thread (g) executing
Thread (g) executing
Thread (g) executing
Thread (g) executing
Thread (g) executing
Final value of n is 5
Thread (operator()) executing
Thread (operator()) executing
Thread (operator()) executing
Thread (operator()) executing
Thread (operator()) executing
Thread (Xf) executing
Thread (Xf) executing
Thread (Xf) executing
Thread (Xf) executing
Thread (Xf) executing
```
Callable コンセプトに基づいた呼び出し可能な関数らを設定し、スレッドを開始できている事が分かると思います。`std::thread`のオブジェクトはコピーする事はできませんが、ムーブする事ができます。また、設定する関数及び設定する引数の全ての型はムーブコンストラクト可能でなければなりません。尚、新しいスレッドの作成に失敗した、つまり`std::thread`の対して関数を設定した時にコンストラクトに失敗した場合、`std::system_error`例外(`std::resource_unavailable_try_again`)が投げられます。また、上記コードのように`join`というメンバ関数を呼び出す事でそんスレッドの完了を待機する事ができます。これを**`join`操作**と言います。同関数が呼び出された時点で`std::thread`のオブジェクトに関連づけられたスレッドがまだ処理継続中だった場合、そのスレッドが完了するまで呼び出し元のスレッドがブロックされます。そして`join`操作が完了すると、`std::thread`は空の状態となります。`join`関数もコンストラクタ同様、`join`操作に失敗した場合、`system_error`例外(`std::no_such_process`または`std::invalid_argument`、デッドロックを検出した場合及び`get_id() == this_thread::get_id()`である場合、`std::resource_deadlock_would_occur`)が投げられます。もう一つ、基本的な操作として`detach`というものがあります。
```cpp
#include<thread>
#include<iostream>
#include<chrono>

void f()
{
    std::cout << "starting concurrent thread\n";
    std::this_thread::sleep_for(std::chrono::seconds(2));
    std::cout << "exiting concurrent thread\n";
}

void thread_caller()
{
    std::thread th(f);
    th.detach();
    std::cout << "non-blocking exit\n";
}

int main()
{
    thread_caller();
    std::this_thread::sleep_for(std::chrono::seconds(5));
}
```
筆者の環境では以下のように出力されました。
```cpp
starting concurrent thread
non-blocking exit
exiting concurrent thread
```
`detach`は、`std::thread`のオブジェクトと関連づけられた実行スレッドを分離して実行を独立して続行できるようにします。これを**`detach`操作**と言います。`join`とは異なり、呼び出し元のスレッドをブロックしない事が出力結果からも分かると思います。`detach`操作を行うと、他のスレッドから直接干渉する事ができなくなります(後に説明する`std::mutex`や`std::future`オブジェクトを利用して間接的に同期する事はできます)。`detach`操作に失敗した場合、`std::system_error`例外(`std::no_such_process`または`std::invalid_argument`)が投げれらます。`detach`メンバ関数を呼び出すと、`std::thread`のオブジェクトは空の状態となります。割り当てられたリソースはスレッドが終了すると共に解放されるようになっています。


尚、空の状態である`std::thread`オブジェクトから`join`または`detach`メンバ関数を呼び出すと`std::system_error`例外(`std::invalid_argument`または`std::no_such_process`)例外が投げられます。メンバ関数を呼び出す空でない`std::thread`を構築した場合は必ず`join`、または`detach`メンバ関数を呼び出す必要があります。呼び出さなかった場合、`std::terminate`が呼び出されます。`join`または`detach`が可能であるか、つまりスレッドオブジェクトがアクティブな実行スレッドを保持しているかどうかをチェックするには、`joinable`メンバ関数を利用します。このメンバ関数から例外が投げられる事はありません。
```cpp
#include<thread>
#include<iostream>
#include<chrono>

int main()
{
    std::thread th1;
    std::cout << "empty: " << std::boolalpha << th1.joinable() << std::endl;

    th1 = std::thread([]()noexcept{});
    std::cout << "after starting: " << th1.joinable() << std::endl;
    th1.join();
    std::cout << "after joining: " << th1.joinable() << std::endl;
}
```
出力例は以下の通りです。
```cpp
empty: false
after starting: true
after joining: false
```
尚、上記コードから分かるように、`std::thread`はムーブ版`operator =`も定義されています。これは、例外を投げません。ムーブが適用される側の`std::thread`のオブジェクトは、空の`std::thread`オブジェクトでなければなりません。そうでない場合、`std::terminate`が呼びだされます。更に基本的な操作である`swap`も用意されています。これも、例外を投げません。
```cpp
#include<thread>
#include<iostream>
#include<chrono>

int main()
{
    std::thread th1,th2([]()noexcept{});
    std::cout << "th1: " << std::boolalpha << th1.joinable() << std::endl;
    std::cout << "th2: " << th2.joinable() << std::endl;

    th1.swap(th2);
    std::cout << "th1: " << th1.joinable() << std::endl;
    std::cout << "th2: " << th2.joinable() << std::endl;

    th1.join();
}
```
出力例は以下の通りです。
```cpp
th1: false
th2: true
th1: true
th2: false
```
また、`get_id`メンバ関数によってスレッドの識別子を取得する事もできます。識別子は、`std::thread::id`型で、`std::thread`のオブジェクトがスレッドに関連づけられている場合はそのスレッドのスレッド識別子、そうでなければデフォルトコンストラクトされた`std::thread::id`型を得る事ができます。このメンバ関数は例外を送出しません。
```cpp
#include<thread>
#include<iostream>
#include<chrono>

int main()
{
    std::thread th1;
    std::cout << "empty: " << std::boolalpha << (std::thread::id() == th1.get_id()) << std::endl;

    th1 = std::thread([]()noexcept{});
    std::cout << "after starting: " << (std::thread::id() == th1.get_id()) << std::endl;
    th1.join();
    std::cout << "after joining: " << (std::thread::id() == th1.get_id()) << std::endl;
}
```
出力例は以下の通りです。
```cpp
empty: true
after starting: false
after joining: true
```
また、スレッドを用いたプログラミングではどうしても環境依存の処理が必要となる事があります。そのような場合、`native_handle`メンバ関数によってスレッドのハンドルを取得する事ができます。下記では例として`pthread_t`を取得しています。
```cpp
#include<thread>
#include<pthread.h>
#include<iostream>

int a = 0;

void f(int n)
{
    sched_param sch;
    int policy;
    pthread_getschedparam(pthread_self(),&policy,&sch);
    a += n;
}

int main()
{
    std::thread th1(f,42);
    sched_param sch;
    int policy;

    pthread_getschedparam(th1.native_handle(),&policy,&sch);
    sch.sched_priority = 20;

    if(pthread_setschedparam(th1.native_handle(),SCHED_FIFO,&sch)){
        std::cerr << "failed" << std::endl;
    }
    th1.join();

    std::cout << a << std::endl;
}
```
出力例は以下の通りです。
```cpp
42
```
次に`hardware_concurrency`静的メンバ関数を紹介します。これは、処理系によってサポートされるスレッドの並行数を取得する事ができるという関数ですが、処理系によって値が取得できなかった場合は`0`が返ってくるため、ヒント程度としてのみ利用する事しかできません。
```cpp
#include<thread>
#include<iostream>

int main()
{
    std::cout << std::thread::hardware_concurrency() << std::endl;
}
```
筆者の環境では、以下のように出力されました。
```cpp
4
```
最後に`std::thread`クラスのデストラクタの挙動についてです。`std::thread`のオブジェクトが`joinable() == true`である場合、`std::terminate`が呼び出され、それ以外の場合は何もしません。ここまでで、`std::thread`クラスにおける機能を一通り見てきましたが、一つだけ注意しなければならないものがありました。それは、空でないスレッドオブジェクトを生成した場合は必ず`join`または`detach`メンバ関数を呼び出さなければならないという事です。これらが呼び出されない場合`std::terminate`によって強制終了されてしまいます。例えば以下のように、`join`もしくは`detach`までの間で例外が投げられた場合、それらが呼び出されない事となってしまいます。
```cpp
std::thread th([]{ /* ... */ });
do_something(); // ここで例外が投げられた場合
th.join(); // join が呼び出されない
```
これを防ぐために、以下のようにRAIIを用いる事で確実に呼び出されるようにする事ができます。
```cpp
#include<thread>
#include<iostream>
#include<stdexcept>
#include<functional>

class scoped_guard{
    std::thread& t_;
public:
    explicit scoped_guard(std::thread& t):t_(t){}
    explicit scoped_guard(std::reference_wrapper<std::thread> ref):t_(ref.get()){}
    ~scoped_guard()
    {
        if(t_.joinable()){
            t_.join();
            std::cout << "joined" << std::endl;
        }
    }
    scoped_guard(const std::thread&) = delete;
    scoped_guard& operator=(const std::thread&) = delete;
};

void f()
{
    throw std::runtime_error("throw");
}

void thread_caller()
{
    std::thread th([]()noexcept{});
    scoped_guard scg(th);

    f();
    th.join();
}

int main()
{
    try{
        thread_caller();
    }catch(const std::runtime_error& er){
        std::cerr << er.what() << std::endl;
    }
}
```
出力例は以下の通りです。
```cpp
joined
throw
```
単純にデストラクタで`joinable`を検査して`join`または`detach`が呼ばれていない場合は`join`させています。これについては本章では少し余談となりますが、Boost C++ Libraries ではこのような仕組みをライブラリとして提供しています。例えば、`scoped_exit`などは上記のような RAII を簡単に記述する事ができます。
```cpp
#include <boost/scope_exit.hpp>
#include <iostream>
#include <stdexcept>
#include <thread>

void f()
{
    throw std::runtime_error("throw");
}

void thread_caller()
{
    std::thread th([]() noexcept {});
    BOOST_SCOPE_EXIT((&th))
    {
    if (th.joinable()) {
        th.join();
        std::cout << "joined" << std::endl;
    }
    }
    BOOST_SCOPE_EXIT_END

    f();
    th.join();
}

int main()
{
    try {
    thread_caller();
    } catch (const std::runtime_error &er) {
    std::cerr << er.what() << std::endl;
    }
}
```
尚 Boost C++ Libraries については[第15章](/chapter_15.md)で取り上げています。また、[16.x 次期 C++バージョン C++20について](/Chap16/cxx20.md)でも取り上げていますが、上記のような RAII の仕組みを用いてガードするライブラリが[提案されています](https://github.com/PeterSommerlad/SC22WG21_Papers/tree/master/workspace/P0052_scope_exit)。


少し寄り道してしまいましたが、ここで`<thread>`ヘッダに定義されている現在のスレッドに対するユーティリティ関数を紹介しておきましょう。まずは、`std::this_thread::get_id`です。これは、現スレッドの識別子を取得する事ができます。戻り型は`std::thread::id`で、同関数は例外を投げません。得られる`std::thread::id`の値は必ず`std::thread::id`のデフォルトコンストラクトの値ではありません。
```cpp
#include <thread>
#include <iostream>

int main()
{
    std::cout << std::this_thread::get_id() << std::endl;
}
```
筆者の環境では以下のように出力されました。
```cpp
0x7fffe6f343c0
```
次に、`std::this_thread::yield`関数を紹介します。これは、実装のヒントを提供してスレッドの実行を再スケジューリングし他のスレッドを実行できるようにします。この機能の正確な動作は実装、特にOSスケジューラのメカニックとシステムの状態によって異なります。例えば Linux の `SCHED_FIFO` は、現在のスレッドを一時停止して実行準備ができている優先順位の同じスレッドのキューの後ろの配置します。他にスレッドがない場合、スレッドは同じ優先順位であるため、結果は何の効果ももたらしません。
```cpp
#include <chrono>
#include <iostream>
#include <thread>

void little_sleep(std::chrono::microseconds s)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto end = start + s;
    do {
    std::this_thread::yield();
    } while (std::chrono::high_resolution_clock::now() < end);
}

int main()
{
    auto start = std::chrono::high_resolution_clock::now();

    little_sleep(std::chrono::microseconds(100));
    auto elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << "waiting for " << std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count() << " microseconds" << std::endl;
}
```
筆者の環境では以下のように出力されました。
```cpp
waiting for 100 microseconds
```
次に、`std::this_thread::sleep_for`についてです。これは、少なくとも指定された間、現在のスレッドの実行をブロックするものです。
```cpp
#include <chrono>
#include <thread>
#include <iostream>

int main()
{
    using namespace std::chrono_literals;

    std::cout << "good night" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(2s);
    auto end = std::chrono::high_resolution_clock::now();
    
    
    std::cout << std::chrono::duration<double,std::milli>(end - start).count() << std::endl;
}
```
筆者の環境では以下のように出力されました。
```cpp
good night
2002.77
```
`std::this_thread::sleep_for`は、スケジューリングまたはリソース競合の遅延のために実際に指定された間よりも長くブロックされる可能性があります。標準では、持続時間を測定するために安定したクロックを利用する事を推奨していますが、実装によってシステムクロックを代わりに代用する場合、待機時間はクロック調整にも影響される可能性があります。最後に、`std::this_thread::sleep_until`です。これは、指定された絶対時刻に達するまで現在のスレッドの実行をブロックします。指定された時刻に関連づけられた型のクロックが使用されます。
```cpp
#include <chrono>
#include <thread>
#include <iostream>

int main()
{
    using namespace std::chrono_literals;

    std::cout << "good night" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_until(start + 2s);
    auto end = std::chrono::high_resolution_clock::now();
    
    std::cout << std::chrono::duration<double,std::milli>(end - start).count() << std::endl;
}
```
筆者の環境では以下のように出力されました。
```cpp
good night
2004.62
```
さて、ここまでで、`std::thread`クラスにおける機能や関連する項目に触れてきました。これで、新たなスレッドが作れるようになり、そのスレッドに関する情報を得ることができるようになりました。ここからは、排他制御を駆使してより意味のあるマルチスレッドプログラミングを行なっていきましょう。

## 14.2.2 mutex
排他制御を行うミューテックス型について説明します。C++17 におけるミューテックス型は`std::mutex`、`std::recursive_mutex`、`std::timed_mutex`、`std::recursive_timed_mutex`、`shared_timed_mutex`があります。これらは`<mutex>`、`<shared_mutex>`をインクルードする事で利用できます。
```cpp
#include<mutex> // std::mutex, std::recursive_mutex, std::timed_mutex, std::recursive_timed_mutex
#include<shared_mutex> // std::shared_timed_mutex
```
では早速複数のスレッドを作成してデータ競合状態となるアクセスに`std::mutex`を使ってロック、アンロックを行い、排他制御をしてみましょう。
```cpp
#include <thread>
#include <mutex>
#include <iostream>

std::mutex m;

void f(int& n)
{
    for(std::size_t i = 0; i < 5; ++i){
        m.lock();
        ++n;
        m.unlock();
    }
}

int main()
{
    int n = 0;

    std::thread th1(f,std::ref(n)),th2(f,std::ref(n));

    th1.join();
    th2.join();

    std::cout << n << std::endl;
}
```
出力例は以下となります。
```cpp
10
```
まず`std::mutex`は、コピーまたはムーブする事はできません。また、コンストラクト時に`std::system_error`(ネイティブハンドル型の計算ができない場合`std::rerouce_unavailable_try_again`、スレッドにこの操作を行う権限がない場合`operation_not_permitted`、ネイティブハンドル型の計算がロックされている場合`device_or_resource_busy`、ミューテックスの構築で一部のネイティブハンドル型の計算が正しくない場合`invalid_argument`)例外を投げる可能性があります。`lock`メンバ関数によってミューテックスをロックします。別のスレッドが既にミューテックスをロックしている場合、ロックの呼び出しはロックが獲得されるまで実行をブロックします。ロックが既にミューテックスを所有しているスレッドによって呼び出された場合、未定義の動作を引き起こします。その一例にデッドロックの発生が考えられます。C++標準規格は、無効なロックの使用を検出できる場合、デッドロックするのではなく、エラー状態`resource_deadlock_would_occur`を持つ`std::system_error`をスローする実装を行う事を推奨します。また、`unlock`メンバ関数によってミューテックスのロックを解除します。ミューテックスは同メンバ関数の呼び出し元のスレッドによってロックされていなければなりません。そうでない場合、未定義の動作を引き起こします。尚、同じミューテックスの`unlock`操作は後に取り上げる`std::memory_order`によって、`lock` happens before `unlock`を保証します。<br>
もう1つ、`std::mutex`は、`try_lock`メンバ関数を持っています。これは、ミューテックスのロックを試行し。ロックに成功した場合`true`を返し、そうでない場合`false`を返します。ただしこの関数は、ミューテックスの所有権を保持しているスレッドが1つもない場合でも所有権の取得を失敗しても良い、つまり`false`を返しても良い事となっています。
```cpp
#include <thread>
#include <mutex>
#include <iostream>
#include <system_error>

class X{
    std::mutex m_;
    int value_ = 0;
public:
    friend std::ostream& operator<<(std::ostream& os,const X& x)
    {
        os << x.value_;
        return os;
    }

    void exclusion_increment()
    {
        if(!m.try_lock()){
            std::error_code ec(static_cast<int>(std::errc::device_or_resource_busy),std::generic_category());
            throw std::system_error(ec);
        }
        ++value_;
        m_.unlock();
    }
};

void f(X& x)
{
    for(std::size_t i = 0; i < 5; ++i){
        x.exclusion_increment();
    }
}

int main()
{
    X x;
    std::thread th1(f,std::ref(x)),th2(f,std::ref(x));

    th1.join();
    th2.join();

    std::cout << x << std::endl;
}
```
筆者の環境では、`false`を返したため、例外が投げられた後に`std::terminate`が呼ばれました。次に、`std::recursive_mutex`について説明します。`std::mutex`と同様、排他的なセマンティックスを提供するクラスですが、それに加えて`std::recursive_mutex`は、同一スレッド上からの再帰的なロックを管理する事のできるクラスです。`std::mutex`ではこのような事はできません。`std::recursive_mutex`は、その内部に所有権カウンタを保持しており、`lock`及び`try_lock`した回数と一致する回数`unlock`を呼び出す事でアンロックされます。尚、`std::recursive_mutex`が再帰的に所有権を獲得できる最大回数は特に規定されていませんが、`lock`メンバ関数による呼び出しにおいてその回数に達すると`std::system_error`例外が投げられます。また、その回数に達した段階で`try_lock`メンバ関数を呼び出すと`false`が返されます。`std::recursive_mutex`は`std::mutex`と同じように、`lock`、`try_lock`、`unlock`、`native_handle`メンバ関数が用意されています。早速使って見ましょう。
```cpp
#include <algorithm>
#include <iostream>
#include <iterator>
#include <mutex>
#include <thread>
#include <type_traits>
#include <vector>

template <class T>
class simply_safe_container {
    std::recursive_mutex lock_;
    std::vector<T> data_;

    template <class... Ts>
    using requirement =
    std::enable_if_t<
        std::conjunction_v<std::is_convertible<T, std::decay_t<Ts>>...> and !std::conjunction_v<std::is_same<simply_safe_container, std::decay_t<Ts>>...>,std::nullptr_t
    >;

public:
    simply_safe_container() = default;

    template <class... Ts, requirement<Ts...> = nullptr>
    simply_safe_container(Ts &&... ts) : lock_{}, data_{std::forward<Ts>(ts)...} {}

    template <class U, class... Ts, requirement<U, Ts...> = nullptr>
    void emplace_back(U &&first, Ts &&... last)
    {
    	lock_.lock();
    	data_.emplace_back(std::forward<U>(first));
    	if constexpr (bool(sizeof...(last))) {
		emplace_back(std::forward<Ts>(last)...);
    	}
    	lock_.unlock();
    }

    friend std::ostream &operator<<(std::ostream &os, simply_safe_container &this_)
    {
    	this_.lock_.lock();
    	std::copy(std::begin(this_.data_), std::end(this_.data_), std::ostream_iterator<T>(os, " "));
    	this_.lock_.unlock();

    	return os;
    }
};

void f(simply_safe_container<int> &s)
{
    s.emplace_back(1, 2, 3);
}

int main()
{
    simply_safe_container<int> ssc{1, 2, 3};

    std::thread th1(f, std::ref(ssc)), th2(f, std::ref(ssc)), th3(f, std::ref(ssc));
    th1.join();
    th2.join();
    th3.join();

    std::cout << ssc << std::endl;
}
```
出力例は以下の通りです。
```cpp
1 2 3 1 2 3 1 2 3 1 2 3
```
再帰的にロックを取得してもデッドロックが発生しない(未定義動作が起こらない)事がわかります。次に、`std::timed_mutex`について説明します。`std::timed_mutex`は`std::mutex`と同様、排他的で非再帰的な所有権セマンティックスを提供するクラスですが、それに加えて`std::timed_mutex`は、`try_lock_for`、`try_lock_until`メンバ関数を使用してロック取得におけるタイムアウト機能を提供します。`try_lock_for`には`std::chrono::duration`を用いてタイムアウトする相対時間を、`try_lock_until`には`std::chrono::time_point`を用いてタイムアウトする絶対時間を指定します。尚、`try_lock_for`において指定された`std::chrono::duration`のオブジェクトが、`.zero()`以下である場合、`try_lock`メンバ関数と同じ効果を持ちブロッキングせずにロック取得を試みます。それ以外の場合、`try_lock_for`はロックが取得されるまで、または`std::chrono::duration`オブジェクトによって指定された時間が経過するまでブロックします。他のスレッドんいよってロックが取得されていなくても取得に失敗する事があります。いずれの場合でも、ロックの取得に成功すれば`true`が返り、それ以外の場合は`false`が返ります。
また、`try_lock_until`は`std::chrono::time_point`オブジェクトによって指定された時間までに残った時間内にロック取得を試みます。指定された時間に既に達していた場合、このメンバ関数は`try_lock`メンバ関数と同じ効果を持ち、ブロッキングせずにロック取得を試みます。それ以外の場合、`try_lock_until`メンバ関数は、ロックが取得されるか、指定された時間が経過するまでブロックします。指定された時間の前のある時点で他のスレッドがロックしていなくても、ロック取得に失敗する事があります。いずれの場合でも、ロック取得に成功した場合`true`が返り、それ以外の場合は`false`が返ります。それぞれ使って見ましょう。
```cpp
#include <chrono>
#include <iostream>
#include <mutex>
#include <system_error>
#include <thread>

class X {
    std::timed_mutex m_;
    int value_ = 0;

public:
    friend std::ostream &operator<<(std::ostream &os, const X &x)
    {
    	os << x.value_;
    	return os;
    }

    void exclusion_increment()
    {
    	if (!m_.try_lock_for(std::chrono::seconds(2))) { // !m_.try_lock_until(std::chrono::system_clock::now() + std::chrono::seconds(2)) と同様
        	std::error_code ec(static_cast<int>(std::errc::device_or_resource_busy), std::generic_category());
        	throw std::system_error(ec);
    	}
    	++value_;
    	m_.unlock();
    }
};

void f(X &x)
{
    for (std::size_t i = 0; i < 5; ++i) {
    	x.exclusion_increment();
    }
}

int main()
{
    X x;
    std::thread th1(f, std::ref(x)), th2(f, std::ref(x));

    th1.join();
    th2.join();

    std::cout << x << std::endl;
}
```
出力例は以下の通りです。
```cpp
10
```
次に、`std::recursive_timed_mutex`について説明します。`std::recursive_timed_mutex`は、`std::recursive_mutex`のように再帰的にロックが可能かつロック取得のタイムアウト機能を提供するクラスです。`std::timed_mutex`同様、`try_lock_for`、`try_lock_until`メンバ関数が用意されており、これらを用いて再帰的なロック管理を行う事ができます。早速使ってみましょう。
```cpp
#include <chrono>
#include <iostream>
#include <mutex>
#include <system_error>
#include <thread>

class X {
    std::timed_mutex m_;
    int value_ = 0;

public:
    friend std::ostream &operator<<(std::ostream &os, const X &x)
    {
    	os << x.value_;
    	return os;
    }

    void exclusion_increment()
    {
    	if (!m_.try_lock_for(std::chrono::seconds(2))) {
        	std::error_code ec(static_cast<int>(std::errc::device_or_resource_busy), std::generic_category());
        	throw std::system_error(ec);
    	}
    	++value_;
    	m_.unlock();
    }
};

void f(X &x)
{
    for (std::size_t i = 0; i < 5; ++i) {
    	x.exclusion_increment();
    }
}

int main()
{
    X x;
    std::thread th1(f, std::ref(x)), th2(f, std::ref(x));

    th1.join();
    th2.join();

    std::cout << x << std::endl;
}
```
出力例は以下の通りです。
```cpp
1 2 3 1 2 3 1 2 3 1 2 3
```
ところで、ミューテックスのアンロックが実行されなかった場合、未定義の動作を引き起こしてしまいます。`std::thread`の`join`、`detach`を RAII に管理させるのと同じようにこれも RAII で管理させてしまえば良いのではないでしょうか。というわけで、そのような機能は既に標準ライブラリによって用意されています。`std::lock_guard`、`std::unique_lock`、`std::scoped_lock`がそれです。まず`std::lock_guard`から見ていきます。`std::lock_guard`は、前述した通り RAII のスタイルでミューテックスを管理します。`std::lock_guard`オブジェクトに`std::mutex`オブジェクトを指定すると、その所有権を取得します。`std::lock_guard`オブジェクトが作成されたスコープを離れた時、`lock_guard`が破棄されるため、それと同時にミューテックスが解放されます。尚、`std::lock_guard`クラスはコピーできません。
```cpp
#include <iostream>
#include <mutex>
#include <thread>

class X{
    std::mutex m_;
    int value_ = 0;
public:
    friend std::ostream& operator<<(std::ostream& os,const X& x)
    {
        os << x.value_;
        return os;
    }

    void exclusion_increment()
    {
        std::lock_guard<decltype(m_)> lock(m_); // ロックを取得し、スコープを外れる段階でロックを解除する
        ++value_;
    }
};

void f(X& x)
{
    for(std::size_t i = 0; i < 5; ++i){
        x.exclusion_increment();
    }
}

int main()
{
    X x;
    std::thread th1(f,std::ref(x)),th2(f,std::ref(x));

    th1.join();
    th2.join();

    std::cout << x << std::endl;
}
```
出力例は以下の通りです。
```cpp
10
```
尚、コンストラクト時に第二引数に対して`std::adopt_lock`を指定する事で、ミューテックスの所有権をロックせずに取得させる事ができます。つまり、自らのスレッド上で予めロックしてあるミューテックスのロック解放を`std::lock_guard`に対して任せたい場合に利用する事ができます。コンストラクト時に`std::adopt_lock`を指定しておきながらも予め自らのスレッド上でロックを取得していなかった場合、未定義の動作を引き起こします。
```cpp
// 略...
void exclusion_increment()
{
    m_.lock();
    std::lock_guard<decltype(m_)> lock(m_,std::adopt_lock); // コンストラクト時にロックを取得させない。
    ++value_;
}
```
出力例は変わりません。尚、`std::lock_guard`は deduction guides が宣言されているため以下のようにテンプレート引数を省略する事ができます。
```cpp
std::lock_guard lock(m_);
```
`std::lock_guard`よりさらに高度なロック制御を行いたい場合、`std::unique_lock`を使用します。`std::unique_lock`は、コンストラクト時点でロックを取得する事もできますが、そうではなく後でロックを取得するように設定する事ができます。また、ロック取得時に、`lock`ではなく、`try_lock`を使用させるよう指示する事ができます。また、ミューテックスの所有権の移動、放棄と任意のタイミングでの所有ミューテックスのロック操作が行えます。まずは単純にコンストラクト時にロックし、デストラクト時にロックを解除する例です。単に上記の`std::lock_guard<decltype(m_)>`を`std::unique_lock<decltype(m_)>`とするだけです。
```cpp
// 略...
void exclusion_increment()
{
    std::unique_lock<decltype(m_)> lock(m_); // ロックを取得しスコープを抜ける段階でロックを解除する
    ++value_;
}
```
実行結果も変わりません。この場合ならば、処理が無駄になってしまいますので`std::lock_guard`を用いた方が良いでしょう。次に、ロックを遅延させてみます。
```cpp
// 略...
void exclusion_increment()
{
    std::unique_lock<decltype(m_)> lock(m_,std::defer_lock); // コンストラクト時ではロックを取得しない。スコープを抜ける段階でロックを解除する
    lock.lock(); // ロック
    ++value_;
}
```
第二引数に`std::defer_lock`を指定する事でコンストラクト時にロックが取得されなくなります。また、`std::lock_guard`のコンストラクト時と同じく`std::adopt_lock`が指定できる他、`std::try_to_lock`が指定できます。`std::try_to_lock`は、ロックの取得に`try_lock`を呼び出します。
```cpp
// 略...
void exclusion_increment()
{
    m1_.lock();
    std::unique_lock<decltype(m1_)> lock1(m1_,std::adopt_lock); // コンストラクト時にロックを取得させない。
    std::unique_lock<decltype(m2_)> lock2(m2_,std::try_to_lock); // ロックに try_lock を使う
    ++value_;
}
```
更に、第二引数に`std::chrono::duration`または`std::chrono::time_point`を渡す事で、それぞれロックの取得に`try_lock_for`と`try_lock_until`を呼び出させるよう設定する事ができます。
```cpp
// 略...
void exclusion_increment()
{
    std::unique_lock<decltype(m1_)> lock1(m1_,std::chrono::seconds(2)); // コンストラクト時に try_lock_for を呼び出す
    std::unique_lock<decltype(m2_)> lock2(m2_,std::chrono::steady_clock::now() + std::chrono::seconds(2)); // コンストラクト時に try_lock_until を呼び出す
    ++value_;
}
```
尚、`std::unique_lock`は deduction guides が宣言されているため、以下のようにテンプレート引数を省略する事ができます。
```cpp
std::unique_lock lock(m1_);
```
また、`std::scoped_lock`というものもあります。これは、可変個のミューテックス型及びミューテックスオブジェクトを取る`std::lock_guard`です。つまり、任意の数のミューテックスを一度にロックしスコープを抜けるタイミングでロックを解除します。第一引数に`std::adopt_lock`を渡すと、ロック済みのミューテックスを設定する事もできます。
```cpp
// 略...
void exclusion_increment()
{
    std::scoped_lock<decltype(m1_),decltype(m2_)> lock(m1_,m2_); // コンストラクト時に二つのミューテックスをロック(lockを呼び出す)する
    m1_.lock();
    m2_.lock();
    std::scoped_lock<decltype(m3_),decltype(m4_)> lock(std::adopt_lock,m3_,m4_); // コンストラクト時にロックを取得しない
    ++value_;
}
```
尚、`std::scoped_lock`には deduction guides が宣言されていますので、以下のようにテンプレート引数を省略する事ができます。
```cpp
std::scoped_lock lock(m1_,m2_);
```
尚、ここまではミューテックス型のメンバ関数`lock`、`try_lock`を直接呼んでいましたが、それらの非メンバ関数版である`std::lock`、`std::try_lock`関数も用意されています。これらは引数が可変個受け取れるので、それぞれ渡したミューテックスを一度にロック及びアンロックします。`std::lock`は、渡された各ミューテックスオブジェクトに対して`lock`、`try_lock`、または`unlock`メンバ関数を順次呼び出すデッドロック回避アルゴリズムを用いて、全てのミューテックスをロックします。この際いずれかの`lock`、`try_lock`操作において例外が発生した場合、それ以降に行われる`lock`、`try_lock`呼び出しを行わずに、それ以前にロック取得した全てのミューテックスオブジェクトの`unlock`メンバ関数を呼び出してアンロックします。そして、`std::system_error`例外が送出されます。`std::try_lock`は、各ミューテックスオブジェクトの`try_lock`メンバ関数を順次呼び出します。この際いずれかの`try_lock`が`false`を返すか例外を送出した場合、以降の`try_lock`メンバ関数の呼び出しを行わずにそれ以前までに取得した全てのミューテックスオブジェクトの`unlock`メンバ関数を呼び出してアンロックします。全てのミューテックスオブジェクトへの`try_lock`メンバ関数の処理が成功した場合、`-1`を返し、失敗した場合その失敗した最初のミューテックスオブジェクトへの0から始まるインデックス値を返します。
```cpp
// 略
void exclusion_increment()
{
    std::lock(m1_,m2_); // デッドロック回避アルゴリズムを用いて全てのミューテックスのロックを取得する。
    std::scoped_lock lock(std::adopt_lock,m1_,m2_); // ロック取得済みなので std::adopt_lock を指定して scoped_lock に管理を委任する。
    ++value_;    
}
```
```cpp
// 略
void exclusion_increment()
{
    if(std::try_lock(m1_,m2_) != -1){
        std::error_code ec(static_cast<int>(std::errc::device_or_resource_busy), std::generic_category());
        throw std::system_error(er);
    }
    std::scoped_lock lock(std::adopt_lock,m1_,m2_); // 同上
    ++value_;
}
```
ところで、ここまで一連の機能を述べておきながらですが、排他制御におけるロックとアンロックの操作は、最適化におけるリオーダーなどを考慮する必要がないのでしょうか。C++ 標準規格では、全てのミューテックスクラスにおける、`lock`系メンバ関数、`lock`非メンバ関数と`unlock`の間に順序関係が成立する事が保証されています。よって**起こらない**と言えます。<br>
最後に`std::call_once`という関数を紹介します。これは、指定された関数を一度だけ呼び出すもので、複数のスレッド間で共通使用するデータの初期化処理などで利用する事ができます。第一引数には`std::once_flag`型のオブジェクトを渡します。これは、初期状態であるかどうかを表すフラグです。第二引数には Callable コンセプトに基づく関数を渡し、第三引数からはその関数に与える引数を指定します。関数の呼び出しが例外を送出する場合、その例外は`call_once`を呼び出した地点に伝達され、フラグは初期状態のままとなります。送出される型はシステムのAPI における何らかのエラーである場合は`std::system_error`、また、その他第二引数に指定した関数から送出されるあらゆる例外型が送出される可能性があります。尚、第二引数に指定された関数が例外を送出する時、同じ`std::once_flag`オブジェクトを指定した次の`call_once`関数の呼び出しでは、その第二引数に指定された関数が呼び出されます。また、同じ`std::once_flag`を指定した複数の`call_once`関数呼び出しにおいては、指定された各関数の呼び出しは同時には行われず、順序関係が成立する事が保証されています。また、C++17 までは第二引数に指定した関数への引数は値コピー、ムーブされていたため、参照型を関数へ渡すためには、それを`std::reference_wrapper`などでラップして渡す必要がありましたが、C++17 からはそれらが Perfect Forwarding されるようになったため、そのような必要は無くなりました。早速、サンプルを見て見ましょう。
```cpp
#include <iostream>
#include <mutex>
#include <thread>

std::once_flag flag;
std::mutex m;

void init(int& n)
{
    n = 42;
}

void do_once()
{
    int n = 0;
    std::call_once(flag, init, n);
    std::lock_guard lock(m);
    std::cout << n << std::endl;
}

int main()
{
    std::thread th1(do_once), th2(do_once), th3(do_once);
    th1.join();
    th2.join();
    th3.join();
}
```
出力例は以下の通りです。
```cpp
42
0
0
```
以上が`<mutex>`ヘッダにて標準が提供する機能でした。次に、**condition variable (条件変数)**という概念を説明します。

## 14.2.3 condition\_variable
マルチスレッドの動作において、スレッド間で共有される情報がある条件を満たしたらあるスレッドが動いて欲しいという場合があります。ある条件を満たすまでそのスレッドにはサスペンドしていて欲しいですが、条件がみたされたのちに他のスレッドから処理を始める合図を行う仕組みが必要となります。このような場合、**condition variable (条件変数)** を利用します。**condition variable とは、ある特定の条件になった時にスレッドの停止と再開を指示するための変数です**。データ競合を防ぐためにあるミューテックスとは**全く異なる概念**です。実は理論的には、この condition variable という概念を用いなくとも同様な事は可能です。例えば、ある条件の状態を保持しているデータに対してループによって定期的にアクセスし、条件に達した段階で処理を続行する(このような実装をポーリング方式という)実装を行う事で実現できます。しかし、このようなポーリング方式では条件を満たしているかについて常にそのスレッドはアクセスし続けなければならないので、一般的な実行環境における実行効率の観点において良いとは言い難い場合があります。条件を満たした時に動き始めてもらって、それまでは特別サスペンドしている方が良いのであれば、ポーリング方式を採用する意義は全く見出せません。<br>
C++ では、`<condition_variable>`ヘッダーをインクルードする事で、この condition variable という概念における表現方法を統一しつつ効率的に記述できる`std::condition_variable`、`std::condition_variable_any`を利用する事ができます。<br>
まずは基本的な条件変数型である`std::condition_variable`から説明します。まず大事なのは、**`std::condition_variable`そのものは、条件とする変数を持ちません**。`condition_variable`という名前でありながら条件変数そのものではないというのは少しおかしいように感じるかもしれませんが、`std::condition_variable`のイメージとしては condition\_variable の所謂ラッパーという捉え方が分かりやすいかもしれません。`std::condition_variable`は条件変数そのもののデータを保持しているのではなく、条件変数という概念における操作のみをモデル化した機構であるとも言えます。ならば、条件はどうやって指定するのかというとこれは、ライブラリユーザーが通常通り記述する事となります。まず`std::condition_variable`には`notify_one`、`notify_all`というスレッドに起床を指示するメンバ関数と`wait`という起床されるまで待機するメンバ関数、また指定される時間に応じてタイムアウトする機能を持ち、起床されるまで待機する`wait_for`、`wait_until`というメンバ関数が用意されています。この中でも基本となる`notify_one`と`wait`を取り上げて、ライブラリの機能と同時に condition variable の概念について見ていきます。まずは、以下のコードを見てください。
```cpp
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <thread>

std::condition_variable cv;
std::mutex m;
bool b = false;

void waits()
{
    std::unique_lock lock(m);
    std::cout << "Waiting..." << std::endl;
    if (!b) {
    	cv.wait(lock);
    }
    std::cout << "finished waiting." << std::endl;
}

void signals()
{
    std::this_thread::sleep_for(std::chrono::seconds(2));
    {
    	std::lock_guard lock(m);
    	b = true;
    	std::cout << "Notifying... " << std::endl;
    }
    cv.notify_one();
}

int main()
{
    std::thread th1(waits), th2(signals);
    th1.join();
    th2.join();
}
```
まず初めに述べておきますと、**このコードは潜在的なバグを含んでいる NG なコードです**が、まずは各処理が何をしているのか解説します。まず、このコードにおける条件とは、「グローバル変数`b`が`true`になった時」です。これを踏まえてコードを見ていきます。まず、スレッドを二つ作成しています。各スレッドは`waits`と`signals`という関数を呼ぶようですね。`signals`関数は分かりやすいですね。まず処理の流れが視覚的に分かりやすいように2秒経過させています。その後、一段深いスコープに入り`std::lock_guard`によってロックを獲得し、グローバル変数`b`を`true`に変えています。その後何か出力してからスコープを抜けます。このタイミングで`signals::lock`によってロックが解除されます。その後、`std::condition_variable`のオブジェクト`cv`から`notify_one`メンバ関数を呼び出しています。`notify_one`メンバ関数は、その呼び出し元の関数を待っているスレッドがある場合、待機中のスレッドの内の**1つ**のブロックを解除するメンバ関数です。つまりこの部分でもう一つのスレッドである`th2`(`waits`)に対して起床を指示しているのです。尚、`notify_one`メンバ関数の呼び出しはミューテックスで保護する必要はありません。次に、待たされる方の`waits`関数を見て見ます。`std::unique_lock`によってロックを取得して何か出力した後、条件をチェックしてから`wait`メンバ関数に渡しています。**`wait`メンバ関数には内部のミューテックスが`std::mutex`であるロック取得済みの`std::unique_lock`オブジェクトを渡さなければならない**(`std::unique_lock<std::mutex>`のみのサポートであるため)ので、`std::unique_lock`でロックを取得しています。条件変数のその用法から、条件の状態を持っているデータへのアクセスを排他的としなければならない事は必然ですから、ロックが必要である事も自然です。その後、`b`が`false`である場合は`wait`するようにしています。まず`wait`メンバ関数が呼ばれると、他のスレッドがロックを取得できるようミューテックスはアンロックされます。そしてブロッキングが行われます。他のスレッドから起床を促されると、`wait`メンバ関数はブロックを辞め、ミューテックスのロックを再度取得します。これは、その後`waits`関数で続く処理を排他的にする事は勿論、ユーザーが明示的にロックを書く必要はなくなりますし、`wait`メンバ関数から例外が送出された場合のロック処理を明示的に書かなければならない点をライブラリ側が隠滅するためにあります。尚、取得されたロックは`unique_lock`にアンロックを委任しているため、`waits`関数を抜けるタイミングでアンロックされます。<br>
さて、ここまで考えてみると上記のコードには何の問題もないように思えますが、残念ながら**spurious wakeup**と言われる問題について、上記コードでは考慮できていないのです。

### spurious wakeup
spurious wakeup とは、条件変数でのwait 処理においてそれに対して起床を命じていないのにも関わらずブロックが解除される現象を言います。これはライブラリの内部実装やシステム、ハードウェア、OS の単位で行われ、条件変数を"利用する"立場からは関与する事のできない原因によって生じます。つまり、これは C++ 言語特有の問題ではないという事です。ではそもそも何故 spurious wakeup という仕組みがあるのかについての詳細は C++17 言語の解説からは少し離れている部分の解説を伴わなければならないため省きますが、簡単に言えば実行効率を落とさないための妥協点なのです。つまりプログラマー側が、これを受け入れてプログラムを書かなければなりません。先ほど掲載したコードから spurious wakeup が考慮されていない部分を抜きだした部分は以下の通りです。
```cpp
if (!b) {
    cv.wait(lock);
}
```
spurious wakeup によって`waits`関数が起きてしまった場合、この wait 処理はそれをそのまま受け入れて処理を再開してしまいます。重要なのは、グローバル変数`b`が`true`となっている時に処理を再開したいという事です。つまり、spurious wakeup によって`waits`関数が起きてしまったとしても、その後もう一度`b`の状態を見て、もし`false`であったらまた`wait`メンバ関数を呼び出して眠らせてしまえば良いのです。つまり、以下のようにします。
```cpp
while(!b) {
    cv.wait(lock);
}
```
これで例え spurious wakeup によって起こされてしまっても、`b`が`true`でない限りまた眠るので予期せぬ処理の続行を防ぐ事ができます。尚、このように状態を扱うデータをチェックする事は condition variable を用いたプログラミングにおける定番のイディオムなので、`std::condition_variable`ではそのイディオムを内包するバージョンの`wait`メンバ関数も用意されています。以下は上記の`while`で`b`の状態をチェックするコードと同等の処理を行います。
```cpp
cv.wait(lock,[]{return b;});
```
という事で先ほどのコードに加えて spurious wakeup が考慮されたコードは以下の通りです。
```cpp
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <thread>

std::condition_variable cv;
std::mutex m;
bool b = false;

void waits()
{
    std::unique_lock lock(m);
    std::cout << "Waiting..." << std::endl;
    cv.wait(lock, []{ return b; });
    std::cout << "finished waiting." << std::endl;
}

void signals()
{
    std::this_thread::sleep_for(std::chrono::seconds(2));
    {
    	std::lock_guard lock(m);
    	b = true;
    	std::cout << "Notifying... " << std::endl;
    }
    cv.notify_one();
}

int main()
{
    std::thread th1(waits), th2(signals);
    th1.join();
    th2.join();
}
```
さてここまでは、`notify_one`と`wait`メンバ関数を用いてきましたが、ここで`notify_all`、`wait_for`、`wait_until`について説明します。まず`notify_one`メンバ関数は前述した通り、待機中のスレッドの内の**1つ**のブロックを解除するもので、複数の待ちスレッドを起床させる事はできません。`notify_all`は全てのスレッドを起床させるメンバ関数です。早速使って見ましょう。
```cpp
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <thread>

std::condition_variable cv;
std::mutex m;
bool b = false;

void waits(const char* s)
{
    std::unique_lock lock(m);
    std::cout << s << " is Waiting..." << std::endl;
    cv.wait(lock,[]{return b;});
    std::cout << s << " finished waiting." << std::endl;
}

void signals()
{
    std::this_thread::sleep_for(std::chrono::seconds(2));
    {
    	std::lock_guard lock(m);
    	b = true;
    	std::cout << __func__ << ": Notifying... " << std::endl;
    }
    cv.notify_all();
}

int main()
{
    std::thread th1(waits,"th1"), th2(waits,"th2"), th3(waits,"th3"), th4(signals);
    th1.join();
    th2.join();
    th3.join();
    th4.join();
}
```
出力例は以下の通りです。
```cpp
th1 is Waiting...
th2 is Waiting...
th3 is Waiting...
signals: Notifying...
th1 finished waiting.
th2 finished waiting.
th3 finished waiting.
```
次に、`wait_for`、`wait_until`メンバ関数について説明します。二つとも前述したブロッキング効果を持ち、それぞれ相対時間(`std::chrono::duration`)と絶対時間(`std::chrono::time_point`)を指定し、指定された時間内に起床されない場合、タイムアウトとなり`std::cv_status::timeout`が返却され、そうでない場合は`std::cv_status::no_timeout`が返却されます。述語を渡すバージョンでは、返却値としては指定された関数の結果が返ります。まずは、`wait_for`メンバ関数の利用例を以下に示します。
```cpp
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <thread>

std::condition_variable cv;
std::mutex m;
bool b = false;

void waits1(const char* s)
{
    std::unique_lock lock(m);
    std::cout << s << " is Waiting..." << std::endl;
    while(!b){
        std::cv_status result = cv.wait_for(lock,std::chrono::seconds(2));
        if(result == std::cv_status::timeout){
            std::cout << "timeout" << std::endl;
            return;
        }
    }
    std::cout << s << " finished waiting." << std::endl;
}

void waits2(const char* s)
{
    std::unique_lock lock(m);
    std::cout << s << " is Waiting..." << std::endl;
    
    if(!cv.wait_for(lock,std::chrono::seconds(2),[]{return b;})){
        std::cout << "timeout" << std::endl;
        return;
    }
    std::cout << s << " finished waiting." << std::endl;
}

void signals()
{
    {
    std::lock_guard lock(m);
    b = true;
    std::cout << __func__ << ": Notifying... " << std::endl;
    }
    cv.notify_all();
}

int main()
{
    std::thread th1(waits1,"th1"), th2(waits1,"th2"), th3(waits2,"th3"), th4(waits2,"th4"), th5(signals);
    th1.join();
    th2.join();
    th3.join();
    th4.join();
    th5.join();
}
```
出力例は以下の通りです。
```cpp
th3 is Waiting...
th2 is Waiting...
th1 is Waiting...
th4 is Waiting...
signals: Notifying...
th3 finished waiting.
th2 finished waiting.
th1 finished waiting.
th4 finished waiting.
```
次に`wait_until`の利用例を以下に示します。
```cpp
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <thread>

std::condition_variable cv;
std::mutex m;
bool b = false;

void waits1(const char* s)
{
    std::unique_lock lock(m);
    std::cout << s << " is Waiting..." << std::endl;
    while(!b){
        std::cv_status result = cv.wait_until(lock,std::chrono::steady_clock::now() + std::chrono::seconds(2));
        if(result == std::cv_status::timeout){
            std::cout << "timeout" << std::endl;
            return;
        }
    }
    std::cout << s << " finished waiting." << std::endl;
}

void waits2(const char* s)
{
    std::unique_lock lock(m);
    std::cout << s << " is Waiting..." << std::endl;
    
    if(!cv.wait_until(lock,std::chrono::steady_clock::now() + std::chrono::seconds(2),[]{return b;})){
        std::cout << "timeout" << std::endl;
        return;
    }
    std::cout << s << " finished waiting." << std::endl;
}

void signals()
{
    {
    std::lock_guard lock(m);
    b = true;
    std::cout << __func__ << ": Notifying... " << std::endl;
    }
    cv.notify_all();
}

int main()
{
    std::thread th1(waits1,"th1"), th2(waits1,"th2"), th3(waits2,"th3"), th4(waits2,"th4"), th5(signals);
    th1.join();
    th2.join();
    th3.join();
    th4.join();
    th5.join();
}
```
出力例は以下の通りです。
```cpp
th1 is Waiting...
th2 is Waiting...
th3 is Waiting...
th4 is Waiting...
signals: Notifying...
th1 finished waiting.
th2 finished waiting.
th3 finished waiting.
th4 finished waiting.
```
`std::condition_variable`はそん特質上、`notify`を行わなければブロックされているスレッドは永遠に眠ったままです。これを防止するのに、`std::notify_all_at_thread_exit`という関数が用意されています。この関数を呼び出すと、その呼び出されたスレッドの終了時に全てのスレッドを起床させます。引数には`std::condition_variable`オブジェクトとロック済みである`std::unique_lock<std::mutex>`オブジェクトを渡します。ミューテックスオブジェクトはムーブさせなければなりません。スレッド終了時に移譲されたロックの`unlock`メンバ関数を呼び出してから`std::condition_variable`オブジェクトの`notify_all`メンバ関数を呼び出す事で同処理を実現します。
```cpp
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <thread>

std::condition_variable cv;
std::mutex m;
bool b = false;

void waits(const char* s)
{
    std::unique_lock lock(m);
    std::cout << s << " is Waiting..." << std::endl;
    
    if(!cv.wait_until(lock,std::chrono::steady_clock::now() + std::chrono::seconds(2),[]{return b;})){
        std::cout << "timeout" << std::endl;
        return;
    }
    std::cout << s << " finished waiting." << std::endl;
}

void signals()
{
    std::unique_lock lock(m);
    b = true;
    std::cout << __func__ << ": Notifying... " << std::endl;
    std::notify_all_at_thread_exit(cv,std::move(lock));
}

int main()
{
    std::thread th1(waits,"th1"), th2(waits,"th2"), th3(signals);
    th1.join();
    th2.join();
    th3.join();
}
```
尚、condition variable には`native_handle`メンバ関数があり、これによって実装依存の condition variable に対する操作を行う事ができます。例えば、環境が pthread を用いていた場合は例えば以下のように利用する事ができます。
```cpp
std::condition_variable cv;

void f()
{
    {
        std::lock_guard lock(m);
        b = true;
    }
    pthread_cond_signal(cv.native_handle());
}
```
以上が、`<condition_variable>`にて提供される機能です。

## 14.2.4 future
ところで、今までスレッドを利用するにおいて、`std::thread`に対して指定していた INVOKE コンセプトに基づく関数の返却型は全て`void`でした。これには何か規定があるのでしょうか。
```cpp
int f() { return 42; }

std::thread th(f);
th.join();
```
これには特別な規定はありませんので、返却型は`void`以外でも構いません。しかし、このようにして利用しても、値が返却されたところで無視されてしまうのです。引数に対して入力を設定するのは、それがデザインとしてベターであるのならば構いません。しかし、本当は値を返却して結果を受け取りたいというようであれば、`std::promise`クラスと`std::future`クラスを使います。``std::promie`クラスは`std::future`クラスを組み合わせて利用します。`std::promise`は別スレッドでの処理結果を書き込む役割を担っています。反対に`std::future`はその結果を読み取る役割を担っています。`std::promise`と`std::future`は互いに同一の共有状態へアクセスするため、これを利用してスレッド間における値の受け渡しやスレッド間同期を実現します。まずは簡単な例を見て見ましょう。
これらの機能は`<future>`ヘッダをインクルードする事で利用できます。

* std::async

## 14.2.x atomic

* std::atomic 基本操作 
* メモリバリア各種

