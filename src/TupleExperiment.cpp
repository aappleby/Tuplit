#include "TupleExperiment.h"
#include "Lexer.h"
#include "Parser.h"
#include "Token.h"
#include "HashTable.h"

#include <algorithm>
#include <string>
#include <map>
#include <hash_map>
#include <vector>
#include <fstream>
#include <intrin.h>

using std::string;
using std::map;
using std::vector;



int main(int /*argc*/, char* /*argv[]*/)
{
  std::ifstream ifs("ParseTest1.h");
  std::string code((std::istreambuf_iterator<char>(ifs)),
                    std::istreambuf_iterator<char>());

  Lexer l;
  l.lex(code);

  Parser p;
  p.parse(&l);

  l.dump();

  return 0;
}
