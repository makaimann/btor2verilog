#pragma once

extern "C" {
#include "btor2parser/btor2parser.h"
}

#include "stdio.h"
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

enum Kind { bitvec=0, array=1, none=2};

struct Sort
{
  Sort() : k(none), w1(0), w2(0) {}
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

    Btor2LineIterator it_;
    Btor2Line * l_;
    Sort linesort_;
    size_t i_;
    size_t idx_;
    bool negated_;
    std::string sym_;

    std::unordered_map<size_t, Sort> sorts_;
    std::vector<std::string> args_;

    std::unordered_map<size_t, std::string> symbols_;
    std::unordered_set<std::string> inputs_;
    std::unordered_set<std::string> outputs_;
    std::vector<std::string> states_;
    std::vector<std::string> wires_;

    std::unordered_map<size_t, std::string> state_updates_;
    std::unordered_map<size_t, std::string> wire_assigns_;

  };
}
