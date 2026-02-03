#include "TelemetryApplication.hpp"

int main(){
    TelemetryApplication app("config.json");
    app.start();
    return 0;
}