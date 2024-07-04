#include "Model.h"
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <taglib/fileref.h>
#include <taglib/tag.h>
namespace fs = std::filesystem;

bool Model::setDirectory(const std::string& directory) {
    this->directory = directory;
    mediaFiles.clear();
    musicFiles.clear();
    videoFiles.clear();
    if (!fs::exists(directory) || !fs::is_directory(directory)) {
        return false;
    }

    for (const auto& entry : fs::recursive_directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string path = entry.path().string();
            // Check if the file has a media extension (e.g., .mp3, .mp4)
            std::string extension = entry.path().extension();
            if (extension == ".mp3" || extension == ".mp4") {
                if(extension == ".mp3"){
                    musicFiles.push_back(path);              
                }
                if(extension == ".mp4"){
                    videoFiles.push_back(path);
                }
                mediaFiles.push_back(path);
            }
        }
    }
    return true;
}



std::vector<MediaFile>& Model::getMetadataofmedia(){
    MediaFile file;
    metadataofmedia.clear();
    for (const auto& elem : mediaFiles) {
        TagLib::FileRef f(elem.c_str());
        if (!f.isNull() && f.tag()) {
            TagLib::Tag *tag = f.tag();
            file.setPath(elem.c_str());
            file.setName(fs::path(elem.c_str()).stem());
            file.setType(fs::path(elem.c_str()).extension());
            file.setSize(fs::file_size(elem.c_str()) / 1000);
        }
        metadataofmedia.push_back(file);
    }
    return metadataofmedia;
}
std::vector<MediaFile>& Model::getMetadataofmusic(){
    MediaFile file;
    metadataofmusic.clear();
    for (const auto& elem : musicFiles) {
        TagLib::FileRef f(elem.c_str());
        if (!f.isNull() && f.tag()) {
            TagLib::Tag *tag = f.tag();
            file.setPath(elem.c_str());
            file.setTitle(tag->title().toCString(true));
            file.setType(fs::path(elem.c_str()).extension());
            file.setArtist(tag->artist().toCString(true));
            file.setAlbum(tag->album().toCString(true));
            file.setGenre(tag->genre().toCString(true));
            file.setYear(tag->year());
            file.setDuration(f.audioProperties()->length());
        }
        metadataofmusic.push_back(file);
    }
    return metadataofmusic;
}
std::vector<MediaFile>& Model::getMetadataofvideo(){
    MediaFile file;
    metadataofvideo.clear();
    for (const auto& elem : videoFiles) {
        TagLib::FileRef f(elem.c_str());
        if (!f.isNull() && f.tag()) {
            TagLib::Tag *tag = f.tag();
            file.setPath(elem.c_str());
            file.setName(fs::path(elem.c_str()).stem());
            file.setType(fs::path(elem.c_str()).extension());
            file.setSize(fs::file_size(elem.c_str()) / 1000);
            file.setBitrate(f.audioProperties()->bitrate());
            file.setDuration(f.audioProperties()->length());
        }
        metadataofvideo.push_back(file);
    }
    return metadataofvideo;
}
std::vector<std::string>& Model::getpath(){
    return mediaFiles;
}
std::vector<std::string>& Model::getmediafilepath(){
    return mediaFiles;
}
std::vector<std::string>& Model::getmusicfilepath(){
    return musicFiles;
}
std::vector<std::string>& Model::getvideofilepath(){
    return videoFiles;
}
void Model::savenewMetadata(const MediaFile &mediaFile) {
    if(mediaFile.getType() == ".mp3"){
        // Edit metadata of the media file
        TagLib::FileRef f(mediaFile.getPath().c_str());
        if (!f.isNull() && f.tag()) {
            TagLib::Tag *tag = f.tag();
            tag->setTitle(mediaFile.getTitle());
            tag->setArtist(mediaFile.getArtist());
            tag->setAlbum(mediaFile.getAlbum());
            tag->setGenre(mediaFile.getGenre());
            tag->setYear(mediaFile.getYear());
            f.save();
        } 
    }
    else if(mediaFile.getType() == ".mp4"){
        //take extension of old file
        std::string extension = fs::path(mediaFile.getPath()).extension().string();
        std::cout << mediaFile.getPath() << std::endl;
        //Creat new path with new name and oldpath
        fs::path newFilePath = fs::path(mediaFile.getPath()).parent_path() / (mediaFile.getName() + extension);
        std::cout << newFilePath << std::endl;
        fs::rename(mediaFile.getPath(),newFilePath);
    }
}

void Model::removeFile(std::vector<std::string>& filepaths, int num) {
     filepaths.erase(filepaths.begin() + num - 1);
}
