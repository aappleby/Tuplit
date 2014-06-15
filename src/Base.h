#pragma once

/*
namespace std {
  template<class _Elem> struct char_traits;
  template<class _Ty> class allocator;
  template<class _Elem,
	  class _Traits = char_traits<_Elem>,
	  class _Ax = allocator<_Elem> >
	  class basic_string;

  typedef basic_string<char, char_traits<char>, allocator<char> > string;

  template<class _Ty,
	  class _Ax = allocator<_Ty> >
	  class vector;
};
*/

#include <string>
#include <vector>
#include <map>
#include <assert.h>
#include "stdint.h"

using std::string;
using std::map;
using std::vector;

vector<string> split(const string &s, char delim);