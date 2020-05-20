#ifndef _BRIDGE_UTILS_H
#define _BRIDGE_UTILS_H
#define DDS_DLL_PATH "/home/vietafan/Sams_folder/cpp_files/dds/include/dll.h"

#include <string>
using namespace std;

struct contract {
  int trump, number, decl;
  bool doubled, redoubled, vuln;
};

struct deal_ptl {
  contract ctr;
  bool vuln;
  int first, to_play, tricks;
  int currentTrickSuit[3];
  int currentTrickRank[3];
  int prevPlayed[4][4];
  int remainCards[4][4]; // -1 means unknown
};

struct trick {
  int lead;
  int suit[4];
  int rank[4];
};

struct deal_state {
  contract ctr;
  trick tricks[13];
  int hands[4][4];
  int tnum, tpos;
};

int hand_of_char(char direc);
int rank_of_char(char rank);
int suit_of_char(char suit);
char char_of_rank(int rank);
char char_of_suit(int suit);
char char_of_hand(int hand);

void holding_of_str(string &holding, int *res);
void print_hand(int *hand);
void print_partial_deal(deal_ptl &pd);

int ncards(int suit);
int count_cards(int *hand);
int hcp(int *hand);
int total_pts(int *hand);
#endif
