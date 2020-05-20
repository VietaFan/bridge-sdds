
  /*
  cout << "results:\n";
  int total_rc = 0;
  string cards[13];
  for (int i=0; i<n_choices; i++) {
    total_rc = 0;
    for (int t=0; t<14; t++) total_rc += results.cr[i].counts[t];
    if (!total_rc) continue;
    cout << char_of_rank(results.cr[i].rank) << char_of_suit(results.cr[i].suit) << ": ";
    cards[i] = "ab";
    cards[i][0] = char_of_rank(results.cr[i].rank);
    cards[i][1] = char_of_suit(results.cr[i].suit);
    for (int t=13; t>=0; t--) {
      if (results.cr[i].counts[t] == 0) continue;
      cout << t << "=>" << results.cr[i].counts[t] << "x, ";
    }
    cout << endl;
  }
  cout << n_choices << endl;
  cout << "\nIMP matrix\n      ";
  for (int i=0; i<n_choices; i++) {
    cout << cards[i] << "  ";
  }
  cout << endl;
  for (int i=0; i<n_choices; i++) {
    cout << cards[i] << "  ";
    for (int j=0; j<n_choices; j++) {
      cout << results.avg_imps[i][j] << " ";
    }
    cout << endl;
  }
  
  //read_deal_partial(cin, dl);
  /*  int hand[4];
  cout << "cards:\n";
  for (char h: string("NEWS")) {
    cout << h << ":\n";
    if (dl.remainCards[hand_of_char(h)][0] < 0) continue;
    for (char s: string("SHDC")) {
      hand[suit_of_char(s)] = dl.prevPlayed[hand_of_char(h)][suit_of_char(s)]|dl.remainCards[hand_of_char(h)][suit_of_char(s)];
      //cout << std::hex << hand[suit_of_char(s)] << std::dec << endl;
      for (char c: string("23456789TJQKA")) {
        if (hand[suit_of_char(s)] & (1<<rank_of_char(c))) {
          cout << c;
        }
      }
      cout << s << endl;
    }
  }
  return 0;*/
  //read_constraints(cin, ctrs);
  /*
  cout << "hcp bounds:\n";
  for (char c: string("NEWS")) {
    cout << c << ": " << ctrs.hcp_lb[hand_of_char(c)] << "-" << ctrs.hcp_ub[hand_of_char(c)] << "hcp,";
    for (char s: string("SHDC")) {
      cout << ctrs.suit_lb[hand_of_char(c)][suit_of_char(s)] << "-" << ctrs.suit_ub[hand_of_char(c)][suit_of_char(s)] << " " << s << ", ";
    }
    cout << endl;
  }

  augment_init_constr(dl, ctrs);*/
  /*  for (char c: string("NEWS")) {
    cout << c << ": " << ctrs.hcp_lb[hand_of_char(c)] << "-" << ctrs.hcp_ub[hand_of_char(c)] << " hcp, " << ctrs.pts_lb[hand_of_char(c)] << "-" << ctrs.pts_ub[hand_of_char(c)] << " total pts, ";
    for (char s: string("SHDC")) {
      cout << ctrs.suit_lb[hand_of_char(c)][suit_of_char(s)] << "-" << ctrs.suit_ub[hand_of_char(c)][suit_of_char(s)] << " " << s << ", ";
    }
    cout << endl;
  }

  //boardsPBN bo;


  char line[80];
  bool match;

#if defined(__linux) || defined(__APPLE__)
  SetMaxThreads(0);
#endif

  /*bo.noOfBoards = 3;
  for (int handno = 0; handno < 3; handno++)
  {
    bo.deals[handno].trump = trump[handno];
    bo.deals[handno].first = first[handno];

    bo.deals[handno].currentTrickSuit[0] = 0;
    bo.deals[handno].currentTrickSuit[1] = 0;
    bo.deals[handno].currentTrickSuit[2] = 0;

    bo.deals[handno].currentTrickRank[0] = 0;
    bo.deals[handno].currentTrickRank[1] = 0;
    bo.deals[handno].currentTrickRank[2] = 0;

    strcpy(bo.deals[handno].remainCards, PBN[handno]);

    bo.target [handno] = -1;
    bo.solutions[handno] = 3;
    bo.mode [handno] = 0;
    }

  res = SolveAllChunksBin(&bds, &solved, 1);

  if (res != RETURN_NO_FAULT)
  {
    ErrorMessage(res, line);
    printf("DDS error: %s\n", line);
  }

  for (int handno = 0; handno < 3; handno++)
  {
    //match = CompareFut(&solved.solvedBoard[handno], handno, 3);

    //sprintf(line,
    //         "SolveAllBoards, hand %d: solutions 3 %s\n",
    //        handno + 1, (match ? "OK" : "ERROR"));

    PrintHand(line, bds.deals[handno].remainCards);

    PrintFut(line, &solved.solvedBoard[handno]);
    }*/
