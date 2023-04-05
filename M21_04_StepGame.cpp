#include <iostream>
#include <ctime>
#include <vector>
#include <sstream>
#include <fstream>
#include <cassert>

void outSplitter (char sign = '-') {
    std::string splitter;
    splitter.assign(50, sign);
    std::cout << splitter << std::endl;
}

void outHeader () {
    outSplitter('=');
    std::cout << "Adventure. The Game" << std::endl;
    std::cout << "(make console wider)" << std::endl;
    outSplitter('=');
}

// Initialisation
const char* fileName = "save.bin";
int activePlayer = 0;

// Command description
enum userCommands {
    cmd_up, cmd_down, cmd_left, cmd_right,
    cmd_save, cmd_load, cmd_list,
    cmd_exit
};

struct userCommandDescribe {
    const char* name = "";
    const char* describe = "";
};

const int userCommandsCount = 1 + (int) cmd_exit;
const userCommandDescribe userCommandsSet[userCommandsCount] = {
        "w", "Move up",
        "s", "Move down",
        "a", "Move left",
        "d", "Move right",
        "save", "Save game",
        "load", "Load game",
        "list", "List of commands",
        "exit", "Finish game"
};

void outAllCommands() {
    std::cout << "Available commands:\n";
    outSplitter();
    for (int i = 0; i < userCommandsCount; ++i)
        std::cout << "- " << userCommandsSet[i].name
                  << " - " << userCommandsSet[i].describe
                  << std::endl;
}

// Field description
enum fieldState {
    fld_Free, fld_Player, fld_Enemy
};

const char* fieldChars = ".PE";
const int fieldHeight = 20, fieldWidth = 20;
fieldState field[fieldHeight][fieldWidth]; // Row / Column

struct fieldPosDescribe {
    int row = 0; // 0...fieldHeight - 1
    int col = 0; // 0...fieldWidth - 1
};

// Characters description
struct characterDescribe {
    std::string name;
    int lives = 0;
    int armor = 0;
    fieldPosDescribe position;
    bool player = false;
};

// Start NPC tuning
const char* npcFirstName = "Enemy # ";
const int npcStartCount = 5;
const int npcStartLivesMin = 50, npcStartLivesMax = 150;
const int npcStartArmorMin = 0, npcStartArmorMax = 50;
const int npcStartParamsStep = 10;
std::vector<characterDescribe> npcCharacters; // NPCs list
// Start players tuning
const int playersStartCount = 1;
std::vector<characterDescribe> playerCharacters; // Players list
// Damage
const int damageMin = 15, damageMax = 30;
const int damageRandStep = 1;

std::string getUserString() {

    std::string userInput;
    std::getline(std::cin, userInput);

    return userInput;
}

std::stringstream getUserStream() {
    return std::stringstream(getUserString());
}

bool gameOver() {

    return playerCharacters.empty()
        || npcCharacters.empty();

}

int getRandInt(int uBound, int lBound = 0, int stepValue = 1) {

    // Check input values
    if (stepValue < 0) stepValue = -stepValue;
    else if (stepValue == 0) stepValue = 1;
    if (lBound > uBound) std::swap(lBound, uBound);

    // Get random value
    int presValue = (uBound - lBound) / stepValue + 1;
    presValue = lBound + stepValue * (std::rand() % presValue);

    return presValue;

}

void outFieldState(bool finalField = gameOver()) {

    // Out field
    std::cout << (finalField ? "Final" : "Present") << " field:\n";
    outSplitter();
    for (int row = -1; row < fieldHeight; ++row) {

        bool itFieldRow = row >= 0;

        // First column
        if (itFieldRow) {
            std::cout.width(2);
            std::cout << row + 1 << ":  ";
        } else // Header row
            std::cout << "R\\C: ";

        // Field columns
        for (int col = 0; col < fieldWidth; ++col) {

            if (itFieldRow)
                std::cout << ' ' << fieldChars[(int)field[row][col]] << ' ';
            else {
                std::cout.width(2);
                std::cout << col + 1 << ' ';

            }

        }

        // Next row
        std::cout << std::endl;

    }

}

