#ifndef SOMEIPTELEMETRYSOURCEIMP
#define SOMEIPTELEMETRYSOURCEIMP

#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
#include <vsomeip/vsomeip.hpp>

#define SERVICE_ID 0x1234
#define INSTANCE_ID 0x5678
#define METHOD_ID 0x0125

class SomeIPTelemetrySourceImpl {
private:
  void on_availability(vsomeip::service_t service, vsomeip::instance_t instance,
                       bool is_available);
  void on_response(const std::shared_ptr<vsomeip::message> &response);

  std::shared_ptr<vsomeip::application> app;
  bool service_available = false;
  std::mutex avail_mtx;
  std::condition_variable avail_cv;

  bool got_response = false;
  std::string response_data;
  std::mutex resp_mtx;
  std::condition_variable resp_cv;

public:
  SomeIPTelemetrySourceImpl();
  ~SomeIPTelemetrySourceImpl();

  bool openSource();
  bool readSource(std::string &out);
};

#endif