#include <iostream>
#include <cassert>

void outSplitter (char sign) {
    std::string splitter;
    splitter.assign(60, sign);
    std::cout << splitter << std::endl;
}

void outHeader () {
    outSplitter('=');
    std::cout << "Sea battle (expand console wide)" << std::endl;
    std::cout << "Signs: O - Ship; X - Sunken ship; * - Shot miss; . - Free cell" << std::endl;
    outSplitter('=');
}

std::string coordsDescription() {
    return "(row column)(0...9)";
}

bool coordsIsCorrect(int x, int y) {

    bool result = true;

    if (x < 0 || x > 9 || y < 0 || y > 9) {
        result = false;
        std::cout << "Enter values from 0 to 9!\n";
    }

    return result;

}

char signOfField(int fieldCode, bool opponentField) {

    fieldCode += 2;
    assert(fieldCode >= 0 && fieldCode <= 4);

    std::string usedSymbols = (opponentField ? "*X.." : "*X.O");

    return usedSymbols[fieldCode];

}

// Return formatted string of cell
std::string cellString(char cellChar) {

    std::string result;

    result += ' ';
    result += cellChar;
    result += ' ';

    return result;

}

void outField(int ownerField[][10], int opponentField[][10]) {

    for (int x = 0; x < 11; ++x) {
        bool isFirstLine = x == 0;
        if (isFirstLine) {
            std::cout << "  ";
        } else {
            std::cout << x - 1 << " ";
        }
        std::string outerLine;
        for (int y = 0; y < 10; ++y) {
            outerLine += cellString(isFirstLine
                    ? char(y + '0')
                    : signOfField(ownerField[x - 1][y], false));
        }
        outerLine += "   ";
        for (int y = 0; y < 10; ++y) {
            outerLine += cellString(isFirstLine
                    ? char(y + '0')
                    : signOfField(opponentField[x - 1][y], true));
        }
        std::cout << outerLine << std::endl;
    }
    outSplitter('-');
}

void standingShip(int ownerField[][10], int opponentField[][10], std::string playerID) {

    std::cout << "Player " << playerID << " standing ship...\n";
    outField(ownerField, opponentField);

    for (int shipSize = 1; shipSize <= 4; ++shipSize) {

        int countOfShip = 5 - shipSize;

        for (int shipCounter = 1; shipCounter <= countOfShip; ++shipCounter) {

            std::cout << "Stand ship " << shipCounter << " size of " << shipSize << ":\n";

            // Standing ship (repeated until stood)
            while (true) {

                // Data for set of the ship
                int x1, y1;
                int dx = 0, dy = 0;

                // Enter first point coords (repeated until correct)
                while (true) {

                    std::cout << "Enter start coords " + coordsDescription() + ":\n";
                    std::cin >> x1 >> y1;

                    if (coordsIsCorrect(x1, y1)) {

                        if (ownerField[x1][y1] != 0) {

                            std::cout << "Start the ship from the free cell!\n";

                        } else {

                            break;

                        }
                    }
                }

                if (shipSize > 1) {

                    // Data for second point of the ship
                    int x2, y2;

                    // Enter second point coords (repeated until correct)
                    do {

                        std::cout << "Enter finish coords " + coordsDescription() + ":\n";
                        std::cin >> x2 >> y2;

                    } while (!coordsIsCorrect(x2, y2));

                    dx = x2 - x1; dy = y2 - y1;

                }

                // Check replacement of the ship
                int enteredSize = (dx == 0 ? dy : dx);
                enteredSize = (enteredSize < 0 ? -enteredSize : enteredSize) + 1;

                if (dx != 0 && dy != 0) {

                    std::cout << "Place the ship horizontally or vertically!\n";

                } else if (enteredSize != shipSize) {

                    std::cout << "Place the ship size of " << shipSize << "!\n";

                } else {

                    // Stand ship with check of cells
                    bool successfulStanding = true;
                    dx = (dx == 0 ? 0 : (dx < 0 ? -1 : 1));
                    dy = (dy == 0 ? 0 : (dy < 0 ? -1 : 1));
                    for (int i = 0; i < 2; ++i) {
                        // 0 - check; 1 - stand
                        int x = x1, y = y1;
                        for (int lengthCounter = 0; lengthCounter < shipSize; ++lengthCounter) {
                            if (i == 0 && ownerField[x][y] != 0) {
                                successfulStanding = false;
                                std::cout << "Place the ship on the free cells!\n";
                                break;
                            } else if(i == 1) {
                                ownerField[x][y] = 1;
                            }
                            x += dx;
                            y += dy;
                        }
                        if (!successfulStanding) break;
                    }
                    if (successfulStanding) break; // Ship is stood
                }
            }

            // Out refresh field
            outSplitter('-');
            outField(ownerField, opponentField);

        }
    }
}

