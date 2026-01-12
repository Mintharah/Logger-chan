#include "SafeFile.hpp"

SafeFile::SafeFile(int fd) : fd(fd){}

SafeFile::SafeFile(SafeFile&& other) : fd(other.fd){
    other.fd = -1;
}

SafeFile& SafeFile::operator=(SafeFile&& other){
    if(this != &other){
        if(fd >= 0){
            close(fd);
        }
        fd = other.fd;
        other.fd = -1;
    }
    return *this; 
}

SafeFile::~SafeFile(){
    if(fd >= 0){
        close(fd);
    }
}

bool SafeFile::isOpen() const{
    return (fd >= 0);
}

int SafeFile::get() const{
    return fd;
}

