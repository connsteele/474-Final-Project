#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <glad/glad.h>
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "WindowManager.h"
#include "Shape.h"
#include "Line.h"
#include "bone.h"
#include "Character.h"
#include "Team.h"

using namespace std;
using namespace glm;

class Board {
public:
    // Board properties
    int boardWidth;
    int boardHeight;

    // Rendered Positions
    vector<vec3> mapBlocks;
    vector<int> characterIndices;
    vector<Character> characters;

    // Character Positions
    vector<vector<Character>> team1Pos;
    vector<vector<Character>> team2Pos;
    vector<vector<Character>> allCharPos;

    //variables to update the character positions
    static double moveCharX;
    static double moveCharY;

    // Constructors
    Board();
    Board(vector<vec3> blockPos, vector<vector<Character>> team1, vector<vector<Character>> team2, vector<vector<Character>> charsPos, int width, int height);

    // Board methods
    int checkWin();
    void convertCharToPhysicalCoordinates();

    // Character management methods
    int addCharacter(Character c, int x, int y);
    int hasCharacter(int x, int y);
    Character getCharacter(int x, int y);
    int moveCharacter(int charX, int charY, int destX, int destY);
    void removeCharacter(int x, int y);
    int startFight(Character a, Character b);

    // Extraneous methods
    int getBoardWidth();
    int getBoardHeight();

};