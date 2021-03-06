#include "constraint.h"
#include "bridge_utils.h"
#include "str_utils.h"
#include <cstdlib>
#include <iostream>
using namespace std;

void reduce_constraints(constraints &cstr) {
  for (int h=0; h<4; h++) {
    if (cstr.pts_lb[h] < cstr.hcp_lb[h]) {
      cstr.pts_lb[h] = cstr.hcp_lb[h];
    }
    if (cstr.hcp_ub[h] > cstr.pts_ub[h]) {
      cstr.hcp_ub[h] = cstr.pts_ub[h];
    }
  }
  int lbsum,ubsum;
  int *ptrs_lb[2], *ptrs_ub[2];
  ptrs_lb[0] = cstr.hcp_lb;
  ptrs_ub[0] = cstr.hcp_ub;
  for (int i=0; i<13; i++) {
    for (int j=0; j<1; j++) {
      lbsum = 0;
      ubsum = 0;
      for (int h=0; h<4; h++) {
        lbsum += ptrs_lb[j][h];
        ubsum += ptrs_ub[j][h];
      }
      for (int h=0; h<4; h++) {
        if (40-(lbsum-ptrs_lb[j][h]) < ptrs_ub[j][h]) {
          ptrs_ub[j][h] = 40-(lbsum-ptrs_lb[j][h]);
        }
        if (40-(ubsum-ptrs_ub[j][h]) > ptrs_lb[j][h]) {
          ptrs_lb[j][h] = 40-(ubsum-ptrs_ub[j][h]);
        }
      }
    }
    for (int s=0; s<4; s++) {
      //      cout << "suit " << s << ":\n";
      lbsum = 0;
      ubsum = 0;
      for (int h=0; h<4; h++) {
        lbsum += cstr.suit_lb[h][s];
        ubsum += cstr.suit_ub[h][s];
        //cout << h << ": " << cstr.suit_lb[h][s] << " - " << cstr.suit_ub[h][s] << ", ";
      }
      //cout << endl;
      for (int h=0; h<4; h++) {
        if (13-(lbsum-cstr.suit_lb[h][s]) < cstr.suit_ub[h][s]) {
          cstr.suit_ub[h][s] = 13-(lbsum-cstr.suit_lb[h][s]);
          //cout << "setting ub for " << h << " to " << cstr.suit_ub[h][s];
        }
        if (13-(ubsum-cstr.suit_ub[h][s]) > cstr.suit_lb[h][s]) {
          cstr.suit_lb[h][s] = 13-(ubsum-cstr.suit_ub[h][s]);
          //cout << "setting lb for " << h << " to " << cstr.suit_lb[h][s];
        }
      }
    }
    for (int h=0; h<4; h++) {
      lbsum = 0;
      ubsum = 0;
      for (int s=0; s<4; s++) {
        lbsum += cstr.suit_lb[h][s];
        ubsum += cstr.suit_ub[h][s];
      }
      for (int s=0; s<4; s++) {
        if (13-(lbsum-cstr.suit_lb[h][s]) < cstr.suit_ub[h][s]) {
          cstr.suit_ub[h][s] = 13-(lbsum-cstr.suit_lb[h][s]);
        }
        if (13-(ubsum-cstr.suit_ub[h][s]) > cstr.suit_lb[h][s]) {
          cstr.suit_lb[h][s] = 13-(ubsum-cstr.suit_ub[h][s]);
        }
      }
    }
    for (int h=0; h<4; h++) {
      if (cstr.pts_lb[h] < cstr.hcp_lb[h]) {
        cstr.pts_lb[h] = cstr.hcp_lb[h];
      }
      if (cstr.hcp_ub[h] > cstr.pts_ub[h]) {
        cstr.hcp_ub[h] = cstr.pts_ub[h];
      }
    }
  }
}

