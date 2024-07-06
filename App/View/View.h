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
    void displayPath(std::vector<std::string>& files);
    void displayMetadata(std::vector<MediaFile>& files);
    void displayHelp();
    void displayPage(std::vector<std::string>& files);
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
};

#endif // VIEW_H
