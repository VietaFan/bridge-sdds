#include "str_utils.h"
using namespace std;

void to_lower(string &s) {
  for (int i=0; i<s.size(); i++) {
    if (s[i] >= 'A' && s[i] <= 'Z') {
      s[i] += 32;
    }
  }
}

string strip(string s) {
  int k = s.size()-1;
  while (k > -1 && s[k] == ',' || s[k] == ';' || s[k] == ':' || s[k] == ' ' || s[k] == '\t' || s[k] == '\n') k--;
  return s.substr(0, k+1);
}
