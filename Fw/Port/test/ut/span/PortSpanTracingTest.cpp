#include <gtest/gtest.h>

#include <Fw/Port/PortTrace.hpp>
#include <Fw/Port/test/ut/span/SpanTestReceiverComponentAc.hpp>
#include <Fw/Port/test/ut/span/SpanTestSenderComponentAc.hpp>

namespace {

enum class EventKind { CALL_BEGIN, CALL_END, DISPATCH_BEGIN, DISPATCH_END, DROPPED, HANDLER };

struct Event {
    EventKind kind;
    const char* port;
    FwIndexType portNum;
    Fw::TraceContext span;
    Fw::TraceContext cause;
};

class RecordingObserver final : public Fw::PortTraceObserver {
  public:
    RecordingObserver() : m_nextId(0), m_depth(0), m_eventCount(0) {}

    void callBegin(const Fw::PassiveComponentBase&,
                   const char* port,
                   FwIndexType portNum,
                   Fw::TraceContext& span) override {
        this->assignSpan(span);
        this->push(span);
        this->record(EventKind::CALL_BEGIN, port, portNum, &span, nullptr);
    }

    void callEnd(const Fw::PassiveComponentBase&,
                 const char* port,
                 FwIndexType portNum,
                 const Fw::TraceContext& span) override {
        this->record(EventKind::CALL_END, port, portNum, &span, nullptr);
        this->pop();
    }

    void current(Fw::TraceContext& ctx) override {
        ctx = this->m_depth == 0 ? Fw::TraceContext{} : this->m_stack[this->m_depth - 1];
    }

    void dispatchBegin(const Fw::PassiveComponentBase&,
                       const char* port,
                       FwIndexType portNum,
                       const Fw::TraceContext& cause,
                       Fw::TraceContext& span) override {
        this->assignSpan(span);
        this->push(span);
        this->record(EventKind::DISPATCH_BEGIN, port, portNum, &span, &cause);
    }

    void dispatchEnd(const Fw::PassiveComponentBase&,
                     const char* port,
                     FwIndexType portNum,
                     const Fw::TraceContext& span) override {
        this->record(EventKind::DISPATCH_END, port, portNum, &span, nullptr);
        this->pop();
    }

    void dropped(const Fw::PassiveComponentBase&,
                 const char* port,
                 FwIndexType portNum,
                 const Fw::TraceContext& cause) override {
        this->record(EventKind::DROPPED, port, portNum, nullptr, &cause);
    }

    void noteHandler(const char* port) { this->record(EventKind::HANDLER, port, 0, nullptr, nullptr); }

    U32 eventCount() const { return this->m_eventCount; }
    const Event& event(U32 index) const { return this->m_events[index]; }

  private:
    static constexpr U32 EVENT_CAPACITY = 16;
    static constexpr U32 STACK_CAPACITY = 8;
    U8 m_nextId;
    U32 m_depth;
    U32 m_eventCount;
    Fw::TraceContext m_stack[STACK_CAPACITY];
    Event m_events[EVENT_CAPACITY];

    void assignSpan(Fw::TraceContext& span) {
        span = Fw::TraceContext{};
        span.traceId[0] = 1;
        span.spanId[7] = ++this->m_nextId;
        span.flags = 1;
    }

    void push(const Fw::TraceContext& span) {
        if (this->m_depth < STACK_CAPACITY) {
            this->m_stack[this->m_depth++] = span;
        }
    }

    void pop() {
        if (this->m_depth > 0) {
            --this->m_depth;
        }
    }

    void record(EventKind kind,
                const char* port,
                FwIndexType portNum,
                const Fw::TraceContext* span,
                const Fw::TraceContext* cause) {
        if (this->m_eventCount < EVENT_CAPACITY) {
            Event& event = this->m_events[this->m_eventCount++];
            event.kind = kind;
            event.port = port;
            event.portNum = portNum;
            event.span = span == nullptr ? Fw::TraceContext{} : *span;
            event.cause = cause == nullptr ? Fw::TraceContext{} : *cause;
        }
    }
};

class SpanTestSender final : public Fw::SpanTestSenderComponentBase {
  public:
    SpanTestSender() : Fw::SpanTestSenderComponentBase("sender") {}

    U32 invokeSync(FwIndexType portNum, U32 arg) { return this->syncOut_out(portNum, arg); }
    void invokeAsync(FwIndexType portNum, U32 arg) { this->asyncOut_out(portNum, arg); }
};

class SpanTestReceiver final : public Fw::SpanTestReceiverComponentBase {
  public:
    explicit SpanTestReceiver(RecordingObserver& observer)
        : Fw::SpanTestReceiverComponentBase("receiver"),
          m_observer(observer),
          m_syncHandlerRan(false),
          m_asyncHandlerCount(0) {}

    U32 syncIn_handler(FwIndexType, U32 arg) override {
        this->m_syncHandlerRan = true;
        if (this->m_recordHandlerEvents) {
            this->m_observer.noteHandler("syncIn");
        }
        return arg + 1;
    }

