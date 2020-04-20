btor2verilog: main.cpp
	${CXX} -static -std=c++11 -I./deps/btor2tools/src -L./deps/btor2tools/build/lib/ -o btor2verilog main.cpp -lbtor2parser

clean:
	rm -f btor2verilog
