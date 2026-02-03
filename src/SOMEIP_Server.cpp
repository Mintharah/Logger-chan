#include <vsomeip/vsomeip.hpp>
#include <iostream>
#include <thread>

#define SERVICE_ID 0x1234
#define INSTANCE_ID 0x5678
#define METHOD_ID 0x0125

int main() {
    auto app = vsomeip::runtime::get()->create_application("Telemetry_Server");

    app->init();

    // Register message handler
    app->register_message_handler(
        SERVICE_ID, INSTANCE_ID, METHOD_ID,
        [app](std::shared_ptr<vsomeip::message> request) {
            std::string data = "42.5%"; // example payload
            auto payload = vsomeip::runtime::get()->create_payload();
            payload->set_data(reinterpret_cast<const vsomeip::byte_t*>(data.data()), data.size());

            auto response = vsomeip::runtime::get()->create_response(request);
            response->set_payload(payload);

            app->send(response);
        }
    );

    app->offer_service(SERVICE_ID, INSTANCE_ID);
    app->start();

    std::cout << "Server running..." << std::endl;
    while (true) std::this_thread::sleep_for(std::chrono::seconds(1));
}