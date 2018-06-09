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
#include "Board.h"

using namespace std;
using namespace glm;

//need to implement in the main

class Team {
public:
    int colour; // 1 for team 1, 2 for team 2
    int turnNumber;
    int teamEndTurn; //check if you want to end the turn, 1 for true end turn, 0 for false
    int teamSumMoves;

    vector<Character> characters;

};