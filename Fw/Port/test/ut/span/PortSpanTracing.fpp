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

    instance spanSender: SpanTestSender base id 0x100
    instance spanReceiver: SpanTestReceiver base id 0x200 \
        queue size 1 \
        stack size 1024

    deployment topology PortSpanTracing {
        instance spanSender
        instance spanReceiver

        connections C {
            spanSender.syncOut -> spanReceiver.syncIn
            spanSender.asyncOut -> spanReceiver.asyncIn
        }
    }
}
