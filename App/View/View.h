#ifndef VIEW_H
#define VIEW_H

#include <string>
#include <vector>
#include "Mediafile.h"
#include <iomanip> 
enum Tab {
    HOME, MUSIC, VIDEO
};

class View {
public:
    std::string convertSecondsToTimeString(int totalSeconds);
    void printLine(char c, int width);
    void clearScreen();
    void displayTabBar();
    void displayMenuBar();
    void displayPlaybackBar();
    void displayMetadata(std::vector<MediaFile>& files);
    void displayProgressBar(std::vector<MediaFile>& files,int duration, int cur_song);
    void displayTitleVolumeBar(std::vector<MediaFile>& files, int cur_song);
    void displayPage(std::vector<MediaFile>& files,uint32_t  duration, int cur_song);
    void displaySelection();
    int getpage();
    void setpage(int page);
    Tab gettab();
    void settab(Tab tab);
    int gettotalpage(std::vector<MediaFile>& files);
private:
    int currentPage = 0;
    int totalPage = 0;
    static constexpr int ITEMS_PER_PAGE = 5;
    Tab currentTab = HOME;
    int currsong = 0;
};

#endif // VIEW_H