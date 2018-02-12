function extract (){
    echo "/**\n* @defgroup $2\n* @brief $3\n*/\n/*@{*/" > $1
    sed -n '/^```/,/^```/ p' < ../Chap16/167-computational-complexity-theory.md | sed '/^```/ d' >> $1
    echo "/*@}*/" >> $1
}

D1671="TPLCXX17_1671.hpp"
D1671_Description="167\t16.7\t第16章/16.7\tアルゴリズムと計算複雑性理論"
D1671_Brief="16.7.1\tで利用されたサンプルコードです"

extract $D1671 $D1671_Description $D1671_Brief