void outPosDescribe(const fieldPosDescribe& pos) {

    std::cout << "(Row/Col): " << 1 + pos.row
        << " / " << 1 + pos.col;

}

void outCharacterDescribe(const characterDescribe& character,
                          bool withPos = false) {

    std::cout << (character.player ? "Player" : "Character")
        << " \"" << character.name << "\"; "
        << "Lives / armor: " << character.lives << " / "
        << character.armor;

    if (withPos) {
        std::cout << std::endl;
        std::cout << "In pos. ";
        outPosDescribe(character.position);
    }

}

void outAllCharacters() {

    for (int setsCount = 0; setsCount < 2; ++setsCount) {

        bool itNPCSet = setsCount == 1;
        if (setsCount > 0) outSplitter('=');

        // Initial data
        const std::vector<characterDescribe> workSet
            = (itNPCSet ? npcCharacters : playerCharacters);
        const characterDescribe* presCharacter = workSet.data();

        // Out set
        auto index = workSet.size();
        while (index-- > 0) {

            outCharacterDescribe(*presCharacter, true);
            std::cout << std::endl;

            presCharacter++;
            if (index > 0) outSplitter();

        }

    }

}

void fieldClearing() {

    int fieldSize = fieldHeight * fieldWidth;
    fieldState* field_Pnt = field[0];

    while (fieldSize-- > 0)
        *field_Pnt++ = fld_Free;

}

bool fieldPosIsFree(const fieldPosDescribe& checkedPos) {

    return field[checkedPos.row][checkedPos.col] == fld_Free;

}

// Check equal field positions
bool possAreEqual(const fieldPosDescribe& pos_1,
                  const fieldPosDescribe& pos_2) {

    return pos_1.row == pos_2.row && pos_1.col == pos_2.col;

}

// Get pointer to character by pos (nullptr if absent)
characterDescribe* getCharacterInPos(const fieldPosDescribe& checkedPos) {

    for (int setCount = 0; setCount < 2; ++setCount) {

        std::vector<characterDescribe>& workSet
            = (setCount == 1 ? npcCharacters : playerCharacters);

        // Check characters set
        auto workCount = workSet.size();
        characterDescribe* workCharacter = workSet.data();
        while (workCount-- > 0)
            if (possAreEqual(workCharacter++->position, checkedPos))
                return --workCharacter;

    }

    return nullptr;

}

void setCharacterInField(const characterDescribe& presCharacter) {

    field[presCharacter.position.row][presCharacter.position.col]
        = (presCharacter.player ? fld_Player : fld_Enemy);

}

void clearCharacterInField(const characterDescribe& presCharacter) {

    field[presCharacter.position.row][presCharacter.position.col]
        = fld_Free;

}

void setCharacterRandomPos(characterDescribe& presCharacter) {

    // Check filling the field
    if (playerCharacters.size() + npcCharacters.size()
            > fieldWidth * fieldHeight)
        return;

    // Get random coords
    fieldPosDescribe& presPos = presCharacter.position;
    do {
        presPos.row = getRandInt(fieldHeight - 1);
        presPos.col = getRandInt(fieldWidth - 1);
    } while (!fieldPosIsFree(presPos));

    // Set to field
    setCharacterInField(presCharacter);

}

void initGameState() {

    // Refresh field
    fieldClearing();

    // Player...s states set
    playerCharacters.resize(playersStartCount);
    for (int i = 0; i < playersStartCount; ++i) {

        std::stringstream userStream;
        characterDescribe& presPlayer = playerCharacters[i];
        presPlayer.player = true;

        std::cout << "Player " << i + 1 << " state...\n";

        // Name cycle
        while (true) {

            std::cout << "Enter player's name (some letters):\n";
            presPlayer.name = getUserString();
            outSplitter();

            if (presPlayer.name.empty())
                std::cout << "Wrong name!\n";
            else break;
            outSplitter();

        }

        // Data cycle
        while (true) {

            // Get data
            std::cout << "Enter (over space)\n";
            std::cout << "(lives[>0] armor[>=0]):\n";
            userStream = getUserStream();
            userStream >> presPlayer.lives >> presPlayer.armor;
            outSplitter();

            // Check data
            if(presPlayer.lives <= 0 || presPlayer.armor < 0)
                std::cout << "Wrong lives or armor!\n";
            else break;
            outSplitter();

        }

        setCharacterRandomPos(presPlayer);

    }

    // NPCs states set
    npcCharacters.resize(npcStartCount);
    for (int i = 0; i < npcStartCount; ++i) {

        characterDescribe& presNPC = npcCharacters[i];

        presNPC.player = false;
        presNPC.name = npcFirstName + std::to_string(i + 1);
        presNPC.lives = getRandInt(npcStartLivesMax,
                                   npcStartLivesMin,
                                   npcStartParamsStep);
        presNPC.armor = getRandInt(npcStartArmorMax,
                                   npcStartArmorMin,
                                   npcStartParamsStep);

        setCharacterRandomPos(presNPC);

    }

}

