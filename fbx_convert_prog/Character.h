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

using namespace std;
using namespace glm;
enum weapon {
    sword,
    spear,
    axe,
    bow,
    magic
};
class Character {
public:
    vec3 position;
    weapon weaponclass;
    bool hasShield;
    int health;
    string name;
    int isCharacter = 0; // is 1 when character actually exists
    GLuint texture;
    int team; //1 for Red Team, 2 for Blue Team
    int moves;
    // constructors
    Character();
    Character(string name, vec3 position, weapon weaponclass, bool hasShield, int health, GLuint charTex, int charTeam, int numMoves);
};
