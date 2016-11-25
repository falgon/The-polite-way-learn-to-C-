# 第 5 章 スコープと制御文

本章では制御文について説明します。今までの概念に加えて制御文を理解する事で、いよいよプログラムらしいプログラムを記述する事ができるようになります。また、「C++での制御文の記述法」自体も大切な知識ですが、その概念である「制御を行うための考え方」もとても大切です。この章で学ぶ概念は、C++言語以外のプログラミング言語でも大抵通用するような概念となりますので、一度通して読んで見て、理解できなければ、しっかりと理解できるまで読み直す事をお勧めします。しかしかと言って、その概念も理解してしまえばとても単純なものなので、そこまで気を張る必要はありません。<br />この章では以下のような流れで説明していきます。<br />

1. スコープ
2. if文
3. for,range-for文
4. while文
5. break,continue文
6. switch文
7. goto文




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