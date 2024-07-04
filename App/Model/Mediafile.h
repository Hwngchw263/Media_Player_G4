#ifndef MEDIAFILE_H
#define MEDIAFILE_H

#include <string>

class MediaFile {
public:
//Constructors
    MediaFile();
    MediaFile(const std::string &path, const std::string &name);
    MediaFile(const std::string &path, const std::string &title, const std::string &artist, const std::string &album, const std::string &genre, int year, int duration);
//Methods
    //Get Metadata of file
    std::string getPath() const;
    std::string getName() const;
    std::string getTitle() const;
    std::string getArtist() const;
    std::string getAlbum() const;
    std::string getGenre() const;
    std::string getType() const;
    int getYear() const;
    int getSize() const;
    int getBitrate() const;
    int getDuration() const;

    //Edit Metadata of file
    void setPath(const std::string &path);
    void setName(const std::string &name);
    void setTitle(const std::string &title);
    void setArtist(const std::string &artist);
    void setAlbum(const std::string &album);
    void setGenre(const std::string &genre);
    void setYear(int year);
    void setSize(int size);
    void setBitrate(int bitrate);
    void setDuration(int duration);
    void setType(const std::string &type);
    bool operator==(const MediaFile &other) const;
private:
//Properties of media files
    std::string path;
    std::string name;
    std::string title;
    std::string artist;
    std::string album;
    std::string genre;
    std::string type;
    int size;
    int bitrate;
    int year;
    int duration;
};

#endif // MEDIAFILE_H
