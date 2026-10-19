module Fw {
    port SpanTestSync(arg: U32) -> U32
    port SpanTestAsync(arg: U32)

    passive component SpanTestSender {
        output port syncOut: SpanTestSync
        output port asyncOut: SpanTestAsync
    }

    active component SpanTestReceiver {
        sync input port syncIn: SpanTestSync
        async input port asyncIn: SpanTestAsync drop
    }
}
