
# SaiDama : Reverse checkers


By Facurib, Fernandez, Ouano, Trazona

## PROJECT OVERVIEW

SciDama is an underrated checkers-like game. Instead of depleting the opponent’s chips, your aim is to deplete your chips. It is a game where the fewer chips you have, the more chances you have at winning a round. This program aims to make the Filipino SaiDama board game playable on a computer, as well as to popularize the game.

## FEATURES

- Interactive UI
- Chessboard display
- Easy-to-read game manual
- Offline 2 player game
- Leaderboards

## RULES AND MECHANICS

1. The first player to start the game and initiate a move will be the one who picks the white chips, which will be decided by the two players. 
2. Any invalid input will be discarded. In particular, the program will keep on asking the user for another input until the input is right.
3. The winner of the game is the first player to have all its chips consumed by the opponent. 
4. Consequently, the winning player will earn a placement in the leaderboard after the game.


## HOW TO COMPILE/RUN THE PROGRAM
### Compiling
#### Compile in VS Code Terminal
```sh
gcc main.c captures.c game.c leaderboard.c move.c utils.c -o saidama
```

### Running
#### Windows
```cmd
saidama.exe
```

#### Linux:
```sh
./saidama
```
#### Gameplay
1. In performing a move, the player chooses the chip based on its coordinate appended by a '-', which is followed by its intended place. For instance: *6-9 implies that the chip on the 6th position will be moved to the 9th position as shown in the diagram below.*
2.



## SAMPLE INPUT/OUTPUT


## CONCEPTS
- Arrays 
    - The chessboard is stored in a single row array with **64** elements.
    - The chessboard is printed by line, which appears to be a **2D-multidimensional** array.
- Functions
    - Functions, such as **print_leader** & **run_game_loop**, were implemented for the algorithm of the game
- Pointers 
    - Structures serve as pointers that are passed to a function.
- Structures
    - They are particularly useful in the movement of the chips, in parsing these movements, and in preserving the game-state.
- File manipulation
    - File manipulation is used to read and write the leaderboards file.
- Linked list
    - Linked list is used to predict the capture moves in **find_captures**