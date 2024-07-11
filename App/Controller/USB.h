#ifndef USB_H
#define USB_H

#include <string>
#include <vector>

class USB {
public:
    USB();

    std::string getUsername() const;
    std::vector<std::string> getMountedUSBNames() const;
    std::string getUSBPath() const;
    std::string getCurrentUsername() const;
    std::vector<std::string> getUSBPaths() const;

    void displayMenu();
    void playFromUSB();
    void playFromFolder();

private:
    std::string username;

    
};

#endif // USB_H