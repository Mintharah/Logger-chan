#include <vsomeip/vsomeip.hpp>
#include "CPUTelemetrySource.hpp"
#include <iostream>
#include <thread>

#define SERVICE_ID 0x1234
#define INSTANCE_ID 0x5678
#define METHOD_ID 0x0125

int main() {
    // Initialize CPU source
    CPUTelemtrySourceImpl cpuSource;
    if (!cpuSource.openSource()) {
        std::cerr << "Failed to open /proc/stat" << std::endl;
        return 1;
    }

    auto app = vsomeip::runtime::get()->create_application("Telemetry_Server");
    app->init();

    app->register_message_handler(
        SERVICE_ID, INSTANCE_ID, METHOD_ID,
        [&app, &cpuSource](std::shared_ptr<vsomeip::message> request) {
            std::string cpuData;
            
            // Use cpu logic to get the percentage string
            if (cpuSource.readSource(cpuData)) {
                std::cout << "Broadcasting CPU Load: " << cpuData << "%" << std::endl;

                auto payload = vsomeip::runtime::get()->create_payload();
                payload->set_data(reinterpret_cast<const vsomeip::byte_t*>(cpuData.data()), cpuData.size());

                auto response = vsomeip::runtime::get()->create_response(request);
                response->set_payload(payload);
                app->send(response);
            }
        }
    );

    app->offer_service(SERVICE_ID, INSTANCE_ID);
    app->start();

    std::cout << "Server running..." << std::endl;
    while (true) std::this_thread::sleep_for(std::chrono::seconds(1));
}