# 方針、注釈、コメント

* C++14 仕様については N3936 を参照している。

## [1,2 C++17 とは](/Chap1/11-c17.md)
* 執筆時現在、正式にはまだ C++17 は策定されていないが近い将来確実に策定される可能性が高いとして、その旨を文面に記している。

## [10.1 例外](Chap10/exception.md)
* [exception specification throwはC++17で禁止](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/p0003r0.html)となったため本稿では触れていない。

## [10.2 attribute](Chap10/attribute.md)
* [`memory_order_consume`の仕様検討](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0371r1.html)につき、関連機能である`[[carries_dependency]]`について本稿では触れていない。

## [16.5.1 評価の順序/言語バージョンごとの解釈](Chap16/165-order_of_eval.md)

* "組み込みの代入演算子と組み込みのすべての複合代入演算子のse（左オペランドの変更）は、左と右のオペランドのvc の後で順序付けされ、代入式のvc の前に順序付けされます"
→ **N3936 [expr.ass]/1**

* "まず、`++i`は、`i += 1`と等価です"
→ **N3936 [expr.pre.incr]/1**

* "組み込みの前置インクリメント演算子と前置デクリメント演算子の se は、その vc の前に順序付けされます"
→ **N3936 [expr.pre.incr]** により複合代入演算子との定義による暗黙のルールによって **N3936 [intro.execution]/15** が適用される。

* "演算子のオペランドの vc は、演算子の結果の vc の前に順序付けされます"
→ **N3936 [intro.execution]/15**

* "後置インクリメント式のvcは、se の前に順序付けされます"
→ **N3936 [expr.post.incr]/1**

* "すべての簡単な代入式`E1 = E2`とすべての複合代入式`E1 @ = E2`(`@`は任意の複合代入演算子の意)では、E1のすべての vc と seの前に、E2のすべての vc と se が順序付けられます。"
→ **N4660 [expr.ass]/1**

### 参照
* N3936
* P0145R3
* http://d.hatena.ne.jp/yohhoy/20170104/p1
* http://d.hatena.ne.jp/Cryolite/20101228
