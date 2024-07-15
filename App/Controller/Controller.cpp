#include "Controller.h"
#include <iostream>
#include <sstream>
#include <algorithm>

Player *Controller::playerptr = nullptr;
Controller::Controller() : sp(SerialPort::getOpenSDADevicePath().c_str())
{
    Mix_VolumeMusic(volume);
    // Configure for serial port
    if (!sp.configure())
    {
        std::cerr << "Failed to configure serial port" << std::endl;
        std::exit(1); // Exit if configuration fails
    }
}

Controller::~Controller()
{
}
//------------------------------Hook for music------------------------------------
void Controller::MusicFinishedCallbackWrapper()
{
    playerptr->FunctionCallback();
}
bool Controller::isValidNumber(const std::string& str) {
    // Check if the string is empty
    if (str.empty()) return false;

    // Check if the string contains only digits
    return std::all_of(str.begin(), str.end(), ::isdigit);
}
void Controller::run()
{
    /*Send data to reset in MCU*/
    std::string reset_message;
    // create message
    mcu_data.createMessage('R', 0);
    // convert message to string
    mcu_data.messageToString(mcu_data.getmess(), reset_message);
    // Send string to MCU
    sp.sendData(reset_message);
    //Get direction from USB
    usbpaths = usb_data.getUSBPaths();
    total_path = usb_data.getUSBPaths().size();
    std::thread display_thread(&Controller::display, this);
    std::thread task_thread(&Controller::executeTask, this);
    std::thread serial_input_thread(&Controller::getInputFromSerial, this);
    std::thread cin_input_thread(&Controller::getInputFromCin, this);

    display_thread.join();
    serial_input_thread.join();
    cin_input_thread.join();
    task_thread.join();
    condition.notify_one();
}
void Controller::display(){
    while(true){
        switch(current_flag){
            case USB_MODE:
            {
                view.displayUSB(current_usb_mode);
                std::cout << "Enter USB mode: " << std::endl;
                break;
            }
            case USB_PATH:
            {
                view.displayUSB(current_usb_path);
                //Display current usb path
                std::cout << "USB direction: \n";
                for (int i = 0; i < total_path; i++) {
                    std::cout << (i + 1) << "." << usbpaths[i] << std::endl;
                }
                std::cout << "Select direction: \n"; 
                if(current_usb_path != 0){
                std::cout << current_usb_path << "." << usbpaths[current_usb_path - 1] << std::endl;
                }
                break;
            }
            case ACT_MODE:
            {
                view.displayPage(executing_lisfile,player.getduration(), player.getcurrenttrack(), volume, current_act_mode, current_song);
                std::cout << "Enter number mode: " << std::endl;
                break;    
            }
            case NUM_MODE:
            {
                //Auto next page(when select number of song)
                //Set curretn page
                int new_currentPage = current_song / 5;
                if (current_song % 5 == 0 && current_song != 0) {
                    new_currentPage -= 1;
                } 
                view.setpage(new_currentPage + 1);
                view.displayPage(executing_lisfile,player.getduration(), player.getcurrenttrack(), volume, current_act_mode, current_song);
                std::cout << "Enter number song: " <<  std::endl; 
                break;   
            }
            case CIN_PATH:
            {
                view.displayUSB(current_usb_mode);
                std::cout << "Enter directory: \n";
                break;
            }
            default:
            {
                break;
            }
        }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}
void Controller::getInputFromSerial()
{
    while (true)
    {
    
        std::string message = sp.receiveData();
        //Check message is empty or not enough length
        if ((!message.empty())&&(message.size() == 8))
        {
            bool messageValid = false;
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                mcu_data.ParseMessage(message);
                messageValid = mcu_data.VerifyMessage(mcu_data.getmess(), message);
            }
            if (messageValid)
            {
                processMessage(message);
            }
            else
            {
                std::cout << "Message failed. Send again" << std::endl;
                // Handle message resend request here
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void Controller::getInputFromCin()
{
    while (true)
    {
        std::string input;
        std::getline(std::cin, input);
        if(current_flag != CIN_PATH){
        //Use for enter a number string
            //Check input string is valid(all character is '0' to '9')
            if (isValidNumber(input))
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                taskQueue.push(input);
                condition.notify_one();
            }
        }
        //using when enter a path
        else{
            std::lock_guard<std::mutex> lock(queueMutex);
            taskQueue.push(input);
            condition.notify_one();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void Controller::processMessage(const std::string &message)
{
    std::string temp = message;
    if (mcu_data.VerifyMessage(mcu_data.getmess(), temp))
    {
        // std::cout << "Message true.\n" << std::endl;
        // std::cout << "Type: " << mcu_data.getmess().type << std::endl;
        // std::cout << "Data: " << mcu_data.getmess().data << std::endl;
        // std::cout << "Checksum: " << mcu_data.getmess().checksum << std::endl;

        if (mcu_data.getmess().type == 'A')
        {
            handleSelection();
        }
        else if (mcu_data.getmess().type == 'S')
        {
            // std::cout << "Select" << std::endl;
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                taskQueue.push("S");
                // std::cout << taskQueue.front() << std::endl;
            }
            condition.notify_one();
        }
        else if (mcu_data.getmess().type == 'V')
        {
            volume = mcu_data.getmess().data;
            Mix_VolumeMusic(volume);
        }
    }
    else
    {
        std::cout << "Message failed. Send again" << std::endl;
    }
}

void Controller::handleSelection()
{
    switch(current_flag){
        case USB_MODE:
        {
            //Calculate current usb mode
            current_usb_mode = mcu_data.PareNum(current_usb_mode, USB_TOTAL_MODE);
            break;
        }
        case USB_PATH:
        {
            //Calculate current usb path
            current_usb_path = mcu_data.PareNum(current_usb_path, total_path);
            break;
        }
        case ACT_MODE:
        {
            //Calculate current action mode
            current_act_mode = mcu_data.PareNum(current_act_mode, TOTAL_MODE);
            break;    
        }
        case NUM_MODE:
        {
            //Calculate current num song
            int total_song = parseTabtofiles().size();
            current_song = mcu_data.PareNum(current_song, total_song);
            break;   
        }
        default:
        {
            break;
        }

    } 
}

std::string Controller::getTaskFromQueue()
{
    std::unique_lock<std::mutex> lock(queueMutex);
    condition.wait(lock, [&]{ return !taskQueue.empty(); });
    if (!taskQueue.empty())
    {
        std::string task = taskQueue.front();
        taskQueue.pop();
        return task;
    }
    return "";
}

void Controller::executeTask()
{
    while (true)
    {
        std::string task = getTaskFromQueue();
        if (!task.empty())
        {
                handleTask(task);
        }
    }
}

void Controller::handleTask(const std::string &task)
{
    //If press button, run selected usb mode
    if (task == "S")
    {
        if(current_flag == USB_MODE){
            handleUSBInput(current_usb_mode);
        }
        else if(current_flag == ACT_MODE){
            handleActInput(current_act_mode);
        }
        
    }
    else
    {
        //set current usb mode
        if(current_flag == USB_MODE){
            //set current usb mode
            current_usb_mode = std::stoi(task);
            handleUSBInput(current_usb_mode);
        }
        else if(current_flag == ACT_MODE){
            //set current action mode
            current_act_mode = std::stoi(task);
            handleActInput(current_act_mode);
        }
    }
}

void Controller::handleUSBInput(int input){
    switch (input) {
            case 1:
                handleUSB();
                break;
            case 2:
                handleFolder();
                break;
            case 3:
                handleExitUSBmenu();
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
        }
}

void Controller::handleExitUSBmenu(){

    std::cout << "Exiting...\n";
    exit(0);
}
void Controller::handleUSB(){
    if(total_path == 0){
        std::cout << "No USB direction." << std::endl;
        current_flag = USB_MODE;
    }
    else{      
        current_flag = USB_PATH;
        std::string task = getTaskFromQueue();
        // If task == "S" enter selection (run with input from MCU)
        if (task == "S")
        {
            //if direction is valid
            if (model.setDirectory(usbpaths[current_usb_path - 1]))
            { 
                current_Dir = usbpaths[current_usb_path - 1];
                handleSetDirectory(current_Dir); 
                current_flag = ACT_MODE;
            }
            //if direction is not valid
            else{
            std::cout << "Direction is not valid." << std::endl;
            current_flag = USB_MODE;  
            }
        }
        //Run with input from Cin
        else{
            int num = std::stoi(task);
            if((num > total_path)||(num < 1)){
                std::cout << "Direction is not exist." << std::endl;
                current_flag = USB_MODE;
            }
            else{
                //if direction is valid
                if (model.setDirectory(usbpaths[num - 1]))
                { 
                    current_Dir = usbpaths[num - 1];
                    handleSetDirectory(current_Dir); 
                    current_flag = ACT_MODE;
                }
                //if direction is not valid
                else{
                std::cout << "Direction is not valid." << std::endl;
                current_flag = USB_MODE;  
                }
            }

        }
    }
}
void Controller::handleFolder(){
    current_flag = CIN_PATH;
    std::string task = getTaskFromQueue();
    //if direction is valid
    if (model.setDirectory(task))
    { 
        current_Dir = task;
        handleSetDirectory(current_Dir); 
        current_flag = ACT_MODE;
    }
    //if direction is not valid
    else{
    std::cout << "Direction is not valid." << std::endl;   
    current_flag = USB_MODE;  
    }
}



void Controller::handleActInput(int input)
{
    playerptr = &player;
    Mix_HookMusicFinished(MusicFinishedCallbackWrapper);
    switch (input)
    {
    case 1:
        handlePlay();
        break;
    case 2:
        handleStop();
        
        break;
    case 3:
        handlePause();  
        break;
    case 4:
        handleResume();
        break;
    case 5:
        handleNext();
        break;
    case 6:
        handlePrevious();
        break;
    case 7:
        player.RepeatOneSong();
        break;
    case 8:
        player.RepeatAllSong();
        break;
    case 9:
        handleBack();
        break;
    case 10:
        handleSwitchTab(HOME);
        break;
    case 11:
        handleSwitchTab(MUSIC);
        break;
    case 12:
        handleSwitchTab(VIDEO);
        break;
    case 13:
        //handleRemoveFile();
        break;
    case 14:
        handleExit();
        break;
    case 15:
        handleRemoveFile();
        break;
    case 16:
        //handleRemoveFile();
        break;
    case 17:
        handleEditMetadata();
        break;
    case 18:
        handlePrevPage();
        break;
    case 19:
        handleNextPage();
        break;
    default:
        std::cout << "Unknown command." << std::endl;
        break;
    }
}
std::vector<MediaFile> &Controller::parseTabtofiles()
{
    if (view.gettab() == HOME)
    {
        return model.getMetadataofmedia();
    }
    if (view.gettab() == MUSIC)
    {
        return model.getMetadataofmusic();
    }
    if (view.gettab() == VIDEO)
    {
        return model.getMetadataofvideo();
    }
    // Thêm một giá trị trả về mặc định hoặc xử lý lỗi
    throw std::runtime_error("Invalid tab selected");
}
std::vector<std::string> &Controller::parseTabtofilepaths()  
{
    if (view.gettab() == HOME)
    {
        return model.getmediafilepath();
    }
    if (view.gettab() == MUSIC)
    {
        return model.getmusicfilepath();
    }
    if (view.gettab() == VIDEO)
    {
        return model.getvideofilepath();
    }
    // Thêm một giá trị trả về mặc định hoặc xử lý lỗi
    throw std::runtime_error("Invalid tab selected");
}
void Controller::handleSetDirectory(const std::string &directory)
{
    //Set tab to display
    view.settab(MUSIC);
    //Set filelist to display
    view.setdisplayfilelist(parseTabtofiles());
    //Set filelist executing
    executing_lisfile = parseTabtofiles();
    //Push current tab to stack
    tabHistory.push(MUSIC);
    //Set page is 0
    view.setpage(1);
}

void Controller::handlePlay()
{
    //Get current playing listfile
    executing_lisfile.clear();
    executing_lisfile = parseTabtofiles();
    //Get current playing list filepath
    executing_listfilepath.clear();
    executing_listfilepath = parseTabtofilepaths();
    //Set flag playing
    current_flag = NUM_MODE;
    //std::cout << "Enter song to play:  \n";
    int num;
    
    // Get data from queue
    std::string task = getTaskFromQueue();
        // If task == "S" enter mode (run with input from MCU)
        if (task == "S")
        {
            num = current_song;
        }
        // Run with input from Cin
        else
        {
            // Convert from string to interger
            num = std::stoi(task);
            if((num > executing_lisfile.size())&&(num < 1)){
                std::cout << "Invalid choice." << std::endl;
                num = current_song;
            }
        }
        // Set current track
        player.setTrack((num - 1));
        // Set current songlist
        player.setSonglist(executing_listfilepath);
        // Play song
        player.play(executing_listfilepath[num - 1]);
        /*Send message to blink GREEN led and turn off led RED*/
        std::string blinkgreen_message;
        // create message
        mcu_data.createMessage('O', 1);
        // convert message to string
        mcu_data.messageToString(mcu_data.getmess(), blinkgreen_message);
        // Send string to MCU
        sp.sendData(blinkgreen_message);
    current_flag = ACT_MODE;
}

void Controller::handlePause()
{
    player.pause();
    /*Send message to turn off GREEN led blink led RED*/
    std::string blinkred_message;
    // create message
    mcu_data.createMessage('O', 0);
    // convert message to string
    mcu_data.messageToString(mcu_data.getmess(), blinkred_message);
    // Send string to MCU
    sp.sendData(blinkred_message);
}

void Controller::handleResume()
{
    player.resume();
    /*Send message to blink GREEN led and turn off led RED*/
    std::string blinkgreen_message;
    // create message
    mcu_data.createMessage('O', 1);
    // convert message to string
    mcu_data.messageToString(mcu_data.getmess(), blinkgreen_message);
    // Send string to MCU
    sp.sendData(blinkgreen_message);
}

void Controller::handleStop()
{
    player.stop();
    /*Send message to turn off GREEN led blink led RED*/
    std::string blinkred_message;
    // create message
    mcu_data.createMessage('O', 0);
    // convert message to string
    mcu_data.messageToString(mcu_data.getmess(), blinkred_message);
    // Send string to MCU
    sp.sendData(blinkred_message);
}
void Controller::handleNext()
{
    player.next();
    /*Send message to blink GREEN led and turn off led RED*/
    std::string blinkgreen_message;
    // create message
    mcu_data.createMessage('O', 1);
    // convert message to string
    mcu_data.messageToString(mcu_data.getmess(), blinkgreen_message);
    // Send string to MCU
    sp.sendData(blinkgreen_message);
}
void Controller::handlePrevious()
{
    player.previous();
    /*Send message to blink GREEN led and turn off led RED*/
    std::string blinkgreen_message;
    // create message
    mcu_data.createMessage('O', 1);
    // convert message to string
    mcu_data.messageToString(mcu_data.getmess(), blinkgreen_message);
    // Send string to MCU
    sp.sendData(blinkgreen_message);
}

void Controller::handleExit()
{
    exit(0);
}

void Controller::handleSwitchTab(const Tab tab)
{
    if (tab != view.gettab())
    {
        //push current tab to stack
        tabHistory.push(tab);
        if (tab == HOME)
        {
            //Set current tab to display
            view.settab(HOME);
            //Set filelist to display
            view.setdisplayfilelist(parseTabtofiles());
        }
        else if (tab == MUSIC)
        {
            //Set current tab to display
            view.settab(MUSIC);
            //Set filelist to display
            view.setdisplayfilelist(parseTabtofiles());
        }
        else if (tab == VIDEO)
        {
            //Set current tab to display
            view.settab(VIDEO);
            //Set filelist to display
            view.setdisplayfilelist(parseTabtofiles());
        }
        else
        {
            //Set current tab to display
            view.settab(HOME);
            //Set filelist to display
            view.setdisplayfilelist(parseTabtofiles());
        }
        //Set start page is 1
        view.setpage(1);
    }
}

void Controller::handleEditMetadata()
{
    std::cout << "Choose file to edit: ";
    int num;
    bool edit = false;
    std::cin >> num;
    MediaFile file;
    file.setPath(parseTabtofiles()[num - 1].getPath());
    file.setType(parseTabtofiles()[num - 1].getType());
    if (!parseTabtofiles().empty())
    {
        if (parseTabtofiles()[num - 1].getType() == ".mp3")
        {
            // Edit file mp3
            std::string title, artist, album, genre;
            int year;
            char choice;
            while (!edit)
            {
                std::cout << "Select the metadata field to edit:\n";
                std::cout << "1. Title\n";
                std::cout << "2. Artist\n";
                std::cout << "3. Album\n";
                std::cout << "4. Genre\n";
                std::cout << "5. Year\n";
                std::cout << "6. Done\n";
                std::cout << "Enter choice: ";
                std::cin >> choice;
                switch (choice)
                {
                case '1':
                    std::cout << "Enter new title (current: " << parseTabtofiles()[num - 1].getTitle() << "): ";
                    std::getline(std::cin >> std::ws, title);
                    if (!title.empty())
                        file.setTitle(title);
                    break;
                case '2':
                    std::cout << "Enter new artist (current: " << parseTabtofiles()[num - 1].getArtist() << "): ";
                    std::getline(std::cin >> std::ws, artist);
                    if (!artist.empty())
                        file.setArtist(artist);
                    break;
                case '3':
                    std::cout << "Enter new album (current: " << parseTabtofiles()[num - 1].getAlbum() << "): ";
                    std::getline(std::cin >> std::ws, album);
                    if (!album.empty())
                        file.setAlbum(album);
                    break;
                case '4':
                    std::cout << "Enter new genre (current: " << parseTabtofiles()[num - 1].getGenre() << "): ";
                    std::getline(std::cin >> std::ws, genre);
                    if (!genre.empty())
                        file.setGenre(genre);
                    break;
                case '5':
                    std::cout << "Enter new year (current: " << parseTabtofiles()[num - 1].getYear() << "): ";
                    std::cin >> year;
                    if (!std::cin.fail()) // Basic validation
                        file.setYear(year);
                    std::cin.clear();                                                   // Clear any error flags
                    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
                    break;
                case '6':
                    edit = true;
                    // model.savenewMetadata(parseTabtofiles()[num - 1]);
                    break; // Exit edit
                default:
                    std::cout << "Invalid choice edit. Please select again." << std::endl;
                    break;
                }
            }
        }
        else if (parseTabtofiles()[num - 1].getType() == ".mp4")
        {
            // Edit file mp4
            std::string name;
            std::cout << "Enter new name (current: " << parseTabtofiles()[num - 1].getName() << "): ";
            std::getline(std::cin >> std::ws, name);
            if (!name.empty())
            {
                file.setName(name);
            }
        }
        model.savenewMetadata(file);
        // get path after change
        model.setDirectory(current_Dir);
    }
}

void Controller::handleNextPage()
{
    int page = view.getpage();
    page++;
    if (page < (view.gettotalpage(parseTabtofiles()) + 1))
    {
        view.setpage(page);
    }
}

void Controller::handlePrevPage()
{
    int currentPage = view.getpage();
    if (view.getpage() > 1)
    {
        currentPage--;
        view.setpage(currentPage);
    }
}

void Controller::handleRemoveFile()
{
    current_flag = NUM_MODE;
    int num;
    // Get data from queue
    std::string task = getTaskFromQueue();
    // If task == "S" enter mode (run with input from MCU)
    if (task == "S")
    {
        num = current_song;
        model.removeFile(parseTabtofilepaths(), num);
    }
    // Run with input from Cin
    else
    {
        // Convert from string to interger
        num = std::stoi(task);
        if((num > parseTabtofilepaths().size())&&(num < 1)){
            std::cout << "Invalid choice." << std::endl;
        }
        else{
            model.removeFile(parseTabtofilepaths(), num);
        }
    }
    current_flag = ACT_MODE;
   
}
void Controller::handleBack()
{
    if (tabHistory.size() > 1)
    {
        tabHistory.pop();
        view.settab(tabHistory.top());
        view.setdisplayfilelist(parseTabtofiles());
    }
}