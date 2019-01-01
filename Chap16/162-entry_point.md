# 16.2 エントリポイント

例えば、WindowsのGUIプログラムを記述しようとした時に、以下が最小のプログラムであるというのを見た事はないでしょうか。
```cpp
#include <windows.h>
int WINAPI WinMain(
HINSTANCE hThisInst,
HINSTANCE hPrevInst,
LPSTR lpszArgs,
int nWinMode
)
{
// ....
}

// main関数がない
```
`WinMain`という関数が`main`関数の代わりをしているかのように振舞います。

また、SDLというグラフィックハードウェアへのアクセスを提供するライブラリでは、`main`関数を記述できるかのように思えます。
```cpp
#include<sdl/sdl.h>
int main(int* argc,char* argv[])
{
// ....
}
```
sdl.hというヘッダーファイルを辿っていくと、中には以下のような記述があります。
```cpp
//SDL_main.h
#define main SDL_main
/** The prototype for the application’s main() function */
extern C_LINKAGE int SDL_main(int argc, char *argv[]);
```
これはつまり、`main`と記述した場合、`SDL_main`へと置き換わるという事です。よって、SDLライブラリをインクルードした状態で`main`関数を記述すると
```cpp
#include<sdl/sdl.h>
int main(int* argc,char* argv[])
{
// ....
}
```
以下のように置き換わるのです。
```cpp
#include<sdl/sdl.h>
int SDL_main(int argc, char *argv[])
{
// ....
}
```
となると、コードからは`main`関数が全くどこにもない事になってしまいました。「2.2 最小のプログラム」で説明したようにプログラムは`main`関数から開始されるはずです。これは、何故コンパイルに成功し、`main`関数の代替のように振る舞えるのでしょうか。
結論から言えば、**エントリポイント**をコンパイル時に設定する事で任意の関数名からプログラムを開始する事ができるからなのです。エントリポイントとは、開始するプログラムの位置を言います。

例えば前述した`WinMain`について取り上げると、この`WinMain`のコードをコンパイルするにはリンカオプションで/SUBSYSTEMを `WINDOWS`に指定しなければなりません。すると、リンカは `WinMain`という名前の関数を探して、それをプログラムのエントリポイントに設定します。エントリポイントに設定する事で、OSがプログラムを実行するために実行ファイルをメモリに読み込んだ後、どのアドレスからプログラムを実行するか指定する事ができるのです。

指定されたエントリポイントは、リンカーによって実行形式ファイルのヘッダ部分書き込まれます。

よって、例えば`main`関数でない別の関数をエントリポイントとしたいのであれば、以下のように指定する事で`main`関数のないC言語プログラムを作る事ができます。
```cpp
#include<stdio.h>
int hoge()
{
	puts("hoge only");
}
```
```cpp
gcc -e hoge() a.c
```
実行結果は以下となります。
```cpp
hoge only
```
C++ではなくC言語であるのはちょっとした理由があります。それには
、**マングリング**というものが起因しています。マングリングについては次項で説明しています。