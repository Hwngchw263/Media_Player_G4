#include "Controller.h"
#include <iostream>
#include <sstream>
#include <algorithm>

Player *Controller::playerptr = nullptr;
Controller::Controller() : sp(SerialPort::getOpenSDADevicePath().c_str()), serial_command_received(false)
{
    Mix_VolumeMusic(1);
    // Configure for serial port
    if (!sp.configure())
    {
        std::cerr << "Failed to configure serial port" << std::endl;
        std::exit(1); // Exit if configuration fails
    }
}
/// vcllll

Controller::~Controller()
{
}
//------------------------------Hook for music------------------------------------
void Controller::MusicFinishedCallbackWrapper()
{
    playerptr->FunctionCallback();
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

    // Start
    std::thread task_thread(&Controller::executeTask, this);
    std::thread serial_input_thread(&Controller::getInputFromSerial, this);
    std::thread cin_input_thread(&Controller::getInputFromCin, this);

    serial_input_thread.join();
    cin_input_thread.join();
    task_thread.join();
    //  running = false;
    condition.notify_one();
}

void Controller::getInputFromSerial()
{
    while (running)
    {
        view.displayMetadata(parseTabtofiles());
        std::string message = sp.receiveData();
        if (!message.empty())
        {
            // std::cout << "Received from Serial: " << message << std::endl;
            bool messageValid = false;
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                mcu_data.ParseMessage(message);
                messageValid = mcu_data.VerifyMessage(mcu_data.getmess());
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
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void Controller::getInputFromCin()
{
    while (running)
    {
        view.displayMetadata(parseTabtofiles());
        std::string cmd;
        if (!is_playing)
        {
            std::cout << "Enter command: ";
        }
        std::getline(std::cin, cmd);
        if (!cmd.empty())
        {
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                taskQueue.push(cmd);
            }
            condition.notify_one();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void Controller::processMessage(const std::string &message)
{
    if (mcu_data.VerifyMessage(mcu_data.getmess()))
    {
        // std::cout << "Message true.\n" << std::endl;
        // std::cout << "Type: " << mcu_data.getmess().type << std::endl;
        // std::cout << "Data: " << mcu_data.getmess().data << std::endl;
        // std::cout << "Checksum: " << mcu_data.getmess().checksum << std::endl;

        if (mcu_data.getmess().type == 'A')
        {
            handleModeAndSongSelection();
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
            int volume_value = mcu_data.getmess().data;
            Mix_VolumeMusic(volume_value);
        }
    }
    else
    {
        std::cout << "Message failed. Send again" << std::endl;
    }
}

void Controller::handleModeAndSongSelection()
{

    // std::cout << "lock mode" << std::endl;
    mode = mcu_data.PareMode(mcu_data.getmess().data, TOTAL_MODE);
    {
        std::unique_lock<std::mutex> lock(numsong_mutex);
        // std::cout << "lock Numsongmode" << std::endl;
        numsong = mcu_data.getmess().data % sizeof(parseTabtofiles());
        // std::cout << "\033[A\033[2K";
        if (is_playing)
        {
            std::cout << "Song: " << numsong << std::endl;
        }
        else
        {
            std::cout << "Enter command: ";
            std::cout << "Mode: " << mode << std::endl;
        }
    }
}

std::string Controller::getTaskFromQueue()
{
    std::lock_guard<std::mutex> lock(queueMutex);
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
    while (running)
    {
        std::unique_lock<std::mutex> lock(condition_mutex);
        condition.wait(lock, [&]
                       { return !taskQueue.empty(); });
        std::string task = getTaskFromQueue();
        // std::cout << "Task: " << task << std::endl;
        if (!task.empty())
        {
            handleTask(task);
        }
    }
}

void Controller::handleTask(const std::string &task)
{
    // std::cout << "Handling task: " << task << std::endl;
    if (task == "S")
    {
        std::lock_guard<std::mutex> lock(mode_mutex);
        handleInput(mode);
    }
    else
    {
        {
            std::lock_guard<std::mutex> lock(mode_mutex);
            mode = task[0];
        }
        handleInput(task[0]);
    }
}

void Controller::handleInput(const char &input)
{
    playerptr = &player;
    Mix_HookMusicFinished(MusicFinishedCallbackWrapper);
    switch (input)
    {
    case '1':
        std::cout << "\nplay mode\n";
        handlePlay();
        break;
    case '2':
        std::cout << "\npause music\n";
        handlePause();
        break;
    case '3':
        std::cout << "\nresume music\n";
        handleResume();
        break;
    case '4':
        std::cout << "\nstop music\n";
        handleStop();
        break;
    case '5':
        std::cout << "\nnext music\n";
        handleNext();
        break;
    case '6':
        std::cout << "\nprevious music\n";
        handlePrevious();
        break;
    case '+':
        handleVolume('+');
        break;
    case '-':
        handleVolume('-');
        break;
    case 'q':
        handleExit();
        break;
    case 'h':
        handleSwitchTab(HOME);
        break;
    case 'm':
        handleSwitchTab(MUSIC);
        break;
    case 'v':
        handleSwitchTab(VIDEO);
        break;
    case 'e':
        handleEditMetadata();
        break;
    case '>':
        handleNextPage();
        break;
    case '<':
        handlePrevPage();
        break;
    case 'd':
        handleRemoveFile();
        break;
    case 'b':
        handleBack();
        break;
    default:
        std::cout << "Unknown command." << std::endl;
        // view.displayHelp();
        view.displayMetadata(parseTabtofiles());
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
    cur_dir = directory;
    if (model.setDirectory(directory))
    {
        view.settab(MUSIC);
        tabHistory.push(MUSIC);
        view.setpage(0);
        view.displayMetadata(parseTabtofiles());
    }
    else
    {
        std::cout << "Invalid directory. Please try again." << std::endl;
    }
}

void Controller::handlePlay()
{
    is_playing = true;
    // TO DO:Reset num in MCU num = 0
    std::cout << "Enter song to play:  \n";
    int num;
    std::string task;
    // taskQueue.push("S");
    // Get data from queue
    {
        std::unique_lock<std::mutex> lock(queueMutex);
        condition.wait(lock, [&]
                       { return !taskQueue.empty(); });
        if (!taskQueue.empty())
        {
            task = taskQueue.front();
            taskQueue.pop();
        }
        // If task == "S" enter mode (run with input from MCU)
        if (task == "S")
        {
            std::unique_lock<std::mutex> lock(numsong_mutex);
            num = numsong;
        }
        // Run with input from Cin
        else
        {
            // Convert from string to interger
            num = std::stoi(task);
        }
        // Set current track
        player.setTrack((num - 1));
        // Set current songlist
        player.setSonglist(parseTabtofilepaths());
        // Set current file list
        player.setMediafile(parseTabtofiles());
        // Play song
        player.play(parseTabtofilepaths()[num - 1]);
        /*Send message to blink GREEN led and turn off led RED*/
        std::string blinkgreen_message;
        // create message
        mcu_data.createMessage('O', 1);
        // convert message to string
        mcu_data.messageToString(mcu_data.getmess(), blinkgreen_message);
        // Send string to MCU
        sp.sendData(blinkgreen_message);
        // TO DO:: reset num in MCU to old mode
    }
    is_playing = false;
    // view.displayMetadata(parseTabtofiles());
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
    std::string blinkred_message;
    // create message
    mcu_data.createMessage('O', 1);
    // convert message to string
    mcu_data.messageToString(mcu_data.getmess(), blinkred_message);
    // Send string to MCU
    sp.sendData(blinkred_message);
}
void Controller::handlePrevious()
{
    player.previous();
    std::string blinkred_message;
    // create message
    mcu_data.createMessage('O', 1);
    // convert message to string
    mcu_data.messageToString(mcu_data.getmess(), blinkred_message);
    // Send string to MCU
    sp.sendData(blinkred_message);
}
void Controller::handleVolume(char volume)
{
    player.setVolume(volume);
}
void Controller::handleExit()
{
    exit(0);
}

void Controller::handleSwitchTab(const Tab tab)
{
    if (tab != view.gettab())
    {
        tabHistory.push(tab);
        if (tab == HOME)
        {
            view.settab(HOME);
        }
        else if (tab == MUSIC)
        {
            view.settab(MUSIC);
        }
        else if (tab == VIDEO)
        {
            view.settab(VIDEO);
        }
        else
        {
            view.settab(HOME);
        }
        view.setpage(0);
    }
    view.displayMetadata(parseTabtofiles());
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
                    std::cout << "Invalid choice. Please select again." << std::endl;
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
        model.setDirectory(cur_dir);
    }
    view.displayMetadata(parseTabtofiles());
}

void Controller::handleNextPage()
{
    int page = view.getpage();
    page++;
    if (page < view.gettotalpage(parseTabtofiles()))
    {
        view.setpage(page);
    }
    view.displayMetadata(parseTabtofiles());
}

void Controller::handlePrevPage()
{
    int currentPage = view.getpage();
    if (view.getpage() > 0)
    {
        currentPage--;
        view.setpage(currentPage);
    }
    view.displayMetadata(parseTabtofiles());
}

void Controller::handleRemoveFile()
{
    std::cout << "Choose file to remove: " << std::endl;
    int num;
    std::cin >> num;
    // Check valid of num
    model.removeFile(parseTabtofilepaths(), num);
    view.displayMetadata(parseTabtofiles());
}

void Controller::handleBack()
{
    if (tabHistory.size() > 1)
    {
        tabHistory.pop();
        view.settab(tabHistory.top());
    }
    view.displayMetadata(parseTabtofiles());
}