// 0 means no error
int read_constraints(istream &fin, constraints &cstr) {
  string token;
  int hand;
  bool bounds_on = false;
  int lb,ub;
  for (int h=0; h<4; h++) {
    cstr.hcp_lb[h] = 0;
    cstr.hcp_ub[h] = 37;
    cstr.pts_lb[h] = 0;
    cstr.pts_ub[h] = 37;
    for (int s=0; s<4; s++) {
      cstr.suit_lb[h][s] = 0;
      cstr.suit_ub[h][s] = 13;
    }
  }
  while (true) {
    fin >> token;
    to_lower(token);
    token = strip(token);
    int T = token.size();
    if (token == "end" || token == "done") break;
    if (bounds_on) {
      bounds_on = false;
      if (token == "hcp" || token == "high" || token == "high-card" || token == "high_card" || token == "high-card-points" || token == "high_card_points") {
        cstr.hcp_lb[hand] = lb;
        cstr.hcp_ub[hand] = ub;
      } else if (token == "pts" || token == "total" || token == "totalpoints" || token == "total_points" || token == "total-points" || token == "points") {
        cstr.pts_lb[hand] = lb;
        cstr.pts_ub[hand] = ub;
      } else if (token == "s" || token == "spades") {
        cstr.suit_lb[hand][0] = lb;
        cstr.suit_ub[hand][0] = ub;
      } else if (token == "h" || token == "hearts") {
        cstr.suit_lb[hand][1] = lb;
        cstr.suit_ub[hand][1] = ub;
      } else if (token == "d" || token == "diamonds") {
        cstr.suit_lb[hand][2] = lb;
        cstr.suit_ub[hand][2] = ub;
      } else if (token == "c" || token == "clubs") {
        cstr.suit_lb[hand][3] = lb;
        cstr.suit_ub[hand][3] = ub;
      } else {
        return 2020;
      }
    }
    else {
      // see if we're getting a new player's constraints
      switch (token[0]) {
      case 'n':
        hand = 0; continue;
      case 'e':
        hand = 1; continue;
      case 's':
        hand = 2; continue;
      case 'w':
        hand = 3; continue;
      }
      // otherwise, get bounds
      bounds_on = true;
      int dash_idx;
      for (dash_idx=0; dash_idx<T; dash_idx++) {
        if (token[dash_idx] == '-') break;
      }
      if (token[T-1] == '+') {
        lb = atoi(token.c_str());
        ub = 100;
      }
      else if (token[T-1] == '-') {
        ub = atoi(token.c_str());
        lb = 0;
      }
      else if (token[0] == '>') {
        if (token[1] == '=') {
          lb = atoi(token.c_str()+2);
        } else {
          lb = atoi(token.c_str()+1)+1;
        }
        ub = 100;
      }
      else if (token[0] == '<') {
        if (token[1] == '=') {
          ub = atoi(token.c_str()+2);
        } else {
          ub = atoi(token.c_str()+1)-1;
        }
        lb = 0;
      }
      else if (dash_idx < T) {
        lb = atoi(token.c_str());
        ub = atoi(token.c_str()+dash_idx+1);
      }
      else {
        lb = ub = atoi(token.c_str());
      }
    }
  }
  reduce_constraints(cstr);
  return 0;
}


void augment_init_constr(deal_ptl &dl, constraints &ctrs) {
  int hand[4];
  // start
  //cout << ctrs.suit_lb[0][0] << " " << ctrs.suit_ub[0][0] << endl;
  for (int h=0; h<4; h++) {
    // it's not me or the dummy, so we only know what they've played - update lower bounds
    if (dl.remainCards[h][0] == -1) {
      ctrs.hcp_lb[h] = max(ctrs.hcp_lb[h], hcp(dl.prevPlayed[h]));
      for (int s=0; s<4; s++) {
        ctrs.suit_lb[h][s] = max(ctrs.suit_lb[h][s], ncards(dl.prevPlayed[h][s]));
      }
    }
    else { // we know the entire hand
      for (int s=0; s<4; s++) {
        hand[s] = dl.remainCards[h][s] | dl.prevPlayed[h][s];
      }
      ctrs.hcp_lb[h] = ctrs.hcp_ub[h] = hcp(hand);
      ctrs.pts_lb[h] = ctrs.pts_ub[h] = total_pts(hand);
      for (int s=0; s<4; s++) {
        ctrs.suit_lb[h][s] = ctrs.suit_ub[h][s] = ncards(hand[s]);
      }
    }
  }
  // cout << ctrs.suit_lb[0][0] << " " << ctrs.suit_ub[0][0] << endl;
  reduce_constraints(ctrs);
}
