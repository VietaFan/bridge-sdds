#ifndef _RESULTS_PROC_H
#define _RESULTS_PROC_H

#include "bridge_utils.h"
#include "scoring.h"
#include DDS_DLL_PATH
#include <iostream>
using namespace std;

struct card_results {
  int suit, rank;
  int counts[14]; // #times this many tricks are taken for the playing side
  int decl_counts[14];
};

struct full_results {
  int n_cards;
  card_results cr[13];
  double avg_imps[13][13];
  double sd_imps[13][13];
};

void calc_imp_table(deal_ptl &dl, full_results &res);
int get_results(deal_ptl &dl, solvedBoards &solved, full_results &results);
void raw_results_csv(ostream &out, full_results &res);
void imp_results_csv(ostream &out, full_results &res);
void impsd_results_csv(ostream &out, full_results &res);

#endif
