#ifndef _GENERATION_H
#define _GENERATION_H
#include "constraint.h"
#include "bridge_utils.h"
using namespace std;

// must call this before anything else will work
void init_choose();
// returns an n-bit integer selected uniformly at random from the set of
// n-bit integers with exactly k ones
int rand_choice(int n, int k);
void generate_deal(deal_ptl &pd, constraints &ctrs, deal &dl, unordered_set<int> &exclude);
void generate_boards(deal_ptl &pd, constraints &ctrs, int nboards, boards &bds);
void precompute3(deal_ptl &pd, constraints &ctrs, vector<int> &hcp_ranks, vector<int> &hcp_suits,
            vector<int> &opens, vector<int> &indices, vector<int> &counts);
#endif