// Return: true if the player hit
bool playerShot(int ownerField[][10], int opponentField[][10], std::string playerID) {

    std::cout << "Player " << playerID << " shooting...\n";
    outField(ownerField, opponentField);

    // Enter coords (until correct)
    int x1, y1;
    while (true) {

        std::cout << "Enter shot coords " + coordsDescription() + ":\n";
        std::cin >> x1 >> y1;

        if (coordsIsCorrect(x1, y1)) {
            if (opponentField[x1][y1] < 0) {
                std::cout << "You are already shot here!\n";
            } else {
                break;
            }
        }
    }

    // Make the shot
    bool result = opponentField[x1][y1] == 1; // Hit
    opponentField[x1][y1] -= 2;
    std::cout << "You are " << (result ? "hit" : "miss") << "!\n";

    // Out refresh field
    outSplitter('-');

    return result;
}

// Return: true if the player win
bool checkWin(int opponentField[][10]) {

    // Check win
    for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
            if (opponentField[x][y] > 0) return false; // There are opponent ships present
        }
    }

    return true; // All opponent ships sunken

}

int main() {

    // 0 - free cell, 1 - ship, -1 - sunken ship, -2 - shot miss
    int fields[2][10][10];
    for (int z = 0; z < 2; ++z) {
        for (int x = 0; x < 10; ++x) {
            for (int y = 0; y < 10; ++y) {
                fields[z][x][y] = 0;
            }
        }
    }
    std::string player01_ID = "1";
    std::string player02_ID = "2";
/*
    // Test fields
    int fields[2][10][10] = {{{0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
                                     {0, 1, 0, 0, 0, 1, 0, 0, 0, 0},
                                     {1, 0, 0, 0, 1, 1, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
                                     {0, 1, 1, 0, 1, 0, 0, 1, 1, 1},
                                     {0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
                                     {0, 0, 0, 0, 1, 0, 1, 0, 0, 0},
                                     {1, 0, 0, 0, 1, 0, 0, 0, 0, 0}},
                             {{0, 0, 0, 0, 0, 0, 1, 1, 0, 1},
                                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
                                     {0, 0, 1, 0, 0, 1, 1, 1, 0, 0},
                                     {0, 0, 1, 0, 0, 0, 0, 0, 0, 0},
                                     {1, 0, 0, 0, 1, 1, 1, 0, 0, 0},
                                     {0, 0, 0, 0, 0, 0, 0, 0, 1, 0},
                                     {1, 0, 0, 0, 0, 0, 0, 0, 1, 0},
                                     {0, 0, 0, 0, 1, 0, 0, 0, 1, 0},
                                     {0, 0, 1, 0, 0, 0, 0, 0, 1, 0},
                                     {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}};
*/

    outHeader();

    // Stand of the ships
    standingShip(fields[0], fields[1], player01_ID);
    standingShip(fields[1], fields[0], player02_ID);

    // Initialize game
    bool playerHit = true;
    std::string presPlayerID; // ID of present player
    int ownerFieldIndex, opponentFieldIndex;

    // Everyone shooting (until win)
    do {

        // If Hit - repeat shot
        if (!playerHit || presPlayerID.empty()) {
            // Change active player
            if (presPlayerID == player01_ID) {
                presPlayerID = player02_ID;
                ownerFieldIndex = 1;
                opponentFieldIndex = 0;
            } else {
                presPlayerID = player01_ID;
                ownerFieldIndex = 0;
                opponentFieldIndex = 1;
            }
        }

        playerHit = playerShot(
                fields[ownerFieldIndex],
                fields[opponentFieldIndex],
                presPlayerID);

    } while (!checkWin(fields[opponentFieldIndex]));

    // Game over
    std::cout << "Player " << presPlayerID << " win! Congratulation!!!\n";
    outField(fields[ownerFieldIndex], fields[opponentFieldIndex]);
    std::cout << "Game over!\n";

    return 0;

}
