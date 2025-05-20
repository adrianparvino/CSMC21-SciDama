
# SaiDama : Reverse checkers


By Facurib, Fernandez, Ouano, Trazona

## PROJECT OVERVIEW

SciDama is an underrated checkers-like game. Instead of depleting the opponent’s chips, your aim is to deplete your chips. It is a game where the fewer chips you have, the more chances you have at winning a round. This program aims to make the Filipino SaiDama board game playable on a computer, as well as to popularize the game.

## FEATURES

- Interactive UI
- Easy-to-read game manual
- Offline 2 player game
- Leaderboards

## HOW TO COMPILE/RUN THE PROGRAM
### Compiling
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


## SAMPLE INPUT/OUTPUT


## CONCEPTS
- Arrays 
    - The chessboard is stored on a single row array with 64 elements
    - The chessboard is printed by line, which makes it look like a 2D-multidimensional array
- Functions
    - Functions such as [insert ya] were implemented for the algorithm of the game
- Pointers
    - Pointer 
- File manipulation
    - File manipulation is used to read and write the leaderboards file