#include <iostream>
#include <ctime>
#include <iomanip>

// Interface class
class Interface {

public:

    static const int sizeOfSplitter = 30;

    static void outSplitter (char sign = '-') {

        std::string splitter;
        splitter.assign(sizeOfSplitter, sign);
        std::cout << splitter << std::endl;

    }

    static void outHeader(const char* headerText) {

        outSplitter('=');
        std::cout << headerText << std::endl;
        outSplitter('=');

    }

    // Stream block
    static void refreshInStream() {
        std::cin.clear();
        std::cin.seekg(0);
    }

};

// User commands class
class UserCmd {

public:

    enum CmdList{
        non = -1, play, pause, next, stop, list, exit
    };

private:

    struct userCmdWrite{
        const char* name;
        const char* description;
    };

    static constexpr userCmdWrite userCmdList[(int) exit + 1] {
            "play", "Play selected track",
            "pause", "Pause playback",
            "next", "Next random track",
            "stop", "Stop playback",
            "list", "List of tracks",
            "exit", "Turn off"
    };

public:

    static void outUserCmdList() {

        std::cout << "Available commands:\n";
        const userCmdWrite* cmd_Pnt = userCmdList - 1;
        while (++cmd_Pnt < std::end(userCmdList))
            std::cout << " - \"" << cmd_Pnt->name << "\" - "
                      << cmd_Pnt->description << std::endl;

    }

    static CmdList getCmdByName(const std::string& cmdName) {

        const userCmdWrite* cmd_Pnt = userCmdList - 1;
        while (++cmd_Pnt < std::end(userCmdList))
            if (cmd_Pnt->name == cmdName) return (CmdList) (cmd_Pnt - userCmdList);

        return UserCmd::non;

    }

};

// Player class
class Player {

    static std::tm getNullTime() {
        const std::time_t nullTime = 0;
        return *gmtime(&nullTime);
    }

    // Track class
    struct Track {
        std::string name;
        std::tm createDate = getNullTime();
        int duration_Sec = 0;
    };

    // Tracks of player
    static const int tracksCount = 10;
    Track tracks[tracksCount];
    Track* shuffle[tracksCount] = {nullptr};

    Track* playbackTrack = nullptr;
    Track** shuffleTrack = nullptr;
    bool isPause = false;

    static int getRandomValue(int minVal, int maxVal) {
        return minVal + std::rand() % (maxVal - minVal + 1);
    }

    // Out tracks information
    static std::string getMinSecFmt(int minSecVal) {

        char timeFmt[3];
        sprintf(timeFmt, "%2d", minSecVal);
        if (timeFmt[0] == ' ') timeFmt[0] = '0';

        return timeFmt;


    }

    static std::string getDurForOut(int durTotal_Sec) {

        int timeDiv = 60;
        std::string result = ':' + getMinSecFmt(durTotal_Sec % timeDiv); // Seconds

        durTotal_Sec /= timeDiv;
        // Minutes
        result = getMinSecFmt(durTotal_Sec % timeDiv) + result; // Minutes
        if (durTotal_Sec > timeDiv)
            result = std::to_string(durTotal_Sec / timeDiv) + ':' + result;

        return result;

    }

    static void outTrackInfo(const Track* track) {

        if (track == nullptr) return;

        std::cout << "- \"" << track->name << "\" ("
                  << getDurForOut(track->duration_Sec) << ")\n";
        std::cout << "   created: " << std::put_time(&track->createDate, "%c") << std::endl;

    }

    void startPlayTrack(Track* selTrack) {

        if (selTrack == nullptr) return;

        // Start playback
        if (playbackTrack != selTrack) {
            playbackTrack = selTrack;
            std::cout << "Start playback:\n";
        } else std::cout << "Continue playback:\n";
        outTrackInfo(playbackTrack);
        Interface::outSplitter();
        isPause = false;

        // Sync shuffle for selected playback
        if (shuffleTrack == nullptr
                || *shuffleTrack != playbackTrack) {
            shuffleTrack = shuffle - 1;
            while (++shuffleTrack < std::end(shuffle) && *shuffleTrack != playbackTrack);
            if (shuffleTrack == std::end(shuffle)) shuffleTrack = nullptr;
        }

    }

public:

