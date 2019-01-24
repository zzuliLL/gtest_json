all : gtest_json file run info html

gtest_json :
	g++ -o gtest_json gtest.cpp json.cpp -lgtest -lpthread -g -Wall -Wshadow -fprofile-arcs -ftest-coverage

file :
	mkdir -p file
run :
	./gtest_json
info :
	gcov gtest.cpp && gcov json.cpp
html :
	lcov -c -d . -o json_gcov.info && genhtml -o json json_gcov.info
	
.PHONY :clean

clean :
	-rm -rf json file *.info *.gcno *.gcda gtest_json
