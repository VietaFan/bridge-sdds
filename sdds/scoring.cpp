#include "scoring.h"
#include "bridge_utils.h"
#include <vector>
using namespace std;

// returns the number of MPs for the declaring side
int get_mps(contract &ctr, int ntricks) {
  int delta = ntricks-6-ctr.number;
  if (delta < 0) { // not made
    switch (ctr.vuln*4+ctr.doubled*2+ctr.redoubled) {
    case 0: // not vulnerable, not doubled
      return 50*delta;
    case 2: // not vulnerable, doubled
      if (delta == -1) {
        return -100;
      } else if (delta >= -3) {
        return 200*delta+100;
      } else {
        return 300*delta+400;
      }
    case 3: // not vulnerable, redoubled
      if (delta == -1) {
        return -200;
      } else if (delta >= -3) {
        return 400*delta+200;
      } else {
        return 600*delta+800;
      }
    case 4: // vulnerable, undoubled
      return 100*delta;
    case 6: // vulnerable, doubled
      return 300*delta+100;
    case 7: // vulnerable, redoubled
      return 600*delta+200;
    }
  }
  int score = 0;
  // contract points
  if (ctr.trump < 2) { // major contract
    score = ctr.number*30*(1+ctr.doubled+2*ctr.redoubled);
  } else if (ctr.trump < 4) { // minor contract
    score = ctr.number*20*(1+ctr.doubled+2*ctr.redoubled);
  } else { // notrump contract
    score = (ctr.number*30+10)*(1+ctr.doubled+2*ctr.redoubled);
  }
  // bonus points
  if (ctr.number == 7) {
    score += (ctr.vuln ? 1500 : 1000);
  } else if (ctr.number == 6) {
    score += (ctr.vuln ? 750 : 500);
  } else if (score >= 100) {
    score += (ctr.vuln ? 500 : 300);
  } else {
    score += 50;
  }
  // overtrick points
  if (!ctr.doubled) {
    if (ctr.trump == 3 || ctr.trump == 4) { // minor
      score += 20*delta;
    } else {
      score += 30*delta;
    }
  } else {
    score += 100*delta*(1+ctr.redoubled)*(1+ctr.vuln);
  }
  return score;
}
int mp_thresholds[] = {20, 50, 90, 130, 170, 220, 270, 320,
                       370, 430, 500, 600, 750, 900, 1100, 1300,
                       1500, 1750, 2000, 2250, 2500, 3000, 3500, 4000, 2020000};

int get_imps(int mp_delta) {
  if (mp_delta < 0) {
    return -get_imps(-mp_delta);
  }
  int imps = 0;
  while (mp_delta >= mp_thresholds[imps]) {
    imps++;
  }
  return imps;
}

void get_imps_mat(contract &ctr, vector<vector<int> > &impmat) {
  impmat.clear();
  for (int i=0; i<14; i++) {
    vector<int> v;
    for (int j=0; j<14; j++)
      v.push_back(0);
    impmat.push_back(v);
  }
  int mps[14];
  for (int ntricks=0; ntricks<14; ntricks++) {
    mps[ntricks] = get_mps(ctr, ntricks);
  }
  for (int n1=0; n1<14; n1++) {
    for (int n2=0; n2<=n1; n2++) {
      impmat[n1][n2] = get_imps(mps[n1]-mps[n2]);
      impmat[n2][n1] = -impmat[n1][n2];
    }
  }
}
