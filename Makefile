btor2verilog: main.cpp
	${CXX} -static -std=c++11 -I. -I./deps/btor2tools/src -L./deps/btor2tools/build/lib/ -o btor2verilog main.cpp btor2verilog.cpp -lbtor2parser

clean:
	rm -f btor2verilog
