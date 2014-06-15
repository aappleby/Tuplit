#include "TupleExperiment.h"
#include "Lexer.h"
#include "Parser.h"
#include "Token.h"
#include "HashTable.h"
#include "Opcodes.h"
#include "Machine.h"

#include <algorithm>
#include <string>
#include <map>
#include <hash_map>
#include <vector>
#include <fstream>
#include <intrin.h>


extern Instruction gcd[];

int main(int /*argc*/, char* /*argv[]*/)
{
  /*
  Machine m;

  std::string a = "thirty five";

  m.in[0] = Atom(35);
  m.in[1] = Atom(125);

  m.run(gcd);
  */

  std::ifstream ifs("examples\\gcd.h");
  std::string code((std::istreambuf_iterator<char>(ifs)),
                    std::istreambuf_iterator<char>());

  Parser p;
  p.lex.lex(code);
  p.lex.stripComments();

  p.lex.dump();
  printf("----------\n");

  p.parse();

  p.dump();
  printf("----------\n");

  return 0;
}
