#pragma once

extern "C" {
#include "btor2parser/btor2parser.h"
}

#include "stdio.h"
#include <iostream>
#include <unordered_map>

enum Kind { bitvec=0, array=1 };

struct Sort
{
  Sort(size_t w) : k(bitvec), w1(w), w2(0) {}
  Sort(size_t w, size_t v) : k(array), w1(w), w2(v) {}
  Kind k;
  size_t w1;
  size_t w2;
};


namespace btor2verilog
{
  class Btor2Verilog
  {
  public:
    Btor2Verilog() { initialize(); }
    ~Btor2Verilog() {}

    bool parse(const char * filename);
    bool gen_verilog();
    std::string verilog() { return verilog_; }
    std::string error() { return err_; }

  protected:
    void initialize();

    std::string err_;
    std::string verilog_;

    std::unordered_map<size_t, std::string> inputs_;
    std::unordered_map<size_t, std::string> states_;
    std::unordered_map<size_t, std::string> wires_;

    std::unordered_map<size_t, std::string> state_updates_;
    std::unordered_map<size_t, std::string> wire_assigns_;

  };
}
