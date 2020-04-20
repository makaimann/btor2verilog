#include "btor2verilog.h"

using namespace std;

namespace btor2verilog
{

void Btor2Verilog::initialize()
{
  err_ = "";
  verilog_ = "";
  inputs_.clear();
  states_.clear();
  wires_.clear();
  state_updates_.clear();
  wire_assigns_.clear();
}

bool Btor2Verilog::parse(const char * filename)
{
  Btor2Parser * reader_ = btor2parser_new();
  FILE * btor2_file = fopen(filename, "r");

  if (!btor2parser_read_lines(reader_, btor2_file))
  {
    err_ = btor2parser_error(reader_);
    fclose(btor2_file);
    btor2parser_delete(reader_);
    return false;
  }

  // TODO: fill this in!

  fclose(btor2_file);
  btor2parser_delete(reader_);
  return true;
}

bool Btor2Verilog::gen_verilog()
{
  verilog_ = "";
  cout << "gen_verilog unimplemented" << endl;
  throw std::exception();
}

}
