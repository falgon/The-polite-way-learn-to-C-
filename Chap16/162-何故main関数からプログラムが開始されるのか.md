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
`WinMain`という関数が`main`関数の代わりをしているかのように振舞います。また、SDLというグラフィックハードウェアへのアクセスを提供するライブラリでは、`main`関数を記述できるかのように思えます。
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
となると、コードからは`main`関数が全くどこにもない事になってしまいました。これは何故