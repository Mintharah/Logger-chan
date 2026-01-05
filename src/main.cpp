#include "LogManager.hpp"
#include "LogMessage.hpp"
#include "ConsoleSinkImpl.hpp"
#include "FileSinkImpl.hpp"



int main(){

    LogMessage message;
    message.app_name = "my app c:";
    message.context = "test";
    message.severity = "meh severity";
    message.text = "ayaya";
    message.time = "log-o-clock";

    LogManager manager;
    
    auto console = std::make_unique<ConsoleSinkImpl>();
    auto file = std::make_unique<FileSinkImpl>();
    file->filePath("/home/yasmine/Desktop/Linux_Project/FileLogging.txt");
    
    manager.addSink(std::move(console));
    manager.addSink(std::move(file));
    
    manager.log(message);
    manager.flush();

    return 0;
}