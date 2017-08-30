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
実行結果は以下となります。
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
実行結果は以下の通りです。
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
実行結果は以下の通りです。
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
実行結果は以下の通りです。
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
実行結果は以下の通りです。
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
実行結果は以下の通りです。
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
実行結果は以下となります。
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
筆者の環境では、`false`を返したため、例外が投げられた後に`std::terminate`が呼ばれました。次に、`std::recursive_mutex`について説明します。

// .........

次に、`std::timed_mutex`について説明します。

// ........

次に、`std::recursive_timed_mutex`について説明します

// ......


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
実行結果は以下の通りです。
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
実行結果は変わりません。<br>
さらにより高度なロック制御を行いたい場合、`std::unique_lock`を使用します。`std::unique_lock`は、コンストラクト時点でロックを取得する事もできますが、そうではなく後でロックを取得するように設定する事ができます。また、ロック取得時に、`lock`ではなく、`try_lock`を使用させるよう指示する事ができます。また、ミューテックスの所有権の移動、放棄と任意のタイミングでの所有ミューテックスのロック操作が行えます。まずは単純にコンストラクト時にロックし、デストラクト時にロックを解除する例です。単に上記の`std::lock_guard<decltype(m_)>`を`std::unique_lock<decltype(m_)>`とするだけです。
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

// ........

また、`std::scoped_lock`というものもあります。これは、可変個のミューテックス型及びミューテックスオブジェクトを取る`std::lock_guard`のようなものです。

// .......

尚、ここまではミューテックス型のメンバ関数`lock`、`try_lock`、`unlock`を直接呼んでいましたが、`std::lock`、`std::try_lock`、`std::unlock`関数も用意されています。これらは引数が可変となっており、それぞれ渡したミューテックスをロック及びアンロックします。

// .....

さらに、ユーティリティとして`std::call_once`という関数も用意されています。これは、指定された関数を一度だけ呼び出すもので、複数のスレッド間で共通使用するデータの初期化処理などで利用する事ができます。

// ........

## 14.2.2 std::future

## 14.2.3 std::promice

## 14.2.4 std::async

## 14.2.x atomic

* std::atomic 基本操作 
* メモリバリア各種

ところで、排他制御におけるロックとアンロックの操作はリオーダーされてしまう事はないのでしょうか。それは C++ の標準規格によって起こりえないと明記されていますので起こりえません。