    void asyncIn_handler(FwIndexType, U32) override {
        ++this->m_asyncHandlerCount;
        if (this->m_recordHandlerEvents) {
            this->m_observer.noteHandler("asyncIn");
        }
    }

    void dispatchMessages() { (void)this->dispatchAvailableMessages(); }
    bool syncHandlerRan() const { return this->m_syncHandlerRan; }
    U32 asyncHandlerCount() const { return this->m_asyncHandlerCount; }
    void recordHandlerEvents(bool enabled) { this->m_recordHandlerEvents = enabled; }

  private:
    RecordingObserver& m_observer;
    bool m_syncHandlerRan;
    U32 m_asyncHandlerCount;
    bool m_recordHandlerEvents = true;
};

class PortSpanTracingTest : public ::testing::Test {
  protected:
    PortSpanTracingTest() : m_receiver(m_observer) {}

    void SetUp() override {
        Fw::PortTrace::setObserver(&this->m_observer);
        this->m_sender.init();
        this->m_receiver.init(1);
        this->m_sender.set_syncOut_OutputPort(0, this->m_receiver.get_syncIn_InputPort(0));
        this->m_sender.set_asyncOut_OutputPort(0, this->m_receiver.get_asyncIn_InputPort(0));
    }

    void TearDown() override {
        Fw::PortTrace::setObserver(nullptr);
        this->m_sender.deinit();
        this->m_receiver.deinit();
    }

    RecordingObserver m_observer;
    SpanTestSender m_sender;
    SpanTestReceiver m_receiver;
};

TEST_F(PortSpanTracingTest, SyncCallBracketsTheHandler) {
    EXPECT_EQ(42U, this->m_sender.invokeSync(0, 41));
    ASSERT_EQ(3U, this->m_observer.eventCount());
    EXPECT_EQ(EventKind::CALL_BEGIN, this->m_observer.event(0).kind);
    EXPECT_STREQ("syncOut", this->m_observer.event(0).port);
    EXPECT_EQ(0, this->m_observer.event(0).portNum);
    EXPECT_EQ(EventKind::HANDLER, this->m_observer.event(1).kind);
    EXPECT_EQ(EventKind::CALL_END, this->m_observer.event(2).kind);
    EXPECT_EQ(this->m_observer.event(0).span.spanId[7], this->m_observer.event(2).span.spanId[7]);
    EXPECT_TRUE(this->m_receiver.syncHandlerRan());
}

TEST_F(PortSpanTracingTest, AsyncCausePropagatesThroughDispatch) {
    this->m_sender.invokeAsync(0, 17);
    this->m_receiver.dispatchMessages();

    ASSERT_EQ(5U, this->m_observer.eventCount());
    EXPECT_EQ(EventKind::CALL_BEGIN, this->m_observer.event(0).kind);
    EXPECT_EQ(EventKind::CALL_END, this->m_observer.event(1).kind);
    EXPECT_EQ(EventKind::DISPATCH_BEGIN, this->m_observer.event(2).kind);
    EXPECT_STREQ("asyncIn", this->m_observer.event(2).port);
    EXPECT_EQ(0, this->m_observer.event(2).portNum);
    EXPECT_EQ(this->m_observer.event(0).span.spanId[7], this->m_observer.event(2).cause.spanId[7]);
    EXPECT_EQ(EventKind::HANDLER, this->m_observer.event(3).kind);
    EXPECT_EQ(EventKind::DISPATCH_END, this->m_observer.event(4).kind);
    EXPECT_EQ(1U, this->m_receiver.asyncHandlerCount());
}

TEST_F(PortSpanTracingTest, QueueDropReportsTheActiveCause) {
    this->m_sender.invokeAsync(0, 17);
    this->m_sender.invokeAsync(0, 18);

    ASSERT_EQ(5U, this->m_observer.eventCount());
    EXPECT_EQ(EventKind::DROPPED, this->m_observer.event(3).kind);
    EXPECT_STREQ("asyncIn", this->m_observer.event(3).port);
    EXPECT_EQ(0, this->m_observer.event(3).portNum);
    EXPECT_EQ(this->m_observer.event(2).span.spanId[7], this->m_observer.event(3).cause.spanId[7]);
    this->m_receiver.dispatchMessages();
    EXPECT_EQ(1U, this->m_receiver.asyncHandlerCount());
}

TEST_F(PortSpanTracingTest, CallsWorkWithoutAnObserver) {
    this->m_receiver.recordHandlerEvents(false);
    Fw::PortTrace::setObserver(nullptr);

    EXPECT_EQ(42U, this->m_sender.invokeSync(0, 41));
    this->m_sender.invokeAsync(0, 17);
    this->m_receiver.dispatchMessages();

    EXPECT_EQ(0U, this->m_observer.eventCount());
    EXPECT_TRUE(this->m_receiver.syncHandlerRan());
    EXPECT_EQ(1U, this->m_receiver.asyncHandlerCount());
}

}  // namespace