// Work with file
bool fileWithSaveExist() {

    std::ifstream fileForCheck(fileName);
    bool result = fileForCheck.is_open();
    fileForCheck.close();

    return result;

}

void saveCharactersSet(std::ofstream& fileStream,
                       std::vector<characterDescribe>& charactersSet) {

    if (fileStream.is_open()) {

        // Size of set
        auto setSize = (int) charactersSet.size();
        fileStream.write((char*) &setSize, sizeof(setSize));

        characterDescribe* character_Pnt = charactersSet.data();
        while (setSize-- > 0) {

            // Name
            auto nameLength = (int) character_Pnt->name.length();
            fileStream.write((char*) &nameLength, sizeof(nameLength));
            fileStream.write(character_Pnt->name.c_str(),
                             nameLength);

            // Data
            fileStream.write((char*) &character_Pnt->player,
                             sizeof(character_Pnt->player));
            fileStream.write((char*) &character_Pnt->lives,
                             sizeof(character_Pnt->lives));
            fileStream.write((char*) &character_Pnt->armor,
                             sizeof(character_Pnt->armor));

            // Position
            fileStream.write((char*) &character_Pnt->position.row,
                             sizeof(character_Pnt->position.row));
            fileStream.write((char*) &character_Pnt->position.col,
                             sizeof(character_Pnt->position.col));

            character_Pnt++;

        }

    }

}

void saveGame() {

    std::ofstream fileStream(fileName, std::ios::binary);

    if (fileStream.is_open()) {

        fileStream.write((char*) &activePlayer, sizeof(activePlayer));

        saveCharactersSet(fileStream, playerCharacters);

        saveCharactersSet(fileStream, npcCharacters);

        fileStream.close();

        std::cout << "Game is saved!\n";

    } else std::cout << "File is not opened!\n";

}

void loadCharactersSet(std::ifstream & fileStream,
                       std::vector<characterDescribe>& charactersSet,
                       const int startCount) {

    if (fileStream.is_open()) {

        charactersSet.clear();

        // Size of set
        int setSize;
        fileStream.read((char*) &setSize, sizeof(setSize));

        while (!fileStream.eof() && setSize-- > 0) {

            auto character_Pnt = new characterDescribe;

            // Name
            int nameLength;
            fileStream.read((char*) &nameLength, sizeof(nameLength));
            character_Pnt->name.resize(nameLength);
            fileStream.read((char*)character_Pnt->name.c_str(),
                            nameLength);

            // Data
            fileStream.read((char*) &character_Pnt->player,
                             sizeof(character_Pnt->player));
            fileStream.read((char*) &character_Pnt->lives,
                             sizeof(character_Pnt->lives));
            fileStream.read((char*) &character_Pnt->armor,
                             sizeof(character_Pnt->armor));

            // Position
            fileStream.read((char*) &character_Pnt->position.row,
                             sizeof(character_Pnt->position.row));
            fileStream.read((char*) &character_Pnt->position.col,
                             sizeof(character_Pnt->position.col));

            // Add character to game
            if (character_Pnt->position.row < fieldHeight
                && character_Pnt->position.col < fieldWidth) {
                charactersSet.push_back(*character_Pnt);
                setCharacterInField(*character_Pnt);
            }

        }

        // Fix size of set
        while (charactersSet.size() > startCount) {
            clearCharacterInField(
                    charactersSet[charactersSet.size() - 1]);
            charactersSet.pop_back();
        }

    }

}

