#include "Board.h"

// Constructors
Board::Board() {}

Board::Board(vector<vec3> blockPos, vector<vector<Character>> team1, vector<vector<Character>> team2, vector<vector<Character>> charsPos, int width, int height) {
    mapBlocks = blockPos;
    allCharPos = charsPos;
    boardWidth = width;
    boardHeight = height;
    team1Pos = team1;
    team2Pos = team2;
}

// Board methods
int Board::checkWin() {
    // returns 0 if player 1 wins, 1 if player 2 wins
    // check number of units on each team
    // if either team number of units = 0, declare victory for other team
    return 0; //temp
}

void Board::convertCharToPhysicalCoordinates() {
    // uses the 2D array of character positions and sets the character indices array
    // used to set the correct position to draw each character
    int counter = 0;
    // cout << "IN CONVERT\n";
    for (int i = 0; i < boardWidth; i++) {
        for (int j = 0; j < boardHeight; j++) {
            if (hasCharacter(i, j)) {
                characterIndices.push_back(counter); // gets indices of 2D and puts it into 1D array
                                                     //  convert to vec3 positions above each grid square and store in characters
                characters.push_back(getCharacter(i, j));
            }
            counter++;
        }
    }

    //cout << "character size: " << characters.size();
    for (int i = 0; i < characters.size(); i++) {
        characters[i].position = mapBlocks[characterIndices[i]] + vec3(2.75, 0.1, 5);
        //cout << "character[i] position.x: " << characters[i].position.x;
        //cout << "character[i] position.y: " << characters[i].position.y;
        //cout << "character[i] position.z: " << characters[i].position.z;
    }

}

// Character management methods
int Board::addCharacter(Character c, int x, int y) {
    // returns 1 if character is added successfully (i.e. no other unit exists in the same position), otherwise 0
    if (hasCharacter(x, y)) {
        return 0;
    }
    allCharPos.at(x).at(y) = c;
    return 0; //temp
}


int Board::hasCharacter(int x, int y) {
    // returns 1 if a character exists at x,y, otherwise 0
    if (allCharPos.at(x).at(y).isCharacter == NULL) {
        return 0;
    }
    return 1;
}

Character Board::getCharacter(int x, int y) {
    // returns character at x,y
    return allCharPos.at(x).at(y);
}

int Board::moveCharacter(int charX, int charY, int destX, int destY) { //fuck this method my guys 
                                                                       // implementation needed
                                                                       // check if character exists at that (charX,charY)
    if (hasCharacter(charX, charY)) {
        // if character exists
        cout << "Char @ tile x: " << charX << " y:" << charY;
        // check if destination point has a character
        if (hasCharacter(destX, destY)) {
            // if has character
            cout << "THERE WILL BE BLOOD" << endl;
            // just print something right now, or do something that shows that there will be a detection when selecting (start fight)
            return 1;
        }
        else {
            // if not have character
            // move current character to dest
            allCharPos.at(destX).at(destY) = getCharacter(charX, charY);
            removeCharacter(charX, charY);
            if (hasCharacter(charX, charY) != 0) {
                cout << "error removing character" << endl;
            }
            return 1;
        }
    }
    return 0;
}

void Board::removeCharacter(int x, int y) {
    // deletes character at that position
    allCharPos.at(x).at(y).isCharacter = 0;
}

int Board::startFight(Character a, Character b) {
    // return 0 if a wins, returns 1 if b wins
    return 0; //temp
}

// Extraneous methods
int Board::getBoardWidth() {
    return boardWidth;
}

int Board::getBoardHeight() {
    return boardHeight;
}