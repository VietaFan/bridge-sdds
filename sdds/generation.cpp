#include "bridge_utils.h"
#include "constraint.h"
#include <cstring>
#include <unordered_set>
#include DDS_DLL_PATH
#include <random>
#include <iostream>
using namespace std;

int choose[32][32];

void init_choose() {
  for (int i=0; i<32; i++) {
    choose[i][0] = 1;
    choose[i][i] = 1;
    for (int j=i+1; j<32; j++) choose[i][j] = 0;
  }
  for (int n=1; n<32; n++) {
    for (int k=1; k<=n; k++) {
      choose[n][k] = choose[n-1][k] + choose[n-1][k-1];
    }
  }
}

random_device r;
seed_seq ss({r(),r(),r(),r(),r(),r(),r(),r()});
mt19937 rng(ss);

// returns an n-bit integer selected uniformly at random from the set of
// n-bit integers with exactly k ones
int rand_choice(int n, int k) {
  uniform_int_distribution<int> unif(0, choose[n][k]-1);
  int x = unif(rng);
  int ans = 0;
  for (int i=n-1; i>-1; i--) {
    if (x >= choose[i][k]) {
      ans |= (1<<i);
      x -= choose[i][k];
      k--;
    }
  }
  return ans;
}

//int get_extra_vecs(int *distpt_lb, int *distpt_ub, constraints &ctrs, vector<int> &counts, vector<int> &num_left, vector<vector<int> > &extras) {
  

