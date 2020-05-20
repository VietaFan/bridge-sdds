#include "bridge_utils.h"
#include <iostream>
using namespace std;
  
int hand_of_char(char direc) {
  switch (direc & (~32)) {
  case 'N':
    return 0;
  case 'E':
    return 1;
  case 'S':
    return 2;
  case 'W':
    return 3;
  }
  return -1;
}
    
int rank_of_char(char rank) {
  switch(rank & (~32)) {
  case 'A': return 14;
  case 'K': return 13;
  case 'Q': return 12;
  case 'J': return 11;
  case 'T': return 10;
  default: return rank-'0';
  }
}

int suit_of_char(char suit) {
  switch(suit & (~32)) {
  case 'S': return 0;
  case 'H': return 1;
  case 'D': return 2;
  case 'C': return 3;
  case 'N': return 4;
  default: return -1;
  }
}

char char_of_rank(int rank) {
  switch(rank) {
  case 14: return 'A';
  case 13: return 'K';
  case 12: return 'Q';
  case 11: return 'J';
  case 10: return 'T';
  default: return '0'+rank;
  }
}

char char_of_suit(int suit) {
  return "SHDC"[suit];
}

char char_of_hand(int hand) {
  return "NESW"[hand];
}

void holding_of_str(string &holding, int *res) {
  int cards = 0, suit;
  for (int i=0; i<4; i++) res[i] = 0;
  for (char c: holding) {
    suit = suit_of_char(c);
    if (suit < 0) {
      cards |= (1<<rank_of_char(c));
    } else {
      res[suit] = cards;
      cards = 0;
    }
  }
}

int ncards(int suit) {
  int count = 0;
  for (int i=4; i<(1<<15); i<<=1) {
    count += ((suit & i) > 0);
  }
  return count;
}

int hcp(int *hand) {
  int h,points=0;
  for (int i=0; i<4; i++) {
    h = hand[i];
    for (int j=1; j<=4; j++) {
      if (h&(1024<<j)) {
        points += j;
      }
    }
  }
  return points;
}

int total_pts(int *hand) {
  int points = hcp(hand);
  int suit_ct;
  for (int i=0; i<4; i++) {
    suit_ct = ncards(hand[i]);
    points += (suit_ct < 3) ? (3-suit_ct) : 0;
  }
  return points;
}

void print_hand(int *hand) {
  for (int s=0; s<4; s++) {
    if (hand[s] == 0) continue;
    for (int r=14; r>1; r--) {
      if (hand[s] & (1<<r)) {
        cout << char_of_rank(r);
      }
    }
    cout << char_of_suit(s);
  }
}

int count_cards(int *hand) {
  int total = 0;
  for (int s=0; s<4; s++) {
    total += ncards(hand[s]);
  }
  return total;
}

void print_partial_deal(deal_ptl &pd) {
  cout << "contract: ";
  cout << pd.ctr.number << char_of_suit(pd.ctr.trump) << char_of_hand(pd.ctr.decl);
  if (pd.ctr.doubled) cout << "x";
  if (pd.ctr.redoubled) cout << "x";
  if (pd.vuln) {
    cout << " vulnerable\n";
  } else {
    cout << " not vuln\n";
  }
  cout << "player starting trick: " << char_of_hand(pd.first) << "\n";
  cout << "to play: " << char_of_hand(pd.to_play) << "\n";
  cout << "# tricks taken by side to play: " << pd.tricks << "\n";

  cout << "trick so far:\n";
  for (int i=0; i<3; i++) {
    cout << pd.currentTrickRank[i] << " " << pd.currentTrickSuit[i] << "\n";
  }
  cout << "\n";
  for (int h=0; h<4; h++) {
    cout << char_of_hand(h) << " player:\n";
    cout << "prev. played: ";
    print_hand(pd.prevPlayed[h]);
    cout << "\nremaining cards: ";
    if (pd.remainCards[h][0] == -1) {
      cout << "unknown\n";
    } else {
      print_hand(pd.remainCards[h]);
      cout << "\n";
    }
  }
}
