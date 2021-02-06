add_executable(pico_rng
        pico_rng.c
        )

target_link_libraries(pico_rng PRIVATE pico_stdlib hardware_resets hardware_irq)
pico_add_extra_outputs(pico_rng)
