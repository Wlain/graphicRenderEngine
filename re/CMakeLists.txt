file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
        re/*.cpp
        ${PROJECT_SOURCE_DIR}/re/*.h
        ${PROJECT_SOURCE_DIR}/re/utils/*.h
        ${PROJECT_SOURCE_DIR}/3rdparty/*.h)

add_library(re SHARED ${SOURCE_FILES})

target_link_libraries(re glfw glm spdlog ${EXTRA_LIBS})

install(TARGETS re DESTINATION lib)

