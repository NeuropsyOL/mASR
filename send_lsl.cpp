#include "lsl_cpp.h"
#include "rapidcsv/rapidcsv.h"
#include <chrono> // std::chrono::seconds
#include <thread> // std::this_thread::sleep_for

/** This code is adapted from the LSL example code and is intended to send
 * test data over the network via LSL for the asr plugin to receive it as
 * "live" data.
 * Usage: send_lsl [STREAM_NAME] [DATA_SOURCE]
 * STREAM_NAME: Name of the LSL stream. Default: SimpleStream
 * DATA_SOURCE: File name of the input file. send_lsl expects a comma separated table
 * where each row represents an EEG channel
 */

int main(int argc, char *argv[]) {

  // Read data from file
  std::string filename = argc > 2 ? argv[2] : "mASR-testdata/proc.csv";
  auto data = rapidcsv::Document(filename, rapidcsv::LabelParams(-1, -1));
  // create a new stream_info and open an outlet with it - Take name from cmd
  // line or use "SimpleStream" as default
  lsl::stream_info info(argc > 1 ? argv[1] : "SimpleStream", "EEG",
                        data.GetRowCount());
  lsl::stream_outlet outlet(info);
  // Wait until there is a consumer before continuing - timeout is 999s
  outlet.wait_for_consumers(999);

  // buffer for table data
  std::vector<double> samples;
  std::size_t idx = 0;

  for (unsigned ii = 0; ii < data.GetColumnCount(); ++ii) {
    samples = data.GetColumn<double>(ii);
    outlet.push_sample(samples);
    idx++;
    // sleep for 1 ms to limit rate to 1000 Hz
    // the real rate will be slightly below, as no compensation for
    // processing time is done for
    // simplicity reasons, we can expect occasional dropouts in this example.
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  // Wait until last consumer has quit
  while (outlet.have_consumers()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  return idx;
}
