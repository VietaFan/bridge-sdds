#ifndef _SCORING_H
#define _SCORING_H
#include "bridge_utils.h"
#include <vector>
using namespace std;

// returns the number of MPs for the declaring side
int get_mps(contract &ctr, int ntricks);
int get_imps(int mp_delta);
// returns a 14x14 grid of the resulting imps of taking i tricks against taking j tricks
void get_imps_mat(contract &ctr, vector<vector<int> > &impmat);
#endif
