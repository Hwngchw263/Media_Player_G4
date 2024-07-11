#include "View.h"
#include <iostream>
#include <cstdlib>
std::string View::convertSecondsToTimeString(int totalSeconds) {
    int hours = totalSeconds / 3600;    
    int minutes = (totalSeconds % 3600) / 60;  
    int seconds = totalSeconds % 60;    

    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hours << ":"
        << std::setw(2) << std::setfill('0') << minutes << ":"
        << std::setw(2) << std::setfill('0') << seconds;

    return oss.str();
}
void View::printLine(char c, int width){
    std::string line(width, c);
    std::cout << line << std::endl;
}

//Clear screen
void View::clearScreen() {
    std::cout << "\033[2J\033[1;1H"; // ANSI escape code to clear the screen and move the cursor to the top-left corner
} 
int View::gettotalpage(std::vector<MediaFile>& files){
    this->totalPage = 1 + (files.size() - 1)/ITEMS_PER_PAGE;
    return totalPage;
} 

//Function to display tab bar
void View::displayTabBar(){
   //Tab
    printLine('=',200);
    std::cout << std::left
    << std::setw(1) << "|"
    << std::setw(33) << "Back[b]" 
    << std::setw(33) << "Home[f]"
    << std::setw(33) << "Music library[R]"
    << std::setw(33) << "Video library[d]"
    << std::setw(33) << "Playlists[y]" 
    << std::setw(33) << "Quit[q]" 
    << std::setw(1) << "|"
    << std::endl;
    printLine('-',200);
}
//Function to display menu bar
void View::displayMenuBar(){
    //Menu
    std::cout << std::left << std::setw(199) << "| Remove [t]" << "|" << std::endl;
    std::cout << std::setw(199) << "| Add to playlist[a]" << "|" << std::endl;
    std::cout << std::setw(199) << "| Edit [e]" << "|" << std::endl; 
    std::cout << std::setw(199) << "| Previous page[p]" << "|" << std::endl;
    std::cout << std::setw(199) << "| Next page[n]" << "|" << std::endl;
    printLine('-',200);
}
void View::displayPlaybackBar(){
    //Playback
    std::cout << std::left
    << std::setw(5) << "|"
    << std::setw(21) << "Previous[6]"
    << std::setw(21) << "Play[1]" 
    << std::setw(21) << "Pause[2]"
    << std::setw(21) << "Resume[3]"  
    << std::setw(21) << "Stop[4]"
    << std::setw(21) << "Next[5]"
    << std::setw(21) << "Repeat all[7]"
    << std::setw(21) << "Repeat one[8]"
    << std::setw(21) << "Volume[+][-]"
    << std::right 
    << std::setw(6) << "|"
    << std::endl;
    printLine('-',200);
}
//Function to display progress bar
void View::displayProgressBar(std::vector<MediaFile>& files,int duration, int cur_song){
    //Progress bar
    int numchar = 170*duration/(files[cur_song].getDuration());
    int notrun  = 170 - numchar;
    std::string runline(numchar, '#');
    std::string line((170-numchar), '-');
    std::cout << std::left
    << std::setw(2) << "|"
    << std::setw(12) << convertSecondsToTimeString(duration)
    << "<"
    << std::setw(numchar) << runline
    << std::setw(notrun) << line
    << ">"
    << std::right 
    << std::setw(12) << convertSecondsToTimeString(files[cur_song].getDuration() - duration)
    << std::setw(2) << "|"
    << std::endl;
}
//Function to display title and volume bar
void View::displayTitleVolumeBar(std::vector<MediaFile>& files, int cur_song){
    //Title, volume
    std::cout << std::left
    << std::setw(6) << "|"
    << std::setw(188) << files[cur_song].getTitle()
    << std::right 
    << std::setw(6) << "|"
    << std::endl;
    printLine('=',200);
}
//Function to display metadata
void View::displayMetadata(std::vector<MediaFile>& files) {
    if(files.size() != 0){
    totalPage = 1 + (files.size() - 1)/ITEMS_PER_PAGE;
    int start = currentPage * ITEMS_PER_PAGE;
    int end = std::min((currentPage+1) * ITEMS_PER_PAGE, static_cast<int>(files.size()));
        if(currentTab == HOME){
        //Media
            std::cout << std::left
            << std::setw(6)  <<"| Num " 
            << std::setw(153) << "| Name" 
            << std::setw(20) << "| Type" 
            << std::setw(20) << "| Size" 
            << std::right 
            << std::setw(1) << "|"
            << std::endl;
            printLine('-',200);
            for (int i = start; i < end; ++i) {
                //50 characters title
                std::string title = files[i].getTitle();
                if (title.length() > 50) {
                    title = title.substr(0, 50);
                }
                std::cout << std::left
                << "| " << std::setw(4) << (i+1)  
                << "| " << std::setw(151) << files[i].getName() 
                << "| " << std::setw(18) << files[i].getType()
                << "| " << std::setw(18) << files[i].getSize() 
                << std::right
                <<std::setw(1) << "|"
                << std::endl;
                printLine('-',200);  
            }
        }
        else if(currentTab == VIDEO){
            //Video
            std::cout << std::left
            << std::setw(6)  <<"| Num " 
            << std::setw(133) << "| Name" 
            << std::setw(20) << "| Size" 
            << std::setw(20) << "| Bitrate" 
            << std::setw(20) << "| Duration" 
            << std::right 
            << std::setw(1) << "|"
            << std::endl;
            printLine('-',200); 
            for (int i = start; i < end; ++i) {
                //50 characters title
                std::string title = files[i].getTitle();
                if (title.length() > 50) {
                    title = title.substr(0, 50);
                }
                std::cout << std::left
                << "| " << std::setw(4) << (i+1)  
                << "| " << std::setw(131) << files[i].getName() 
                << "| " << std::setw(18) << files[i].getSize()
                << "| " << std::setw(18) << files[i].getBitrate() 
                << "| " << std::setw(18) << convertSecondsToTimeString(files[i].getDuration()) 
                << std::right
                <<std::setw(1) << "|"
                << std::endl;
                printLine('-',200);  
            } 
        }
        else if(currentTab == MUSIC){
            //Music
            std::cout << std::left
            << std::setw(6)  <<"| Num " 
            << std::setw(53) << "| Title" 
            << std::setw(40) << "| Artist" 
            << std::setw(30) << "| Album" 
            << std::setw(30) << "| Genre" 
            << std::setw(20) << "| Year" 
            << std::setw(20) << "| Duration" 
            << std::right 
            << std::setw(1) << "|"
            << std::endl;
            printLine('-',200);
            for (int i = start; i < end; ++i) {
                //50 characters title
                std::string title = files[i].getTitle();
                if (title.length() > 50) {
                    title = title.substr(0, 50);
                }
                std::cout << std::left
                << "| " << std::setw(4) << (i+1) 
                << "| " << std::setw(51) << title  
                << "| " << std::setw(38) << files[i].getArtist() 
                << "| " << std::setw(28) << files[i].getAlbum()
                << "| " << std::setw(28) << files[i].getGenre() 
                << "| " << std::setw(18) << files[i].getYear()
                << "| " << std::setw(18) << convertSecondsToTimeString(files[i].getDuration()) 
                << std::right
                <<std::setw(1) << "|"
                << std::endl;
                printLine('-',200);  
            }
        }
        std::cout << std::left
        << std::setw(90) << "|"
        << std::setw(5) <<"Page "
        << std::right 
        << std::setw(7) << (currentPage + 1)  
        << std::setw(1) << "/" 
        << std::setw(7) << totalPage  
        << std::setw(90) << "|" 
        << std::endl;
        printLine('-',200);
    }
    else {
        std::cout << std::left
        << std::setw(96) << "|"
        << std::setw(8) << "No file." 
        << std::right
        << std::setw(96) << "|"
        << std::endl;
        printLine('-',200);
    }
}

//Function to display full page
void View::displayPage(std::vector<MediaFile>& files,uint32_t  duration, int cur_song){
    //Clear page
    clearScreen();
    displayTabBar();
    displayMetadata(files);
    displayMenuBar();
    displayPlaybackBar();
    displayProgressBar(files, duration, cur_song);
    displayTitleVolumeBar(files, cur_song);
}

int View::getpage(){
    return currentPage;
}
void View::setpage(int page){
    this->currentPage = page;
}
Tab View::gettab(){
    return currentTab;
}
void View::settab(Tab tab){
    this->currentTab = tab;
}