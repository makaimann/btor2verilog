#include "btor2verilog.h"

using namespace btor2verilog;
using namespace std;

int main(int argc, char ** argv)
{
  if (argc != 2)
  {
    std::cout << "usage: " << argv[0] << " <btor2file>" << std::endl;
    return 1;
  }

  Btor2Verilog b2v;
  if (!b2v.parse(argv[1]))
  {
    cout << b2v.error() << endl;
    return 2;
  }

  if (!b2v.gen_verilog())
  {
    cout << b2v.error() << endl;
    return 3;
  }

  cout << b2v.get_verilog() << endl;

  return 0;
}
