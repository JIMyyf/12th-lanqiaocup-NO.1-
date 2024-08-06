# components.cmake

# component ARM::CMSIS:CORE@5.6.0
add_library(ARM_CMSIS_CORE_5_6_0 INTERFACE)
target_include_directories(ARM_CMSIS_CORE_5_6_0 INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_INCLUDE_DIRECTORIES>
  ${CMSIS_PACK_ROOT}/ARM/CMSIS/5.9.0/CMSIS/Core/Include
)
target_compile_definitions(ARM_CMSIS_CORE_5_6_0 INTERFACE
  $<TARGET_PROPERTY:${CONTEXT},INTERFACE_COMPILE_DEFINITIONS>
)
