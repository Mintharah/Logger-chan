#include "SomeIPTelemetrySourceImpl.hpp"
#include <thread>

SomeIPTelemetrySourceImpl::SomeIPTelemetrySourceImpl() {
  app = vsomeip::runtime::get()->create_application("Telemetry_Client");
}
SomeIPTelemetrySourceImpl::~SomeIPTelemetrySourceImpl() {
  if (app) {
    app->stop();
  }
}

bool SomeIPTelemetrySourceImpl::openSource() {
  if (!app) {
    return false;
  }
  app->init();
  app->register_availability_handler(
      SERVICE_ID, INSTANCE_ID,
      std::bind(&SomeIPTelemetrySourceImpl::on_availability, this,
                std::placeholders::_1, std::placeholders::_2,
                std::placeholders::_3));
  ;
  app->register_message_handler(
      SERVICE_ID, INSTANCE_ID, METHOD_ID,
      std::bind(&SomeIPTelemetrySourceImpl::on_response, this,
                std::placeholders::_1));

  app->request_service(SERVICE_ID, INSTANCE_ID);
  std::thread([this](){
    app->start();
  }).detach();

  std::unique_lock<std::mutex> uL(avail_mtx);
  avail_cv.wait(uL, [this]() { return this->service_available; });

  return service_available;
}

bool SomeIPTelemetrySourceImpl::readSource(std::string &out) {
  if (!service_available) {
    return false;
  }

  got_response = false;

  std::shared_ptr<vsomeip::message> request =
      vsomeip::runtime::get()->create_request();
  request->set_service(SERVICE_ID);
  request->set_instance(INSTANCE_ID);
  request->set_method(METHOD_ID);
  request->set_client(app->get_client());

  app->send(request);

  std::unique_lock<std::mutex> uL(resp_mtx);
  resp_cv.wait(uL, [this]() { return this->got_response; });

  out = this->response_data;
  return true;
}

void SomeIPTelemetrySourceImpl::on_availability(vsomeip::service_t service,
                                                vsomeip::instance_t instance,
                                                bool is_available) {
  if (service == SERVICE_ID && instance == INSTANCE_ID && is_available) {
    std::unique_lock<std::mutex> uL(avail_mtx);
    service_available = true;
  }
  avail_cv.notify_one();
}

void SomeIPTelemetrySourceImpl::on_response(
    const std::shared_ptr<vsomeip::message> &response) {
  if (response->get_service() == SERVICE_ID &&
      response->get_instance() == INSTANCE_ID &&
      response->get_method() == METHOD_ID) {
    std::shared_ptr<vsomeip::payload> payload = response->get_payload();
    if (payload) {
      vsomeip::length_t len = payload->get_length();
      const vsomeip::byte_t *data = payload->get_data();

      response_data.clear();
      for (vsomeip::byte_t i = 0; i < len; i++) {
        response_data.push_back(static_cast<char>(data[i]));
      }

      {
        std::lock_guard<std::mutex> lock(resp_mtx);
        got_response = true;
      }
      resp_cv.notify_one();
    }
  }
}