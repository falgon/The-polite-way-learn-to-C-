# コラム
このチャプターではちょっとした小ネタを紹介します。

## strict aliasing rule
C/C++言語には、strict aliasing ruleと言われるルールが存在します。本文書では「7.2 配列」の項で少し触れました。

```cpp
#include<climits>
#include<iostream>
int main()
{
 int array[]={INT_MAX,42};
 char* ptr=reinterpret_cast<char*>(&array[0]); // int型のデータがある領域のアドレスを強引にchar型のポインターへ代入
 std::cout<< *(ptr+1) << std::endl; // うまく動かない
}
```