int get_count(deal_ptl &pd, constraints &ctrs, vector<int> &hcp_ranks, vector<int> &hcp_suits,
              vector<int> &opens, int index) {
  int hands[4][4];
  int p = 0;
  for (int i=0; i<4; i++) {
    for (int s=0; s<4; s++) {
      hands[i][s] = pd.prevPlayed[i][s];
    }
    while (p < 3 && i > opens[p]) p++;
    if (p == 3 || i < opens[p]) {
      //cout << "known position = " << i << endl;
      for (int s=0; s<4; s++) {
        hands[i][s] |= pd.remainCards[i][s];
      }
    }
  }
  int n = hcp_ranks.size();
  for (int i=0; i<n; i++) {
    hands[opens[index%3]][hcp_suits[i]] |= (1<<hcp_ranks[i]);
    index /= 3;
  }
  int hp[4];
  int dpt_lb[4], dpt_ub[4];
  for (int i=0; i<3; i++) {
    hp[i] = hcp(hands[opens[i]]);
    dpt_lb[i] = ctrs.pts_lb[opens[i]]-hp[i];
    dpt_ub[i] = ctrs.pts_ub[opens[i]]-hp[i];
  }

  int cards_left[4];
  for (int i=0; i<4; i++) {
    cards_left[i] = 13;
    for (int j=0; j<4; j++) {
      cards_left[i] -= ncards(hands[j][i]);
    }
  }
  int ns[4], nh[4], nd[4], nc[4];
  int ls[4], lh[4], ld[4], lc[4], us[4], uh[4], ud[4], uc[4];
  for (int p=0; p<3; p++) {
    ns[p] = ncards(hands[opens[p]][0]);
    nh[p] = ncards(hands[opens[p]][1]);
    nd[p] = ncards(hands[opens[p]][2]);
    nc[p] = ncards(hands[opens[p]][3]);
    ls[p] = ctrs.suit_lb[p][0]; us[p] = ctrs.suit_ub[p][0];
    lh[p] = ctrs.suit_lb[p][1]; uh[p] = ctrs.suit_ub[p][1];
    ld[p] = ctrs.suit_lb[p][2]; ud[p] = ctrs.suit_ub[p][2];
    lc[p] = ctrs.suit_lb[p][3]; uc[p] = ctrs.suit_ub[p][3];
  }  
  int dpt_s[4], dpt_h[4], dpt_d[4], dpt_c[4], dist_pts;
  
  int s2,h2,d2,c2;

  // yes, this is super messy and terrible style
  // recursion would have taken a lot of parameters though, and I didn't want
  // that much overhead since there are only 4 suits and 3 players for each suit to go through
  vector<vector<int> > allocs;
  vector<int> alloc(12,0);
  for (int s0=0; s0<=cards_left[0]; s0++) {
    if (ns[0]+s0 < ls[0] || ns[0]+s0 > us[0]) continue;
    dpt_s[0] = (ns[0]+s0 < 3 ? 3-(ns[0]+s0) : 0);
    for (int s1=0; s1<=cards_left[0]-s0; s1++) {
      if (ns[1]+s1 < ls[1] || ns[1]+s1 > us[1]) continue;
      s2 = cards_left[0]-s0-s1;
      dpt_s[1] = (ns[1]+s1 < 3 ? 3-(ns[1]+s1) : 0);
      if (ns[2]+s2 < ls[2] || ns[2]+s2 > us[2]) continue;
      dpt_s[2] = (ns[2]+s2 < 3 ? 3-(ns[3]+s2) : 0);
      alloc[0] = s0;
      alloc[1] = s1;
      alloc[2] = s2;
      for (int h0=0; h0<=cards_left[1]; h0++) {
        if (nh[0]+h0 < lh[0] || nh[0]+h0 > uh[0]) continue;
        dpt_h[0] = (nh[0]+h0 < 3 ? 3-(nh[0]+h0) : 0);
        for (int h1=0; h1<=cards_left[1]-h0; h1++) {
          if (nh[1]+h1 < lh[1] || nh[1]+h1 > uh[1]) continue;
          h2 = cards_left[1]-h0-h1;
          dpt_h[1] = (nh[1]+h1 < 3 ? 3-(nh[1]+h1) : 0);
          if (nh[2]+h2 < lh[2] || nh[2]+h2 > uh[2]) continue;
          dpt_h[2] = (nh[2]+h2 < 3 ? 3-(nh[2]+h2) : 0);
          alloc[3] = h0;
          alloc[4] = h1;
          alloc[5] = h2;
          for (int d0=0; d0<=cards_left[2]; d0++) {
            if (nd[0]+d0 < ld[0] || nd[0]+d0 > ud[0]) continue;
            dpt_d[0] = (nd[0]+d0 < 3 ? 3-(nd[0]+d0) : 0);
            for (int d1=0; d1<=cards_left[2]-d0; d1++) {
              if (nd[1]+d1 < ld[1] || nd[1]+d1 > ud[1]) continue;
              d2 = cards_left[2]-d0-d1;
              dpt_d[1] = (nd[1]+d1 < 3 ? 3-(nd[1]+d1) : 0);
              if (nd[2]+d2 < ld[2] || nd[2]+d2 > ud[2]) continue;
              dpt_d[2] = (nd[2]+d2 < 3 ? 3-(nd[2]+d2) : 0);
              alloc[6] = d0;
              alloc[7] = d1;
              alloc[8] = d2;
              for (int c0=0; c0<=cards_left[3]; c0++) {
                if (nc[0]+c0 < lc[0] || nc[0]+c0 > uc[0]) continue;
                dpt_c[0] = (nc[0]+c0 < 3 ? 3-(nc[0]+c0) : 0);
                for (int c1=0; c1<=cards_left[3]-c0; c1++) {
                  if (nc[1]+c1 < lc[1] || nc[1]+c1 > uc[1]) continue;
                  c2 = cards_left[3]-c0-c1;
                  dpt_c[1] = (nc[1]+c1 < 3 ? 3-(nc[1]+c1) : 0);
                  if (nc[2]+c2 < lc[2] || nc[2]+c2 > uc[2]) continue;
                  dpt_c[2] = (nc[2]+c2 < 3 ? 3-(nc[2]+c2) : 0);
                  alloc[9] = c0;
                  alloc[10] = c1;
                  alloc[11] = c2;
                  bool bad = false;
                  for (int p=0; p<3; p++) {
                    dist_pts = dpt_s[p]+dpt_h[p]+dpt_d[p]+dpt_c[p];
                    if (dist_pts < dpt_lb[p] || dist_pts > dpt_ub[p]) {
                      bad = true;
                      break;
                    }
                  }
                  if (bad) continue;
                  allocs.push_back(alloc);
                }
              }
            }
          }
        }
      }
    }
  }
  if (allocs.size() > 0) {
    /*cout << "cards left: ";
    for (int i=0; i<4; i++) {
      cout << cards_left[i] << " of " << ("SHDC"[i]) << ", ";
    }
    cout << endl;
    cout << "bounds for distribution points:\n";
    for (int i=0; i<3; i++) {
      cout << dpt_lb[i] << " - " << dpt_ub[i] << endl;
    }
    for (int i=0; i<4; i++) {
      cout << ("NESW"[i]) << ": ";
      print_hand(hands[i]);
      cout << " -- " << hcp(hands[i]) << " hcp" << endl;
    }
    cout << "# possible allocs: " << allocs.size() << endl;
    cout << "first alloc:\n";*/
    for (int s=0; s<4; s++) {
      int n=2;
      for (int p=0; p<3; p++) {
        for (int i=0; i<allocs[0][3*s+p]; i++) {
          hands[opens[p]][s] |= (1<<n);
          n++;
        }
      }
    }
    /*
    for (int i=0; i<4; i++) {
      cout << ("NESW"[i]) << ": ";
      print_hand(hands[i]);
      cout << " -- " << hcp(hands[i]) << " hcp, " << total_pts(hands[i]) << " pts\n";
    }
    cout << endl;*/
  }
  return allocs.size();
}

