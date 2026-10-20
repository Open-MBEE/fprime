#ifndef Fw_Port_test_ut_span_SpanTestReceiver_HPP
#define Fw_Port_test_ut_span_SpanTestReceiver_HPP

#include <Fw/Port/test/ut/span/SpanTestReceiverComponentAc.hpp>

namespace Fw {

class SpanTestReceiver final : public SpanTestReceiverComponentBase {
  public:
    explicit SpanTestReceiver(const char* compName) : SpanTestReceiverComponentBase(compName) {}

    void asyncIn_handler(FwIndexType, U32) override {}
    U32 syncIn_handler(FwIndexType, U32 arg) override { return arg; }
};

}  // namespace Fw

#endif
