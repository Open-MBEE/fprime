#include <Fw/Port/PortTrace.hpp>

namespace Fw {

PortTraceObserver* PortTrace::s_observer = nullptr;

void PortTrace::setObserver(PortTraceObserver* observer) {
    PortTrace::s_observer = observer;
}

void PortTrace::callBegin(const PassiveComponentBase& caller,
                          FwIndexType portId,
                          FwIndexType portNum,
                          TraceContext& span) {
    if (PortTrace::s_observer != nullptr) {
        PortTrace::s_observer->callBegin(caller, portId, portNum, span);
    } else {
        span = TraceContext{};
    }
}

void PortTrace::callEnd(const PassiveComponentBase& caller,
                        FwIndexType portId,
                        FwIndexType portNum,
                        const TraceContext& span) {
    if (PortTrace::s_observer != nullptr) {
        PortTrace::s_observer->callEnd(caller, portId, portNum, span);
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
                              FwIndexType portId,
                              FwIndexType portNum,
                              const TraceContext& cause,
                              TraceContext& span) {
    if (PortTrace::s_observer != nullptr) {
        PortTrace::s_observer->dispatchBegin(comp, portId, portNum, cause, span);
    } else {
        span = TraceContext{};
    }
}

void PortTrace::dispatchEnd(const PassiveComponentBase& comp,
                            FwIndexType portId,
                            FwIndexType portNum,
                            const TraceContext& span) {
    if (PortTrace::s_observer != nullptr) {
        PortTrace::s_observer->dispatchEnd(comp, portId, portNum, span);
    }
}

void PortTrace::dropped(const PassiveComponentBase& comp,
                        FwIndexType portId,
                        FwIndexType portNum,
                        const TraceContext& cause) {
    if (PortTrace::s_observer != nullptr) {
        PortTrace::s_observer->dropped(comp, portId, portNum, cause);
    }
}

}  // namespace Fw