void loadGame() {

    std::ifstream fileStream(fileName, std::ios::binary);

    if (fileStream.is_open()) {

        fieldClearing(); // Clear field

        fileStream.read((char*) &activePlayer, sizeof(activePlayer));

        loadCharactersSet(fileStream, playerCharacters,
                          playersStartCount);

        loadCharactersSet(fileStream, npcCharacters,
                          npcStartCount);

        fileStream.close();

        std::cout << "Game is loaded!\n";

    } else std::cout << "File is not opened!\n";

}

// Moving block
bool isFoe(const characterDescribe& character_1,
           const characterDescribe& character_2) {

    return character_1.player != character_2.player;

}

void removeCharacter(characterDescribe& character) {

    for (int setCount = 0; setCount < 2; ++setCount) {

        std::vector<characterDescribe>& workSet =
                (setCount == 1 ? npcCharacters : playerCharacters);
        auto setSize = (int) workSet.size();

        for (int i = 0; i < setSize; ++i)
            if (&character == &workSet[i]) {
                clearCharacterInField(character);
                for (auto j = i; j < setSize - 1; ++j)
                    std::swap(workSet[j], workSet[j + 1]);
                workSet.pop_back();
                return;
            }

    }

}

fieldPosDescribe getNextPosByMove(const fieldPosDescribe& presPos,
                                  userCommands moveCmd) {

    fieldPosDescribe nextPos = {presPos.row, presPos.col};

    if (moveCmd == cmd_up && nextPos.row > 0)
        nextPos.row--;
    else if (moveCmd == cmd_down && nextPos.row < fieldHeight - 1)
        nextPos.row++;
    else if (moveCmd == cmd_left && nextPos.col > 0)
        nextPos.col--;
    else if (moveCmd == cmd_right && nextPos.col < fieldWidth - 1)
        nextPos.col++;

    return nextPos;

}

// With check next enemy
userCommands getRandomMoveCommand(const characterDescribe& mover) {

    const int cmdCount = 4;
    userCommands moveCommands[cmdCount] = {
            cmd_up, cmd_right, cmd_down, cmd_left};

    // Check move to next enemy
    for (int cmdIndex = 0; cmdIndex < cmdCount; ++cmdIndex) {

        fieldPosDescribe nextPos = getNextPosByMove(
                mover.position,
                moveCommands[cmdIndex]);
        const auto nextCharacter
            = getCharacterInPos(nextPos);

        if (nextCharacter != nullptr
                && isFoe(mover, *nextCharacter))
            return moveCommands[cmdIndex];

    }

    // Random move
    return moveCommands[getRandInt(cmdCount - 1)];

}

void makeMove(characterDescribe& mover,
              userCommands direction) {

    fieldPosDescribe nextPos = getNextPosByMove
            (mover.position,direction);
    fieldPosDescribe& pos = mover.position;

    outCharacterDescribe(mover);
    std::cout << std::endl;
    bool makeMove = false;
    if (!possAreEqual(pos, nextPos)) {

        // Check next cell character conflict
        characterDescribe* barrier = getCharacterInPos(nextPos);
        if (barrier != nullptr) {

            if (isFoe(mover, *barrier)) {
                // Conflict

                // Get damage
                int damage = getRandInt(damageMax, damageMin,
                                        damageRandStep);
                std::cout << "Apply damage " << damage << " to \""
                          << barrier->name << "\"\n";

                // Apply damage
                barrier->armor -= damage;
                if (barrier->armor < 0) {
                    barrier->lives += barrier->armor;
                    barrier->armor = 0;
                }
                std::cout << "-> Target";

                // Describe damage
                if (barrier->lives <= 0) {
                    std::cout << " was been OFF!\n";
                    removeCharacter(*barrier);
                    makeMove = true;
                } else {
                    std::cout << " remains of lives: " << barrier->lives
                        << ", armor: " << barrier->armor << std::endl;
                }

            }

        } else makeMove = true;

    }

    // Make move if need
    if (makeMove) {

        // Out description
        std::cout << "Moves from ";
        outPosDescribe(pos);
        std::cout << " to ";

        // Registering move
        clearCharacterInField(mover);
        pos.row = nextPos.row;
        pos.col = nextPos.col;
        setCharacterInField(mover);

    } else std::cout << "Stay at ";
    outPosDescribe(pos);
    std::cout << std::endl;

}

