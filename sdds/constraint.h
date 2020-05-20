#ifndef _CONSTRAINT_H
#define _CONSTRAINT_H
#include "bridge_utils.h"
using namespace std;

// first index of everything is Hand/Direction
struct constraints {
  int hcp_lb[4];
  int hcp_ub[4];
  int suit_lb[4][4];
  int suit_ub[4][4];
  int pts_lb[4];
  int pts_ub[4];
};

void reduce_constraints(constraints &cstr);
void augment_init_constr(deal_ptl &dl, constraints &ctrs);
int read_constraints(istream &fin, constraints &cstr);

#endif
