echo "/**\n* @defgroup 167 16.7 第16章/16.7 アルゴリズムと計算量理論\n* @brief 16.7.1 で利用されたサンプルコードです\n*/\n/*@{*/" > TPLCXX17_1671.hpp
sed -n '/^```/,/^```/ p' < ../167-computational-complexity-theory.md | sed '/^```/ d' >> TPLCXX17_1671.hpp
echo "/*@}*/" >> TPLCXX17_1671.hpp
