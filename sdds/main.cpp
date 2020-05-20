//   uses the double dummy solver by Bo Haglund & Soren Hein
// assumes the dll is stored in the right place

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>

#include "bridge_utils.h"
#include DDS_DLL_PATH
#include "str_utils.h"
#include "constraint.h"
#include "contract_in.h"
#include "scoring.h"
#include "generation.h"
#include "results_proc.h"

using namespace std;


int main()
{
  init_choose();
  deal_ptl dl;
  constraints ctrs;
  boards bds;
  solvedBoards solved;

  deal_state ds;
  //read_state2(cin, ds, ctrs);
  vector<int> v;
  v.push_back(0);
  v.push_back(2);
  //get_partial(ds, 0, 1, v, dl);
  
  
  read_state(cin, dl, ctrs);
  //print_partial_deal(dl);
  
  int nboards;
  cin >> nboards;
  //cout << nboards << endl;
  generate_boards(dl, ctrs, nboards, bds);
  //cout << "calling solver..\n";
  int res = SolveAllChunksBin(&bds, &solved, 1);
  char line[80];
  if (res != 1) {
    ErrorMessage(res, line);
    cout << "DDS error: " << line << "\n";
  }
 
  //card_results results[13];
  full_results results;
  int n_choices = get_results(dl, solved, results);
  raw_results_csv(cout, results);
  imp_results_csv(cout, results);
  impsd_results_csv(cout, results);
  vector<int> a,b,c,d,e;
  //  precompute3(dl, ctrs, a,b,c,d,e);
}
