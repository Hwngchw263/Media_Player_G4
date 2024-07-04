#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <map>
#include "Mediafile.h"

class Model {
public:
    bool setDirectory(const std::string& directory);
    std::vector<std::string>& getpath();
    std::vector<MediaFile>& getMetadataofmedia();
    std::vector<MediaFile>& getMetadataofmusic();
    std::vector<MediaFile>& getMetadataofvideo();
    std::vector<std::string>& getmediafilepath();
    std::vector<std::string>& getmusicfilepath();
    std::vector<std::string>& getvideofilepath();
    void savenewMetadata(const MediaFile &mediaFiles);
    void removeFile(std::vector<std::string>& filepaths, int num);
private:
    std::string directory;
    std::vector<std::string> mediaFiles;
    std::vector<std::string> musicFiles;
    std::vector<std::string> videoFiles;
    std::vector<MediaFile> metadataofmedia;
    std::vector<MediaFile> metadataofmusic;
    std::vector<MediaFile> metadataofvideo;
};

#endif // MODEL_H
