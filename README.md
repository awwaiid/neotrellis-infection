Infection (aka Ataxx)

A two-player game for the 8x8 Neotrellis, written in Arduino/C++.

I can't remember if I first played this as a sub-game in The 7th Guest or as the 7-Up Spot game on my Commodore 64. Either way it was fun, and I played against the computer until I could beat it.

This two player game takes place on a grid with a few initial pieces for each player. They take turns, and when it is your turn you pick one of your existing pieces and an empty square one or two spaces away including diagonal. You can infect squares (add a new piece, replicate, multiply) one square away including diagonal. You can jump, removing the original piece and placing it in a new location two squares away including diagonal. Either way all of the occupied squares that you land next to including diagonal are infected/mutated and become your pieces! Kinda like Othello or Reversi.

See https://en.wikipedia.org/wiki/Ataxx

I used the Neotrellis 8x8 (4 Neotrellis boards) kit, which comes with an Adafruit Feather. Only took two tries to get the hardwork working, not bad for my first soldering project in 20 years!
