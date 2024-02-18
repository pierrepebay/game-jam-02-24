set(
    TOP_LEVEL_SUBDIRS
    api
    objects
)

file(
    GLOB
    HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

file(
    GLOB
    SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

foreach(SUB_DIR ${TOP_LEVEL_SUBDIRS})
    file(
        GLOB
        "${SUB_DIR}_HEADER_FILES"
        RELATIVE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/${SUB_DIR}/*.h"
    )

    file(
        GLOB
        "${SUB_DIR}_SOURCE_FILES"
        RELATIVE
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/${SUB_DIR}/*.cpp"
    )

    message("headers=${${SUB_DIR}_HEADER_FILES}")
    message("source=${${SUB_DIR}_SOURCE_FILES}")

    list(
        APPEND
        HEADER_FILES
        "${${SUB_DIR}_HEADER_FILES}"
    )

    list(
        APPEND
        SOURCE_FILES
        "${${SUB_DIR}_SOURCE_FILES}"
    )
endforeach()

add_library(
    ${JAM_LIB}
    STATIC
    ${HEADER_FILES} ${SOURCE_FILES}
)