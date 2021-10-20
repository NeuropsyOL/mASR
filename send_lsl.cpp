#include "lsl_cpp.h"
#include "rapidcsv.h"
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

/**
 * This is a minimal lsl example that demonstrates how a multi-channel
 * stream can be used to push data to the network via stream outlet.
 * It creates a stream outlet, waits until there are one or more consumers
 * and then sends random data to the stream.
 */

int main(int argc, char* argv[]) {

  // Read data from file
  std::string filename=argc > 2 ? argv[2] : "calib_csv/sme_1_1.xdf_filt.csv";
std::cout<<filename<<"\n";
  auto data=rapidcsv::Document(filename,rapidcsv::LabelParams(-1,-1));

  // create a new stream_info and open an outlet with it - Take name from cmd line or use
  // "SimpleStream" as default
  lsl::stream_info info(argc > 1 ? argv[1] : "SimpleStream", "EEG", data.GetRowCount());
  lsl::stream_outlet outlet(info);

  // Wait until there is a consumer before continuing - timeout is 999s
  outlet.wait_for_consumers(999);

  // allocate buffer for data
  std::vector<double> samples;
  std::size_t idx=0;
  while(outlet.have_consumers()) {
    // Fill the buffer with data
    samples=data.GetColumn<double>(idx++ % data.GetColumnCount());
    // send it, using lsl_local_clock as timestamp
    outlet.push_sample(samples);
    // sleep for 10 ms to limit rate to 100 Hz to get a similar frame rate as the receiving end
    // the real rate will be slightly below, as no compensation is done for simplicity reasons,
    // we can expect occasional dropouts in this example.
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }
  return 0;
}
 
