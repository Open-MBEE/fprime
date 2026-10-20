#ifndef FW_PORT_PORTTRACE_HPP
#define FW_PORT_PORTTRACE_HPP

#include <Fw/FPrimeBasicTypes.hpp>

namespace Fw {

class PassiveComponentBase;

struct TraceContext {
    U8 traceId[16];
    U8 spanId[8];
    U8 flags;
};

class PortTraceObserver {
  public:
    virtual ~PortTraceObserver() = default;
    virtual void callBegin(const PassiveComponentBase& caller,
                           FwIndexType portId,
                           FwIndexType portNum,
                           TraceContext& span) = 0;
    virtual void callEnd(const PassiveComponentBase& caller,
                         FwIndexType portId,
                         FwIndexType portNum,
                         const TraceContext& span) = 0;
    virtual void current(TraceContext& ctx) = 0;
    virtual void dispatchBegin(const PassiveComponentBase& comp,
                               FwIndexType portId,
                               FwIndexType portNum,
                               const TraceContext& cause,
                               TraceContext& span) = 0;
    virtual void dispatchEnd(const PassiveComponentBase& comp,
                             FwIndexType portId,
                             FwIndexType portNum,
                             const TraceContext& span) = 0;
    virtual void dropped(const PassiveComponentBase& comp,
                         FwIndexType portId,
                         FwIndexType portNum,
                         const TraceContext& cause) = 0;
};

class PortTrace {
  public:
    //! Set before port-invoking threads start; the observer owns per-thread span state.
    static void setObserver(PortTraceObserver* observer);
    static void callBegin(const PassiveComponentBase& caller,
                          FwIndexType portId,
                          FwIndexType portNum,
                          TraceContext& span);
    static void callEnd(const PassiveComponentBase& caller,
                        FwIndexType portId,
                        FwIndexType portNum,
                        const TraceContext& span);
    static void current(TraceContext& ctx);
    static void dispatchBegin(const PassiveComponentBase& comp,
                              FwIndexType portId,
                              FwIndexType portNum,
                              const TraceContext& cause,
                              TraceContext& span);
    static void dispatchEnd(const PassiveComponentBase& comp,
                            FwIndexType portId,
                            FwIndexType portNum,
                            const TraceContext& span);
    static void dropped(const PassiveComponentBase& comp,
                        FwIndexType portId,
                        FwIndexType portNum,
                        const TraceContext& cause);

  private:
    static PortTraceObserver* s_observer;
};

}  // namespace Fw

#endif
