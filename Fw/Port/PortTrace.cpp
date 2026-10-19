#include <Fw/Port/PortTrace.hpp>

namespace Fw {

PortTraceObserver* PortTrace::s_observer = nullptr;

void PortTrace::setObserver(PortTraceObserver* observer) {
    PortTrace::s_observer = observer;
}

void PortTrace::callBegin(const PassiveComponentBase& caller,
                          const char* port,
                          FwIndexType portNum,
                          TraceContext& span) {
    if (PortTrace::s_observer != nullptr) {
        PortTrace::s_observer->callBegin(caller, port, portNum, span);
    } else {
        span = TraceContext{};
    }
}

void PortTrace::callEnd(const PassiveComponentBase& caller,
                        const char* port,
                        FwIndexType portNum,
                        const TraceContext& span) {
    if (PortTrace::s_observer != nullptr) {
        PortTrace::s_observer->callEnd(caller, port, portNum, span);
    }
}

void PortTrace::current(TraceContext& ctx) {
    if (PortTrace::s_observer != nullptr) {
        PortTrace::s_observer->current(ctx);
    } else {
        ctx = TraceContext{};
    }
}

void PortTrace::dispatchBegin(const PassiveComponentBase& comp,
                              const char* port,
                              FwIndexType portNum,
                              const TraceContext& cause,
                              TraceContext& span) {
    if (PortTrace::s_observer != nullptr) {
        PortTrace::s_observer->dispatchBegin(comp, port, portNum, cause, span);
    } else {
        span = TraceContext{};
    }
}

void PortTrace::dispatchEnd(const PassiveComponentBase& comp,
                            const char* port,
                            FwIndexType portNum,
                            const TraceContext& span) {
    if (PortTrace::s_observer != nullptr) {
        PortTrace::s_observer->dispatchEnd(comp, port, portNum, span);
    }
}

void PortTrace::dropped(const PassiveComponentBase& comp,
                        const char* port,
                        FwIndexType portNum,
                        const TraceContext& cause) {
    if (PortTrace::s_observer != nullptr) {
        PortTrace::s_observer->dropped(comp, port, portNum, cause);
    }
}

}  // namespace Fw
