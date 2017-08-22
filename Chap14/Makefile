GXX = /usr/local/Cellar/gcc/7.2.0/bin/g++-7
CLANG = /usr/local/Cellar/llvm/4.0.1/bin/clang++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++1z
INCLUDEBOOST = -I /usr/local/include/
INCLUDESROOK = -I $(HOME)/Documents/src/cpp/IncludeLibraries

TARGET = t.cpp

target: $(TARGET)
	$(GXX) $(CXXFLAGS) $(TARGET)
	$(CLANG) $(CXXFLAGS) $(TARGET)
run: $(TARGET)
	$(GXX) $(CXXFLAGS) $(TARGET)
	$(CLANG) $(CXXFLAGS) $(TARGET)
	./a.out
clean: a.out
	$(RM) a.out
