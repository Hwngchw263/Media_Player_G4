#ifndef VIEW_H
#define VIEW_H

#include <string>
#include <vector>
#include "Mediafile.h"
#include <iomanip> 
enum Tab {
    HOME, MUSIC, VIDEO
};
enum Flag {
    USB_MODE, USB_PATH, CIN_PATH, ACT_MODE, NUM_MODE
};
class View {
public:
    std::string convertSecondsToTimeString(int totalSeconds);
    void printLine(char c, int width);
    void clearScreen();
    void displayUSB(int num);
    void displayTabBar(int num);
    void displayMenuBar(int num);
    void displayPlaybackBar(int num);
    void displayMetadata(int song_num);
    void displayProgressBar(std::vector<MediaFile>& files,int duration, int cur_song);
    void displayTitleVolumeBar(std::vector<MediaFile>& files, int cur_song, int volume_value);
    void displayPage(std::vector<MediaFile>& executing_files, int duration, int cur_song, int volume_value, int num_mode, int song_num);
    void displaySelection();
    int getpage();
    void setpage(int page);
    Tab gettab();
    void settab(Tab tab);
    void setdisplayfilelist(std::vector<MediaFile>& files);
    int gettotalpage(std::vector<MediaFile>& files);
private:
    const std::string bold_on = "\033[1m";
    const std::string bold_off = "\033[0m";
    int currentPage = 0;
    int totalPage = 0;
    static constexpr int ITEMS_PER_PAGE = 5;
    Tab currentTab;
    std::vector<MediaFile> displayfilelist;
    int currsong = 0;
    std::vector<std::string> USB_commands {"[1] Play from USB","[2] Play from Folder","[3] Exit"};
    std::vector<std::string> TabBar_commands{"[9] Back","[10] Home","[11] Music library","[12] Video library","[13] Playlists","[14] Quit" };
    std::vector<std::string> MenuBar_commands{ "[15] Remove","[16] Add to playlist","[17] Edit","[18] Previous page","[19] Next page"};
    std::vector<std::string> PlaybackBar_commands{"[1] Play","[2] Stop","[3] Pause","[4] Resume","[5] Next","[6] Previous","[7] Repeat one","[8] Repeat all"};
};

#endif // VIEW_H