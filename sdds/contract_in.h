#ifndef _CONTRACT_IN_H
#define _CONTRACT_IN_H
#include <iostream>
#include <vector>
#include "bridge_utils.h"
#include "constraint.h"
#include DDS_DLL_PATH


void read_contract(istream &fin, contract &ctr);
int parse_trick(int lead, int trump, string &tstr, int *ranks, int *suits);

/* example input:

3NWx
W :3CQC
N AQ742S982HAKDKT3C
S ...
E ...
W ...
*/

void read_deal_full(istream &fin, deal &dl, contract &ctr);
void read_boards(istream &fin, boards &bds, contract *ctrs);

/* example input
3NWx n                            <contract> <n: declarers not vuln, v: declarers vuln>
3 :QHAH3H5H :5SJSAS2S :7DQD       <numTricks (including current, partially empty one)>  <list of >=1 tricks played so far> (in order, with : starting)
me S ...                          me <my-position> <my-hand>
dummy ...                         dummy <dummy-hand>
*/

void read_deal_partial(istream &fin, deal_ptl &dl);
void read_deal_state(istream &fin, deal_state &ds);
void read_state(istream &fin, deal_ptl &dl, constraints &ctrs);
void read_state2(istream &fin, deal_state &ds, constraints &ctrs);
void get_partial(deal_state &ds, int tnum, int tpos, vector<int> known, deal_ptl &dl);

#endif
