# bridge-sdds
Stochastic Double-Dummy Solver
site: http://bridge-sdds.herokuapp.com

This improves on a standard double dummy solver to evaluate declarer and defensive plays. The goal is to be able to determine what the optimal play for a situation was a priori, including constraints from the bidding but not unknown information about other players' hands. To evaluate plays, it generates N (N <= 200) random boards with the given deals for the current player and dummy which satisfy the constraints provided, runs a double-dummy solver on each, and averages the results. Although it's still a crude approximation, we can use this to estimate the expected number of IMPs gained or lost by playing a specific card against a partnership with the same deal, contract, and earlier plays who made a different choice, assuming both sides play optimally afterwards.

# Input specification:

information about deal state:

(example:)
3NS n 2
:2STS4S6S :
me S AJ6ST7HAKQJ63DK3C
dummy TSAQ952H9DQ97542C

The first line contains the contract, a character that is either v (for vulnerable) or n (non-vulnerable), and the number of tricks played (including the trick for the play being simulated), k.
The second line contains a space-separated list of k tricks. Each one consists of a colon followed by up to 4 2-character sequences, representing the cards that have been played. (The last one will have 0-3 cards.) 
The third and fourth lines specify the initial cards held by the person to play (who can be either the declarer or a defender).

known constraints on other hands:
(example:)
west: 5+ spades, 10+ hcp, 10+ pts
east: 3+ spades

This can have up to 4 lines, possibly indicating constraints for each position. The variables that you can constrain are number of cards of each suit (spades, hearts, clubs, diamonds), HCPs (hcp), and total points (pts). The colon and comma in the above specification are optional. 6-10, >=6, 6+, 10-, <=10, 8 are all valid syntax.

# Technical details
The site uses Heroku with Flask and socket.io to handle queries, connected to a Python program on a Raspberry Pi running python_socketio. The underlying double dummy simulations are run on a Raspberry Pi using Bo Haglund and Soren Hein's optimized C++ double dummy solver: https://github.com/dds-bridge/dds. 
