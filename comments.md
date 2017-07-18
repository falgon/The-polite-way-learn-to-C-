# 項ごとの方針と注釈

## [1,2 C++17 とは](/Chap1/11-c17.md)
* 執筆時現在、正式にはまだ C++17 は策定されていないが近い将来確実に策定される可能性が高いとして、その旨を文面に記している。

## [10.1 例外](Chap10/exception.md)
* [exception specification throwはC++17で禁止](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0003r0.html)となったため本稿では触れていない。

## [10.2 attribute](Chap10/attribute.md)
* [`memory_order_consume`の仕様検討](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0371r1.html)につき、関連機能である`[[carries_dependency]]`について本稿では触れていない。
