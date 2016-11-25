# 第 5 章 スコープと制御文

本章では制御文について説明します。今までの概念に加えて制御文を理解する事で、いよいよプログラムらしいプログラムを記述する事ができるようになります。また、「C++での制御文の記述法」自体も大切な知識ですが、その概念である「制御を行うための考え方」もとても大切です。この章で学ぶ概念は、C++言語以外のプログラミング言語でも大抵通用するような概念となりますので、一度通して読んで見て、理解できなければ、しっかりと理解できるまで読み直す事をお勧めします。しかしかと言って、その概念も理解してしまえばとても単純なものなので、そこまで気を張る必要はありません。<br />この章では以下のような流れで説明していきます。<br />

1. スコープ
2. if文
3. for,range-for文
4. while文
5. break,continue文
6. switch文
7. goto文



## 5.5 break,continue文

### 5.5.1 break文

ここに事前に初期化された変数`x`と無限ループのコードがあります。

```cpp
int x = 10;
while(true){} // 無限ループ
```

例えばこの`x`に対して何らかの処理結果が後々代入され、その値が例えば`0`であったら無限ループを脱出するというコードを書きたいとしましょう。そういった場合、以下のように書きます。

```cpp
int x = 10;
while(true){
    if(x == 0){
        break;
    }
    x = 0; // xに対して何かの処理結果0が代入された!
}
```

まず、`while(true)`によって無限ループを起こそうとしています。その後、変数`x`の値が`0`であるかどうかを`if`文を使って判定しています。この場合、まだ`x`は`0`ではないので`if`文の中身は実行されません。しかしその後、`x`に`0`が入っています。ここで1回目のループが終わります。次に2回目のループに入ると、`if`文の判定で`x == 0`は`true`となりますから、`if`文の中身が実行されます。`if`文の中身は`break`と書かれていますね。この`break`によって強制的にループから抜け出す事ができるのです。もう少し意味のあるコードを書いて見ましょう。

```cpp
#include<iostream>
int main()
{
    int array[]={10,20,30,40};
    int x=0;
    
    while(true){
        if(sizeof(array) / sizeof(array[0]) -1 < x){
            break;
        }
        std::cout << array[x] << std::endl;
        ++x;
    }
}
```

実行結果は以下となります。

```cpp
10
20
30
40
```

`sizeof(array) / sizeof(array[0])`によって配列の要素数を演算しその後`-1`をしています。ここで`-1`が必要なのは、`if`文は継続する場合の条件式ではなく、ループが終わる条件式だからです。ここで`-1`をしなければ、変数`x`が`4`である状態でもループが継続されてしまいますので、`array`という配列の範囲外にアクセスしてしまう事になってしまいます。しかし、何だかこのコード、どこかで似たような事をしていたような気がしませんか？実はその通りで、このコードは`for`文でも書く事ができるのです。

```cpp
#include<iostream>
int main()
{
    int array[]={10,20,30,40};
    for(int x = 0; x < sizeof(array) / sizeof(array[0]); ++x){
        std::cout << array[x] << std::endl;
    }
}
```

実行結果は上記のコードと同じです。このような単純な処理の場合は、一般的には`for`文で書いた方が意味合いとしては分かりやすいコードとなるでしょう。また、`while`で書くにしても以下のように書く事ができます。

```cpp
#include<iostream>
int main()
{
    int array[]={10,20,30,40};
    int x = 0;
    while(x < sizeof(array) / sizeof(array[0])){
        std::cout << array[x] << std::endl;
        ++x;
    }
}
```

`while(継続条件式)`ですので、`for`文と同じ継続条件式をそのまま入れて、`while`の中でカウンターである`x`をインクリメントしてあげれば同じ機能を実現できます。それぞれのコードの関係性が分かるでしょうか？じっくりそれぞれを見比べて見てください。

### 5.5.2 continue文

例えば、ループがこのように入れ子になっているとします。

```cpp
#include<iostream>
int main()
{
    for(unsigned int i = 0; i < 5; ++i){
        std::cout << "iの値は" << i << "です" << std::endl;
        int a=0;
        while(a <= 5){
            std::cout << "aの値は" << a++ << "です ";
        }
        std::cout<<std::endl;
    }
} 
```

実行結果は以下となります。

```cpp
iの値は0です
aの値は0です aの値は1です aの値は2です aの値は3です aの値は4です aの値は5です
iの値は1です
aの値は0です aの値は1です aの値は2です aの値は3です aの値は4です aの値は5です
iの値は2です
aの値は0です aの値は1です aの値は2です aの値は3です aの値は4です aの値は5です
iの値は3です
aの値は0です aの値は1です aの値は2です aの値は3です aの値は4です aの値は5です
iの値は4です
aの値は0です aの値は1です aの値は2です aの値は3です aの値は4です aの値は5です
```

このコードで、`i`が2以下の場合のみ`while`文を実行したいとします。そういった場合、`continue`文が役立ちます。

```cpp
#include<iostream>
int main()
{
    for(unsigned int i = 0; i < 5; ++i){
        std::cout << "iの値は" << i << "です" << std::endl;
        int a=0;
        
        if(i > 2){
            continue;
        }
        while(a <= 5){
            std::cout << "aの値は" << a++ << "です";
        }
        std::cout<<std::endl;
    }
}
```

実行結果は以下となります。

```cpp
iの値は0です
aの値は0ですaの値は1ですaの値は2ですaの値は3ですaの値は4ですaの値は5です
iの値は1です
aの値は0ですaの値は1ですaの値は2ですaの値は3ですaの値は4ですaの値は5です
iの値は2です
aの値は0ですaの値は1ですaの値は2ですaの値は3ですaの値は4ですaの値は5です
iの値は3です
iの値は4です
```

