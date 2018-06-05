#include "Character.h"

Character::Character() {
    isCharacter = 0;
}
Character::Character(string charName, vec3 charPos, weapon curWeapon, bool shield, int charHealth, GLuint charTex, int charTeam, int numMoves) {
    position = charPos;
    weaponclass = curWeapon;
    hasShield = shield;
    name = charName;
    health = charHealth;
    isCharacter = 1;
    texture = charTex;
    team = charTeam;
    moves = numMoves;
}