#include "TelemetryApplication.hpp"

int main(){
    TelemetryApplication app("/home/raspberry/logger-chan/config.json");
    app.start();
    return 0;
}