`while`文内に`if`文によって`i`が2よりも大きい時の条件を作りました。その中には`continue`と書かれていますね。つまり、`continue`とはループ内でそこから下にある記述をスキップしてループを継続するという意味になります。なので、3以上となった時、`a`の値に関する出力がされないという結果になるのです。

## 5.6 switch文
if文の拡張的な文法としてswitch文という文法があります。

```cpp
#include<iostream>
int main()
{
    int a[4]={};
    for(unsigned int i=0; i<sizeof(a)/sizeof(a[0]); ++i)
        std::cin>>a[i];
    
    for(int value:a){
        switch(value){
            case 1:
                std::cout<<"1です"<<std::endl;
                break;
            case 2:
                std::cout<<"2です"<<std::endl;
                break;
            case 3:
                std::cout<<"3です"<<std::endl;
                break;
            default:
                std::cout<<"それ以外です"<<std::endl;
                break;
        }
    }
}
```
実行結果は以下となります。
```cpp
1 2 3 50
1です
2です
3です
それ以外です
```
1 2 3 50という入力に対してそれぞれの結果が上から出力されています。文法を以下に示します。
```cpp
switch(制御式){
    case 事例式: // ;ではなく:
        文;
        break;
    case 事例式: // ;ではなく:
        文;
        break;
    case 事例式: // ;ではなく:
        文;
        break;
    // ...
    default:
        文;
        break;
}
```
このように事例式には事例の式を記述するので、直接数値を記述しても良いですし、1つの演算式などを記述する事もできますが、整数値か文字(文字列ではない)のみとなります。先ほどのコードでは、それぞれ1、2、3、`default`としましたので、意味合いとしては1の場合、2の場合、3の場合、それ以外というように分岐します。つまり、先ほどのコードは下記コードと同じ意味合いとなるのです。
```cpp
#include<iostream>
int main(){
    int a[4]={};
    for(unsigned int i=0; i<sizeof(a)/sizeof(a[0]); ++i)
        std::cin>>a[i];

    for(int value:a){
        if(value==1)
            std::cout<<"1です"<<std::endl;
        else if(value==2)
            std::cout<<"2です"<<std::endl;
        else if(value==3)
            std::cout<<"3です"<<std::endl;
        else
            std::cout<<"それ以外です"<<std::endl; 
    }
}
```
両者の記述方がある理由は、可読性と効率性を向上させるためにあります。<br>ところでbreak;とありますが、この記述を省くとどのように動作するでしょうか。やってみましょう。
```cpp
#include<iostream>
int main()
{
    int a[4]={};
    for(std::size_t i=0; i<sizeof(a)/sizeof(a[0]); ++i)
        std::cin>>a[i];

    for(int value:a){
        switch(value){
            case 1:
                std::cout<<"1です"<<std::endl;
            case 2:
                std::cout<<"2です"<<std::endl;
                break;
            case 3:
                std::cout<<"3です"<<std::endl;
                break;
            default:
                std::cout<<"それ以外です"<<std::endl;
                break;
        }
    }
}
```
実行結果は以下となります。
```cpp
1 2 3 50
1です
2です
2です
3です
それ以外です
```
1 2 3 50という入力に対して何故だか「2です」という出力が二度行われています。これはどのタイミングで実行されたかというと、`value`の値が1だった時に起きるのです。何故なら、`case 1:`の部分で`break`を記述していないからです。`break`を記述しなかった場合、その次の`case 2:`の文部分まで実行されます。そのため、`value`の値が1である時に、「1です」という出力と「2です」という出力がされるので結果的に「2です」という出力が二度される事となるのです。因みに、1 2 3 というように事例式を昇順で記述しましたが、これについての規則は特にありません。
```cpp
#include<iostream>
int main(){
    int a[4]={};
    for(unsigned int i=0; i<sizeof(a)/sizeof(a[0]); ++i)
        std::cin>>a[i];
    
    for(int value:a){
        switch(value){
            case 3:
                std::cout<<"3です"<<std::endl;
            case 2:
                std::cout<<"2か3です"<<std::endl;
                break;
            case 1:
                std::cout<<"1です"<<std::endl;
                break;
            default:
                std::cout<<"それ以外です"<<std::endl;
                break;
        }
    }
}
```
また、最後のcase文 or default文ではbreak文があろうがなかろうが動作としては変わらないので省略しても構いません。

## 5.7 goto文

goto文は以下のように使います。
```cpp
#include<iostream>#include<iomanip>
int main()
{
    int i=10;
    
    GO:
    if(i>0){
        std::cout<<std::setw(3)<<(11-i--);
        goto GO;
    }
}
```
実行結果は以下となります。
```cpp
 1 2 3 4 5 6 7 8 9 10
```
文法は以下となります。
```cpp
goto ラベル名;
```
文法の通り記述するとラベル名の部分へ有無を言わさず飛んでいきます。ラベルはgoto文と同じ関数内にただ1つなければなりません。ラベル名は
```cpp
LABEL_NAME:
```
というように記述します。<br>
goto文は一般的に無意味に多様するべきではありません。有無を言わさずジャンプができるためプログラムの内容として無視されてはならない部分を無視してジャンプしてしまったり、単純に可読性が落ちるなどの理由があります。使うと便利なシーンとしては例えば何重にもなったループを一気に抜ける時や、例外処理を扱う時などで重宝される場合があります。例外処理については後に学ぶ概念なのでそれまでは気にしなくて大丈夫です。