void precompute3(deal_ptl &pd, constraints &ctrs, vector<int> &hcp_ranks, vector<int> &hcp_suits,
                 vector<int> &opens, vector<int> &indices, vector<int> &counts) {
  int pos = 0;
  for (int i=0; i<4; i++) {
    if (pd.remainCards[i][0] >= 0) {
      pos = i;
    }
  }
  for (int i=0; i<4; i++) {
    if (i != pos) {
      opens.push_back(i);
    }
  }
  hcp_ranks.clear();
  hcp_suits.clear();
  int leftover[4];
  for (int s=0; s<4; s++) {
    leftover[s] = 65535;
    leftover[s] &= (~pd.remainCards[pos][s]);
    for (int i=0; i<4; i++) {
      leftover[s] &= ~(pd.prevPlayed[i][s]);
    }
  }
  //cout << leftover[0] << " " << leftover[1] << " " << leftover[2] << " " << leftover[3];
  for (int s=0; s<4; s++) {
    for (int r=11; r<15; r++) {
      if (leftover[s] & (1<<r)) {
        hcp_ranks.push_back(r);
        hcp_suits.push_back(s);
      }
    }
  }
  int n = hcp_ranks.size();
  //cout << " " << n << endl;
  int player[16], card_pts[16];
  int N = 1;
  for (int i=0; i<n; i++) {
    N *= 3;
    player[i] = 0;
    card_pts[i] = hcp_ranks[i]-10;
  }
  int hp[4];
  hp[1] = hcp(pd.prevPlayed[opens[1]]);
  hp[2] = hcp(pd.prevPlayed[opens[2]]);
  hp[0] = 40-hcp(pd.remainCards[pos])-hp[1]-hp[2];
  int cards[4];
  cards[1] = count_cards(pd.prevPlayed[opens[1]]);
  cards[2] = count_cards(pd.prevPlayed[opens[2]]);
  cards[0] = count_cards(pd.prevPlayed[opens[0]])+n;
  int hpmin[4], hpmax[4];
  for (int i=0; i<3; i++) {
    hpmin[i] = ctrs.hcp_lb[opens[i]];
    hpmax[i] = ctrs.hcp_ub[opens[i]];
  }
  bool ok = true;
  //int t=0;
  for (int deal=0; deal<N; deal++) {
    ok = true;
    for (int i=0; i<3; i++) {
      if (hp[i] < hpmin[i] || hp[i] > hpmax[i] || cards[i] > 13) {
        ok = false;
        break;
      }
    }
    if (ok) {
      /*if (t < 4) {
        cout << hp[0] << ", " << hp[1] << ", 11, " << hp[2] << endl;
        get_count(pd, ctrs, hcp_ranks, hcp_suits, opens, deal);
        t++;
        }*/
      indices.push_back(deal);
    }
    hp[player[0]] -= card_pts[0];
    cards[player[0]]--;
    player[0]++;
    cards[player[0]]++;
    hp[player[0]] += card_pts[0]; // has no effect if player[0] is 3
    for (int i=0; player[i]==3; i++) {
      player[i] = 0;
      cards[0]++;
      hp[0] += card_pts[i];
      cards[player[i+1]]--;
      hp[player[i+1]] -= card_pts[i+1];
      player[i+1]++;
      cards[player[i+1]]++;
      hp[player[i+1]] += card_pts[i+1];
    }
  }
  //cout << "# valid hcp distributions: " << indices.size() << endl;
  for (int i=0; i<indices.size(); i++) {
    get_count(pd, ctrs, hcp_ranks, hcp_suits, opens, indices[i]);
  }
}
    

void generate_deals3(deal_ptl &pd, constraints &ctrs, int ndeals, vector<deal> &dls) {
  
}

