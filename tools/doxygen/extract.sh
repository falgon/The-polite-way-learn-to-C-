#!/bin/bash

cwd=`dirname "${0}"`

extract (){
    echo -e "/**\n* @addtogroup $2\n* @brief $3\n*/\n/*@{*/" > "$cwd/outputsrc/$1"
    sed -n '/^```cpp/,/^```/ p' < $4 | sed '/^```/ d' >> "$cwd/outputsrc/$1"
    echo -e "/*@}*/" >> "$cwd/outputsrc/$1"
}

### main

mkdir -p $cwd/outputsrc

D1671="TPLCXX17_1671.hpp"
D1671_Description="167\t16.7\t第16章/16.7\tアルゴリズムと計算複雑性理論"
D1671_Brief="16.7.1\tで利用されたサンプルコードです"
D1671_PATH="$cwd/../../Chap16/167-computational-complexity-theory.md"

extract $D1671 $D1671_Description $D1671_Brief $D1671_PATH

D1681="TPLCXX17_1681.hpp"
D1681_Description="168\t16.8\t第16章/16.8\tfloatのしくみ"
D1681_Brief="16.8.1\tで利用されたサンプルコードです"
D1681_PATH="$cwd/../../Chap16/168-float.md"

extract $D1681 $D1681_Description $D1681_Brief $D1681_PATH