    // Fill tracks random values
    void formTracks() {

        int rndTimeRange = 8 * 60 * 60; // Int value
        std::time_t presTime = time(nullptr);

        // Fill tracks
        int trackNum = 1;
        Track* tracks_Pnt = tracks - 1;
        while (++tracks_Pnt < std::end(tracks)) {

            tracks_Pnt->name = "Track " + std::to_string(trackNum++);
            std::time_t rndTime = presTime - getRandomValue(0, rndTimeRange);
            tracks_Pnt->createDate = *localtime(&rndTime);
            tracks_Pnt->duration_Sec = (int) getRandomValue(30,  7200);

            int shuffleIdx;
            do {
                shuffleIdx = getRandomValue(0, tracksCount - 1);
                if (shuffle[shuffleIdx] == nullptr) shuffle[shuffleIdx] = &*tracks_Pnt;
            } while(shuffle[shuffleIdx] != &*tracks_Pnt);

        }

    }

    // Player commands
    void list() {

        // Shuffle list
        std::cout << "Shuffle list:\n";
        Track** shuffle_Pnt = shuffle;
        while (shuffle_Pnt < std::end(shuffle))
            outTrackInfo(*shuffle_Pnt++);
        Interface::outSplitter();

        // Track list
        std::cout << "Tracks (total: " << tracksCount << "):\n";
        Track* tracks_Pnt = tracks;
        while (tracks_Pnt < std::end(tracks))
            outTrackInfo(tracks_Pnt++);
        Interface::outSplitter();

    }

    void play() {

        if (playbackTrack == nullptr) {

            // Get track name
            std::string trackName;
            std::cout << "Enter name of track:\n";
            std::getline(std::cin, trackName);
            Interface::outSplitter();
            if (trackName.empty()) return;

            // Find track_Pnt
            Track* tracks_Pnt = std::begin(tracks) - 1;
            while (++tracks_Pnt < std::end(tracks) && tracks_Pnt->name != trackName);

            // Check and start playback
            if (tracks_Pnt == std::end(tracks)) {
                std::cout << "Track with name \"" << trackName << "\" is not found!\n";
                Interface::outSplitter();
                return;
            } else startPlayTrack(tracks_Pnt); // Start selected playback

        } else if (isPause) startPlayTrack(playbackTrack); // Continue playback
        else return;

    }

    void pause() {
        if (playbackTrack != nullptr && !isPause) {
            isPause = true;
            std::cout << "Pause:\n";
            outTrackInfo(playbackTrack);
            Interface::outSplitter();
        }
    }

    void next() {

        // Find next track
        if (shuffleTrack == nullptr) shuffleTrack = shuffle;
        else {
            ++shuffleTrack;
            if (shuffleTrack >= std::end(shuffle)) shuffleTrack = shuffle;
        }

        // Check next track
        if (shuffleTrack < std::end(shuffle))
            startPlayTrack(*shuffleTrack);
        else shuffleTrack = nullptr;

    }

    void stop() {
        if (playbackTrack != nullptr) {
            std::cout << "Stop playback:\n";
            outTrackInfo(playbackTrack);
            Interface::outSplitter();
            playbackTrack = nullptr;
        }
    }

};

int main() {

    Interface::outHeader("Player");

    srand(time(nullptr));
    Player* player = new Player;
    player->formTracks();
    player->list();

    while (true) {

        // Out command list
        UserCmd::outUserCmdList();
        Interface::outSplitter();

        // Get command
        std::string userInput;
        std::cout << "Enter command:\n";
        std::getline(std::cin, userInput);
        UserCmd::CmdList userCmd = UserCmd::getCmdByName(userInput);
        Interface::outSplitter();

        // Select command
        if (userCmd == UserCmd::exit) break;
        else if (userCmd == UserCmd::play) player->play();
        else if (userCmd == UserCmd::pause) player->pause();
        else if (userCmd == UserCmd::next) player->next();
        else if (userCmd == UserCmd::stop) player->stop();
        else if (userCmd == UserCmd::list) player->list();
        else {
            std::cout << "Wrong command!\n";
            Interface::outSplitter();
        }

    }

    delete player;

    std::cout << "Turned off\n";

    return 0;

}
