add_executable(pico_rng
        pico_rng.c
        )

target_link_libraries(pico_rng PRIVATE pico_stdlib hardware_resets hardware_irq hardware_adc)

pico_enable_stdio_uart(pico_rng 1)
pico_add_extra_outputs(pico_rng)
