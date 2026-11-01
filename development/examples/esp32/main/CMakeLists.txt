idf_component_register(
    SRCS ${APP_SOURCE_FILE}
    INCLUDE_DIRS "."
    REQUIRES hf_pf1550 esp_driver_gpio driver freertos
)

target_compile_features(${COMPONENT_LIB} PRIVATE cxx_std_20)
