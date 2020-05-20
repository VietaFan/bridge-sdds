#include "contract_in.h"
#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <vector>
#include "bridge_utils.h"
#include "str_utils.h"
#include "constraint.h"
#include DDS_DLL_PATH
using namespace std;

void read_contract(istream &fin, contract &ctr) {
  string s;
  fin >> s;
  ctr.number = s[0]-'0';
  ctr.trump = suit_of_char(s[1]);
  ctr.decl = hand_of_char(s[2]);
  ctr.doubled = s.size() >= 4;
  ctr.redoubled = s.size() == 5;
}  

/* example input:

3NWx
W :3CQC
N AQ742S982HAKDKT3C
S ...
E ...
W ...
*/
int parse_trick(int lead, int trump, string &tstr, int *ranks, int *suits) {
  int k, n=tstr.size();
  int best_score = -1, score, winner;
  for (k=0; 2*k+1 < n; k++) {
    ranks[k] = rank_of_char(tstr[2*k+1]);
    suits[k] = suit_of_char(tstr[2*k+2]);
    score = ranks[k]+(suits[k] == suits[0])*100+(suits[k] == trump)*1000;
    if (score > best_score) {
      winner = lead+k;
      best_score = score;
    }
  }
  for (; k<4; k++) {
    ranks[k] = 0;
    suits[k] = 0;
  }
  return winner%4;
}

void read_deal_full(istream &fin, deal &dl, contract &ctr) {
  read_contract(fin, ctr);
  dl.trump = ctr.trump;
  string lead_dir;
  fin >> lead_dir;
  dl.first = hand_of_char(lead_dir[0]);
  string play_seq; // : followed by cards played in order
  fin >> play_seq;
  play_seq += string(7-play_seq.size(), '-');
  for (int i=0; i<3; i++) {
    if (play_seq[2*i+1] == '-') {
      dl.currentTrickRank[i] = dl.currentTrickSuit[i] = 0;
      continue;
    }
    dl.currentTrickRank[i] = rank_of_char(play_seq[2*i+1]);
    dl.currentTrickSuit[i] = suit_of_char(play_seq[2*i+2]);
  }
  int cards_held[4];
  string direc, hld_str;
  for (int i=0; i<4; i++) {
    fin >> direc >> hld_str;
    holding_of_str(hld_str, cards_held);
    memcpy(dl.remainCards[hand_of_char(direc[0])], cards_held, 16);
  }
}

void read_boards(istream &fin, boards &bds, contract *ctrs) {
  fin >> bds.noOfBoards;
  for (int b=0; b<bds.noOfBoards; b++) {
    read_deal_full(fin, bds.deals[b], ctrs[b]);
    bds.target[b] = -1;
    bds.solutions[b] = 3;
    bds.mode[b] = 0;
  }
}

/* example input
3NWx n                            <contract> <n: declarers not vuln, v: declarers vuln>
3 :QHAH3H5H :5SJSAS2S :7DQD       <numTricks (including current, partially empty one)>  <list of >=1 tricks played so far> (in order, with : starting)
me S ...                          me <my-position> <my-hand>
dummy ...                         dummy <dummy-hand>
*/

void read_deal_partial(istream &fin, deal_ptl &dl) {
  read_contract(fin, dl.ctr);
  string vuln;
  fin >> vuln;
  dl.vuln = (vuln[0]|32) == 'v';
  dl.ctr.vuln = dl.vuln;
  int num_tricks;
  fin >> num_tricks;
  int lead, next_lead = (dl.ctr.decl+1)%4;
  int ranks[4], suits[4];
  string trick_str;
  for (int hand=0; hand<4; hand++) {
    for (int suit=0; suit<4; suit++) {
      dl.prevPlayed[hand][suit] = 0;
    }
  }
  int decl_tricks=0, opp_tricks=0;
  for (int tnum=0; tnum<num_tricks; tnum++) {
    fin >> trick_str;
    lead = next_lead;
    next_lead = parse_trick(lead, dl.ctr.trump, trick_str, ranks, suits);
    for (int i=0; i<4; i++) {
      dl.prevPlayed[(lead+i)%4][suits[i]] |= (1<<ranks[i]);
    }
    if (tnum < num_tricks-1) {
      if (next_lead%2 == dl.ctr.decl%2) {
        decl_tricks++;
      } else {
        opp_tricks++;
      }
    }
  }
  dl.first = lead;
  dl.to_play = (dl.first + (trick_str.size()-1)/2)%4;
  memcpy(dl.currentTrickSuit, suits, 12);
  memcpy(dl.currentTrickRank, ranks, 12);
  for (int hand=0; hand<4; hand++) {
    dl.prevPlayed[hand][0] &= ~1; // empty cards at the end of the trick have suit=rank=0; undo this
  }
  string name_str, pos_str, hand_str;
  int pos;
  for (int i=0; i<4; i++) {
    for (int j=0; j<4; j++) {
      dl.remainCards[i][j] = -1;
    }
  }
  int cards_held[4];
  for (int i=0; i<2; i++) {
    fin >> name_str;
    if (name_str == "me") {
      fin >> pos_str;
      pos = hand_of_char(pos_str[0]);
      if ((pos+dl.to_play)%2) {
        cout << "error! opponents are to play!!\n";
      }
      fin >> hand_str;
      holding_of_str(hand_str, cards_held);
      memcpy(dl.remainCards[pos], cards_held, 16);
      for (int s=0; s<4; s++) {
        dl.remainCards[pos][s] &= ~dl.prevPlayed[pos][s];
      }
      if (pos%2 == dl.ctr.decl%2) {
        dl.tricks = decl_tricks;
      } else {
        dl.tricks = opp_tricks;
      }
    }
    else if (name_str == "dummy") {
      fin >> hand_str;
      holding_of_str(hand_str, cards_held);
      int dpos = (dl.ctr.decl+2)%4;
      memcpy(dl.remainCards[dpos], cards_held, 16);
      for (int s=0; s<4; s++) {
        dl.remainCards[dpos][s] &= ~dl.prevPlayed[dpos][s];
      }
    }
  }
  fin >> name_str; // get "end" or "done"
}

