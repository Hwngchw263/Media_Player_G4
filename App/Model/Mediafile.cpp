#include "Mediafile.h"
// Constructor
MediaFile::MediaFile() : year(0), duration(0) {}

MediaFile::MediaFile(const std::string &path, const std::string &name)
    : path(path), name(name), year(0), duration(0) {}

MediaFile::MediaFile(const std::string &path, const std::string &title, const std::string &artist, const std::string &album, const std::string &genre, int year, int duration)
    : path(path), title(title), artist(artist), album(album), genre(genre), year(year), duration(duration) {}
// Get Metadata of file
std::string MediaFile::getPath() const { return path; }
std::string MediaFile::getName() const { return name; }
std::string MediaFile::getTitle() const { return title; }
std::string MediaFile::getArtist() const { return artist; }
std::string MediaFile::getAlbum() const { return album; }
std::string MediaFile::getGenre() const { return genre; }
int MediaFile::getYear() const { return year; }
int MediaFile::getBitrate() const {return bitrate;}
int MediaFile::getSize() const {return size;}
int MediaFile::getDuration() const { return duration; }
std::string MediaFile::getType() const{return type;}
// Edit Metadata of file
void MediaFile::setPath(const std::string &path) { this->path = path; }
void MediaFile::setName(const std::string &name) { this->name = name; }
void MediaFile::setTitle(const std::string &title) { this->title = title; }
void MediaFile::setArtist(const std::string &artist) { this->artist = artist; }
void MediaFile::setAlbum(const std::string &album) { this->album = album; }
void MediaFile::setGenre(const std::string &genre) { this->genre = genre; }
void MediaFile::setYear(int year) { this->year = year; }
void MediaFile::setBitrate(int bitrate){this->bitrate = bitrate;}
void MediaFile::setSize(int size){this->size = size;}
void MediaFile::setDuration(int duration) { this->duration = duration; }
void MediaFile::setType(const std::string &type) { this->type = type; }
bool MediaFile::operator==(const MediaFile &other) const
{
    return path == other.path && name == other.name;
}