void generate_deal(deal_ptl &pd, constraints &ctrs, deal &dl, unordered_set<int> &exclude) {
  dl.trump = pd.ctr.trump;
  dl.first = pd.first;
  memcpy(dl.currentTrickSuit, pd.currentTrickSuit, 12);
  memcpy(dl.currentTrickRank, pd.currentTrickRank, 12);
  for (int i=0; i<4; i++) {
    memcpy(dl.remainCards[i], pd.remainCards[i], 16);
  }

  int left[4];
  for (int s=0; s<4; s++) {
    left[s] = (1<<15)-4;
    for (int h=0; h<4; h++) {
      left[s] &= ~pd.prevPlayed[h][s];
      if (pd.remainCards[h][s] >= 0) {
        left[s] &= ~pd.remainCards[h][s];
      }
    }
  }

  int unk1,unk2;
  for (unk1=0; pd.remainCards[unk1][0] >= 0; unk1++);
  for (unk2=unk1+1; pd.remainCards[unk2][0] >= 0; unk2++);
  
  int cards1[4], cards2[4], n1, n2, t1, t2;
  int iters=0;
  int hcp1,hcp2,pts1,pts2;
  int randy;

  int spots1=0, spots_tot=0;
  int suits_left[32], ranks_left_pow2[32];
  for (int s=0; s<4; s++) {
    for (int r=2; r<15; r++) {
      if (left[s] & (1<<r)) {
        suits_left[spots_tot] = s;
        ranks_left_pow2[spots_tot] = (1<<r);
        spots_tot++;
      }
    }
  }
  spots1 = 13;
  for (int s=0; s<4; s++) {
    spots1 -= ncards(pd.prevPlayed[unk1][s]);
  }

  while(true) { 
  restart:
    iters++;
    //    if (iters%1000 == 0) cout << iters << endl;
    t1 = t2 = 0;
    randy = rand_choice(spots_tot, spots1);
    for (int s=0; s<4; s++) {
      cards1[s] = pd.prevPlayed[unk1][s];
      cards2[s] = pd.prevPlayed[unk2][s];
    }
    for (int i=0; i<spots_tot; i++) {
      if (randy & (1<<i)) {
        cards1[suits_left[i]] |= ranks_left_pow2[i];
      } else {
        cards2[suits_left[i]] |= ranks_left_pow2[i];
      }
    }
    for (int s=0; s<4; s++) {
      n1 = ncards(cards1[s]);
      n2 = ncards(cards2[s]);
      if (n1 < ctrs.suit_lb[unk1][s] || n1 > ctrs.suit_ub[unk1][s] ||
          n2 < ctrs.suit_lb[unk2][s] || n2 > ctrs.suit_ub[unk2][s]) goto restart;
      t1 += n1;
      t2 += n2;
    }
    //    cout << "distribution ok\n";
    if (t1 != 13 || t2 != 13) continue;
    hcp1 = hcp(cards1);
    hcp2 = hcp(cards2);
    if (hcp1 < ctrs.hcp_lb[unk1] || hcp1 > ctrs.hcp_ub[unk1] ||
        hcp2 < ctrs.hcp_lb[unk2] || hcp2 > ctrs.hcp_ub[unk2]) continue;
    // cout << "hcp and distribution ok\n";
    pts1 = total_pts(cards1);
    pts2 = total_pts(cards2);
    if (pts1 < ctrs.pts_lb[unk1] || pts1 > ctrs.pts_ub[unk1] ||
        pts2 < ctrs.pts_lb[unk2] || pts2 > ctrs.pts_ub[unk2]) continue;
    if (exclude.count(randy)) continue;
    exclude.insert(randy);
    break;
  }

  for (int i=0; i<4; i++) {
    dl.remainCards[unk1][i] = cards1[i] & ~pd.prevPlayed[unk1][i];
    dl.remainCards[unk2][i] = cards2[i] & ~pd.prevPlayed[unk2][i];
  }
  //cout << "#iters: " << iters << endl;
} 

void generate_boards(deal_ptl &pd, constraints &ctrs, int nboards, boards &bds) {
  bds.noOfBoards = nboards;
  unordered_set<int> exclude;
  /*  for (int s=0; s<4; s++) {
    for (int i=0; i<4; i++) {
      cout << ctrs.suit_lb[i][s] << " - " << ctrs.suit_ub[i][s] << endl;
    }
    } */
  for (int i=0; i<nboards; i++) {
    //cout << "generating board " << i << endl;
    bds.target[i] = -1;
    bds.solutions[i] = 3;
    bds.mode[i] = 0;
    generate_deal(pd, ctrs, bds.deals[i], exclude);
  }
}