void read_deal_state(istream &fin, deal_state &ds) {
  read_contract(fin, ds.ctr);
  string vuln;
  fin >> vuln >> ws;
  ds.ctr.vuln = (vuln[0] == 'v');
  int num_tricks;
  if (fin.peek() != ':') {
    fin >> num_tricks;
  }
  int lead, next_lead = (ds.ctr.decl+1)%4;
  int ranks[4], suits[4];
  string trick_str;
  for (int hand=0; hand<4; hand++) {
    for (int suit=0; suit<4; suit++) {
      ds.hands[hand][suit] = -1;
    }
  }
  int tnum;
  for (tnum=0;;tnum++) {
    fin >> trick_str >> ws;
    lead = next_lead;
    ds.tricks[tnum].lead = lead;
    next_lead = parse_trick(lead, ds.ctr.trump, trick_str,
                            ds.tricks[tnum].rank, ds.tricks[tnum].suit);
    if (fin.peek() != ':') break;
  }
  ds.tnum = tnum;
  ds.tpos = ((trick_str.size()-1)/2)%4;
  string name_str, pos_str, hand_str;
  int cards_held[4], pos;
  while (true) {
    fin >> name_str;
    to_lower(name_str);
    if (name_str == "end" || name_str == "done") {
      break;
    }
    if (hand_of_char(name_str[0] & (~32)) >= 0) { // n/e/w/s
      pos = hand_of_char(name_str[0] & (~32));
      fin >> hand_str;
      holding_of_str(hand_str, cards_held);
      memcpy(ds.hands[pos], cards_held, 16);
    }
    else if (name_str == "me") { // for backwards compatibility; it's followed by a direction
      continue;
    }
    else if (name_str == "dummy") {
      pos = (ds.ctr.decl+2)%4;
      fin >> hand_str;
      holding_of_str(hand_str, cards_held);
      memcpy(ds.hands[pos], cards_held, 16);
    }
  }
}

// the corresponding partial deal, using tricks up to and including tnum, where
// positions up to (but not including) tpos have been played and tpos is to play
// we assume the hands of the players in the vector known are known
// (this will either be just the current player if it's the initial lead, or
// the current player and the dummy otherwise)
void get_partial(deal_state &ds, int tnum, int tpos, vector<int> known, deal_ptl &dl) {
  dl.ctr = ds.ctr;
  dl.vuln = dl.ctr.vuln;
  dl.first = ds.tricks[tnum].lead;
  dl.to_play = (dl.first+tpos)%4;
  dl.tricks = 0;
  for (int i=1; i<=tnum; i++) {
    if ((ds.tricks[i].lead+dl.to_play)%2 == 0) { // we won that trick
      dl.tricks++;
    }
  }
  for (int i=0; i<3; i++) {
    dl.currentTrickSuit[i] = ds.tricks[tnum].suit[i];
    dl.currentTrickRank[i] = ds.tricks[tnum].rank[i];
  }
  for (int i=tpos; i<3; i++) {
    dl.currentTrickSuit[i] = 0;
    dl.currentTrickRank[i] = 0;
  }
  bool is_known[4];
  for (int i=0; i<4; i++) {
    is_known[i] = false;
  }
  for (int i=0; i<known.size(); i++) {
    is_known[known[i]] = true;
  }
  for (int i=0; i<4; i++) {
    for (int s=0; s<4; s++) {
      dl.remainCards[i][s] = ds.hands[i][s];
    }
  }
  for (int i=0; i<tnum; i++) {
    for (int j=0; j<4; j++) {
      dl.remainCards[(ds.tricks[i].lead+j)%4][ds.tricks[i].suit[j]] &= ~(1<<ds.tricks[i].rank[j]);
      dl.prevPlayed[(ds.tricks[i].lead+j)%4][ds.tricks[i].suit[j]] |= (1<<ds.tricks[i].rank[j]);
    }
  }
  for (int j=0; j<tpos; j++) {
    dl.remainCards[(ds.tricks[tnum].lead+j)%4][ds.tricks[tnum].suit[j]] &= ~(1<<ds.tricks[tnum].rank[j]);
    dl.prevPlayed[(ds.tricks[tnum].lead+j)%4][ds.tricks[tnum].suit[j]] |= (1<<ds.tricks[tnum].rank[j]);
  }
  for (int hand=0; hand<4; hand++) {
    dl.prevPlayed[hand][0] &= ~1; // empty cards at the end of the trick have suit=rank=0; undo this
  }
  for (int i=0; i<4; i++) {
    if (!is_known[i]) {
      for (int s=0; s<4; s++) {
        dl.remainCards[i][s] = -1;
      }
    }
  }
}

void read_state(istream &fin, deal_ptl &dl, constraints &ctrs) {
  read_deal_partial(fin, dl);
  read_constraints(fin, ctrs);
  augment_init_constr(dl, ctrs);
}

void read_state2(istream &fin, deal_state &ds, constraints &ctrs) {
  read_deal_state(fin, ds);
  read_constraints(fin, ctrs);
}

