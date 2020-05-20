#include "results_proc.h"
#include "bridge_utils.h"
#include "scoring.h"
#include DDS_DLL_PATH
#include <vector>
#include <iostream>
#include <cmath>

using namespace std;

void calc_imp_table(deal_ptl &dl, full_results &res) {
  vector<vector<int> > imp_mat;
  get_imps_mat(dl.ctr, imp_mat);
  int count = 0;
  for (int i=0; i<=13; i++) {
    count += res.cr[0].decl_counts[i];
  }
  for (int i=0; i<res.n_cards; i++) {
    res.avg_imps[i][i] = 0.0;
    for (int j=0; j<i; j++) {
      double imptotal = 0.0;
      for (int k1=0; k1<=13; k1++) {
        for (int k2=0; k2<=13; k2++) {
          imptotal += imp_mat[k1][k2]*res.cr[i].decl_counts[k1]*res.cr[j].decl_counts[k2];
        }
      }
      imptotal /= (count*count);
      if ((dl.ctr.decl+dl.to_play)%2 == 0) {
        res.avg_imps[i][j] = imptotal;
        res.avg_imps[j][i] = -imptotal;
      } else {
        res.avg_imps[i][j] = -imptotal;
        res.avg_imps[j][i] = imptotal;
      }
    }
  }
  for (int i=0; i<res.n_cards; i++) {
    res.sd_imps[i][i] = 0.0;
    for (int j=0; j<i; j++) {
      double impsqtotal = 0.0;
      for (int k1=0; k1<=13; k1++) {
        for (int k2=0; k2<=13; k2++) {
          impsqtotal += imp_mat[k1][k2]*imp_mat[k1][k2]*res.cr[i].decl_counts[k1]*res.cr[j].decl_counts[k2];
        }
      }
      impsqtotal /= (count*count);
      res.sd_imps[j][i] = res.sd_imps[i][j] = sqrt(impsqtotal - res.avg_imps[i][j]*res.avg_imps[i][j])/sqrt(count);
    }
  }
}

int get_results(deal_ptl &dl, solvedBoards &solved, full_results &results) {
  futureTricks ft;
  int n_choices = 0;
  for (int s=0; s<4; s++) {
    for (int r=2; r<15; r++) {
      if (dl.remainCards[dl.to_play][s] & (1<<r)) {
        results.cr[n_choices].suit = s;
        results.cr[n_choices].rank = r;
        for (int i=0; i<=13; i++) {
          results.cr[n_choices].counts[i] = 0;
        }
        n_choices++;
      }
    }
  }
  
  for (int b=0; b<solved.noOfBoards; b++) {
    ft = solved.solvedBoard[b];
    for (int k=0; k<ft.cards; k++) {
      for (int r=2; r<15; r++) {
        if (!((r == ft.rank[k]) || ((1<<r) & ft.equals[k]))) continue;
        for (int i=0; i<n_choices; i++) {
          if (ft.suit[k] == results.cr[i].suit && r == results.cr[i].rank) {
            if (ft.score[k] < 0) {
              cout << "yakkety-yak-yak-yak!!!!!\n";
            }
            results.cr[i].counts[ft.score[k]+dl.tricks]++;
          }
        }
      }
    }
  }
  if ((dl.to_play+dl.ctr.decl) % 2 == 0) {// we're declaring
    for (int i=0; i<n_choices; i++) {
      for (int j=0; j<=13; j++) {
        results.cr[i].decl_counts[j] = results.cr[i].counts[j];
      }
    }
  } else {
    for (int i=0; i<n_choices; i++) {
      for (int j=0; j<=13; j++) {
        results.cr[i].decl_counts[13-j] = results.cr[i].counts[j];
      }
    }
  }

  int total;
  results.n_cards = 0;
  card_results *target;
  for (int i=0; i<n_choices; i++) {
    total = 0;
    for (int j=0; j<=13; j++)
      total += results.cr[i].decl_counts[j];
    if (total > 0) {
      target = &results.cr[results.n_cards];
      target->suit = results.cr[i].suit;
      target->rank = results.cr[i].rank;
      for (int c=0; c<=13; c++) {
        target->counts[c] = results.cr[i].counts[c];
        target->decl_counts[c] = results.cr[i].decl_counts[c];
      }
      results.n_cards++;
    }
  }
  calc_imp_table(dl, results);
  return results.n_cards;
}

void raw_results_csv(ostream &out, full_results &res) {
  out << "play";
  for (int n=0; n<14; n++) {
    out << "," << n;
  }
  out << "\n";
  for (int r=0; r<res.n_cards; r++) {
    out << char_of_rank(res.cr[r].rank) << char_of_suit(res.cr[r].suit);
    for (int n=0; n<14; n++) {
      out << "," << res.cr[r].counts[n];
    }
    out << "\n";
  }
}

void imp_results_csv(ostream &out, full_results &res) {
  out << "play";
  for (int r=0; r<res.n_cards; r++) {
    out << "," << char_of_rank(res.cr[r].rank) << char_of_suit(res.cr[r].suit);
  }
  out << "\n";
  for (int r=0; r<res.n_cards; r++) {
    out << char_of_rank(res.cr[r].rank) << char_of_suit(res.cr[r].suit);
    for (int s=0; s<res.n_cards; s++) {
      out << "," << res.avg_imps[r][s];
    }
    out << "\n";
  }
}

void impsd_results_csv(ostream &out, full_results &res) {
  out << "play";
  for (int r=0; r<res.n_cards; r++) {
    out << "," << char_of_rank(res.cr[r].rank) << char_of_suit(res.cr[r].suit);
  }
  out << "\n";
  for (int r=0; r<res.n_cards; r++) {
    out << char_of_rank(res.cr[r].rank) << char_of_suit(res.cr[r].suit);
    for (int s=0; s<res.n_cards; s++) {
      out << "," << res.sd_imps[r][s];
    }
    out << "\n";
  }
}
