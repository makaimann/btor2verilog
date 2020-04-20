#include "btor2verilog.h"

using namespace std;

namespace btor2verilog
{

const unordered_map<Btor2Tag, string> bvopmap({
    { BTOR2_TAG_add, "+" },
    { BTOR2_TAG_and, "&" },
    // { BTOR2_TAG_bad, },
    //{ BTOR2_TAG_concat, Concat },
    //{ BTOR2_TAG_const, },
    //{ BTOR2_TAG_constraint, },
    //{ BTOR2_TAG_constd, },
    //{ BTOR2_TAG_consth, },
    //{ BTOR2_TAG_dec, },
    // { BTOR2_TAG_eq, BVComp }, // handled this specially, because could also
    // have array arguments
    //{ BTOR2_TAG_fair, },
    { BTOR2_TAG_iff, "==" },
    // { BTOR2_TAG_implies, Implies }, // handle specially (needs to work with
    // boolector AND other solvers), gets complicated with bools and BV(1) are
    // aliased
    //{ BTOR2_TAG_inc, },
    //{ BTOR2_TAG_init, },
    //{ BTOR2_TAG_input, },
    //{ BTOR2_TAG_ite, Ite },
    //{ BTOR2_TAG_justice, },
    { BTOR2_TAG_mul, "*" },
    //{ BTOR2_TAG_nand, BVNand },
    { BTOR2_TAG_neq, "!=" },
    //{ BTOR2_TAG_neg, BVNeg },
    //{ BTOR2_TAG_next, },
    //{ BTOR2_TAG_nor, BVNor },
    { BTOR2_TAG_not, "~" },
    //{ BTOR2_TAG_one, },
    //{ BTOR2_TAG_ones, },
    { BTOR2_TAG_or, "|" },
    //{ BTOR2_TAG_output, },
    // { BTOR2_TAG_read, Select }, // handle specially -- make sure it's casted
    // to bv
    { BTOR2_TAG_redand, "&"},
    { BTOR2_TAG_redor, "|"},
    { BTOR2_TAG_redxor, "^"},
    // { BTOR2_TAG_rol, },
    // { BTOR2_TAG_ror, },
    //{ BTOR2_TAG_saddo, },
    //{ BTOR2_TAG_sdiv, BVSdiv },
    //{ BTOR2_TAG_sdivo, },
    //{ BTOR2_TAG_sext, },
    //{ BTOR2_TAG_sgt, BVSgt },
    //{ BTOR2_TAG_sgte, BVSge },
    //{ BTOR2_TAG_slice, },
    { BTOR2_TAG_sll, "<<" },
    // { BTOR2_TAG_slt, BVSlt },
    // { BTOR2_TAG_slte, BVSle },
    //{ BTOR2_TAG_sort, },
    // { BTOR2_TAG_smod, BVSmod },
    //{ BTOR2_TAG_smulo, },
    { BTOR2_TAG_sra, ">>>" },
    // { BTOR2_TAG_srem, BVSrem },
    { BTOR2_TAG_srl, ">>" },
    //{ BTOR2_TAG_ssubo, },
    //{ BTOR2_TAG_state, },
    { BTOR2_TAG_sub, "-" },
    //{ BTOR2_TAG_uaddo, },
    { BTOR2_TAG_udiv, "/" },
    //{ BTOR2_TAG_uext, },
    { BTOR2_TAG_ugt, ">" },
    { BTOR2_TAG_ugte, ">=" },
    { BTOR2_TAG_ult, "<" },
    { BTOR2_TAG_ulte, "<=" },
    //{ BTOR2_TAG_umulo, },
    { BTOR2_TAG_urem,  "%"},
    //{ BTOR2_TAG_usubo, },
    //{ BTOR2_TAG_write, Store }, // handle specially -- make sure it's casted
    //to bv
    //{ BTOR2_TAG_xnor, BVXnor },
    { BTOR2_TAG_xor, "^" },
    //{ BTOR2_TAG_zero, }
});

void Btor2Verilog::initialize()
{
  err_ = "";
  verilog_ = "";
  sorts_.clear();
  symbols_.clear();
  inputs_.clear();
  outputs_.clear();
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

  fclose(btor2_file);

  it_ = btor2parser_iter_init(reader_);
  while ((l_ = btor2parser_iter_next(&it_)))
  {
    // identify sort
    if (l_->tag != BTOR2_TAG_sort && l_->sort.id)
    {
      linesort_ = sorts_.at(l_->sort.id);
    }

    // Gather arguments
    args_.clear();
    args_.reserve(l_->nargs);
    for (i_ = 0; i_ < l_->nargs; i_++)
    {
      negated_ = false;
      idx_ = l_->args[i_];
      if (idx_ < 0) {
        negated_ = true;
        idx_ = -idx_;
        args_.push_back("~" + symbols_.at(idx_));
      }
      else
      {
        args_.push_back(symbols_.at(idx_));
      }
    }

    // handle the special cases

    if (l_->tag == BTOR2_TAG_state)
    {
      sym_ = "s" + std::to_string(states_.size());
      states_.push_back(sym_);
      symbols_[l_->id] = sym_;
    }
    else if (l_->tag == BTOR2_TAG_input)
    {
      sym_ = "i" + std::to_string(inputs_.size());
      inputs_.insert(sym_);
      symbols_[l_->id] = sym_;
    }
    else if (l_->tag == BTOR2_TAG_output)
    {
      sym_ = "o" + std::to_string(outputs_.size());
      outputs_.insert(sym_);
      symbols_[l_->id] = sym_;
    }
    else if (l_->tag == BTOR2_TAG_sort)
    {
      switch(l_->sort.tag)
      {
      case BTOR2_TAG_SORT_bitvec: {
        linesort_ = Sort(l_->sort.bitvec.width);
        sorts_[l_->id] = linesort_;
        break;
      }
      case BTOR2_TAG_SORT_array: {
        Sort s1 = l_->sort.array.index;
        Sort s2 = l_->sort.array.element;
        if (s1.k != bitvec || s2.k != bitvec)
        {

          btor2parser_delete(reader_);
          err_ = "Multi-dimensional arrays not yet supported";
          return false;
        }
        else
        {
          linesort_ = Sort(s1.w1, s2.w1);
          sorts_[l_->id] = linesort_;
        }
        break;
      }
      }
    }

  }

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
