#ifndef LOGMANAGERBUILDER_HPP
#define LOGMANAGERBUILDER_HPP

#include "LogManager.hpp"
#include "ConsoleSinkImpl.hpp"
#include "FileSinkImpl.hpp"
#include <memory>

class LogManagerBuilder {
private:
std::unique_ptr<LogManager> manager;
public:
LogManagerBuilder() : manager(std::make_unique<LogManager>()){}

LogManagerBuilder& addConsoleSink(){
    manager->addSink(std::make_unique<ConsoleSinkImpl>());
    return *this;
}

LogManagerBuilder& addFileSink(const std::string& path){
    auto sink = std::make_unique<FileSinkImpl>();
    sink->filePath(path);
    manager->addSink(std::move(sink));
    return *this;
}

std::unique_ptr<LogManager> build(){
    return std::move(manager);
}

};


#endif