int main() {

    // Initialization
    std::srand(std::time(nullptr)); // Randomizer

    outHeader();

    // Check ready for load game
    bool loadGameChosen = false;
    if (fileWithSaveExist()) {
        std::cout << "Continue the saved game (yes - if you would)?..\n";
        loadGameChosen = getUserString() == "yes";
    }
    if (loadGameChosen) {
        loadGame();
        outSplitter('=');
    }
    else initGameState();

    // Game cycle (interface module)
    bool showField = true;
    bool showCharacters = true;
    while (!gameOver()) {

        // Correct active player
        //activePlayer = -1; // Only characters move
        if (activePlayer >= playerCharacters.size())
            activePlayer = -1; // NPCs step

        // Out field
        if (showCharacters) {
            outAllCharacters();
            outSplitter('=');
            showCharacters = false;
        }
        if (showField) {
            outFieldState();
            outSplitter('=');
            showField = false;
        }

        if (activePlayer >= 0) {
            // Step of next player

            std::string userInput;
            std::stringstream userStream;

            // Get command
            userCommandDescribe listCmdInfo
                = userCommandsSet[(int) cmd_list];
            outCharacterDescribe(
                    playerCharacters[activePlayer],
                    true);
            std::cout << std::endl;
            std::cout << "Enter command (" << listCmdInfo.name
                << " - " << listCmdInfo.describe << "):\n";
            userInput = getUserString();
            outSplitter();

            // Find command
            int cmdIndex = 0;
            while (userCommandsSet[cmdIndex].name != userInput
                && ++cmdIndex < userCommandsCount);

            if (cmdIndex < userCommandsCount) {
                // Processing commands
                auto cmdCurrent = (userCommands) cmdIndex;
                if (cmdCurrent == cmd_exit) break;
                else if (cmdCurrent == cmd_save)
                    saveGame();
                else if (cmdCurrent == cmd_load) {
                    loadGame();
                    //outGameState();
                    showField = true;
                    showCharacters = true;
                } else if (cmdCurrent == cmd_list)
                    outAllCommands();
                else if (cmdCurrent == cmd_up
                        || cmdCurrent == cmd_down
                        || cmdCurrent == cmd_left
                        || cmdCurrent == cmd_right) {
                    makeMove(playerCharacters[activePlayer],
                             cmdCurrent);
                    activePlayer++; // Next player step
                    showField = activePlayer < playerCharacters.size();
                } else std::cout << "Not applicable command!\n";
            } else {
                std::cout << "Wrong command!\n";
                outSplitter();
                outAllCommands();
            }

        } else {

            // Step of NPCs
            std::cout << "Characters moving...\n";
            outSplitter('=');
            characterDescribe* npcCharacter_Pnt = &npcCharacters[0];
            characterDescribe* npcCharacter_End
                = &npcCharacters[npcCharacters.size() - 1];
            while (npcCharacter_Pnt <= npcCharacter_End) {
                makeMove(*npcCharacter_Pnt,
                         getRandomMoveCommand(*npcCharacter_Pnt));
                if (npcCharacter_Pnt != npcCharacter_End)
                    outSplitter();
                if (gameOver()) break;
                npcCharacter_Pnt++;
            }

            // Finish step of characters
            activePlayer++; // Next player step
            showField = true; // Show changed field

        }
        outSplitter('=');

    }

    outFieldState(true);
    outSplitter('=');
    if (npcCharacters.empty())
        std::cout << "Player" << (playersStartCount > 1 ? "s" : "")
            << " win! Congratulations!!!\n";
    else if (playerCharacters.empty())
        std::cout << "Player" << (playersStartCount > 1 ? "s" : "")
        << " loose! Oh, sorry!!!\n";
    else std::cout << "Game finished by player\n";

    return 0;

}
