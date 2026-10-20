#ifndef Fw_Port_test_ut_span_SpanTestSender_HPP
#define Fw_Port_test_ut_span_SpanTestSender_HPP

#include <Fw/Port/test/ut/span/SpanTestSenderComponentAc.hpp>

namespace Fw {

class SpanTestSender final : public SpanTestSenderComponentBase {
  public:
    explicit SpanTestSender(const char* compName) : SpanTestSenderComponentBase(compName) {}
};

}  // namespace Fw

#endif
