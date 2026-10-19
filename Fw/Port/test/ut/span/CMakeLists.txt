register_fprime_module(
  Fw_Port_PortSpanTracingTestModel
  AUTOCODER_INPUTS
    "${CMAKE_CURRENT_LIST_DIR}/PortSpanTracing.fpp"
  DEPENDS
    Fw_Port
    Fw_CompQueued
)
target_compile_definitions(Fw_Port_PortSpanTracingTestModel PRIVATE FW_PORT_SPAN_TRACING=1)

register_fprime_ut(
  Fw_Port_PortSpanTracing_ut_exe
  SOURCES
    "${CMAKE_CURRENT_LIST_DIR}/PortSpanTracingTest.cpp"
  DEPENDS
    Fw_Port
    Fw_CompQueued
    Fw_Port_PortSpanTracingTestModel
)
target_compile_definitions(Fw_Port_PortSpanTracing_ut_exe PRIVATE FW_PORT_SPAN_TRACING=1)
