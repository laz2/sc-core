
cmake_minimum_required(VERSION 2.6)

set(BINARY_OUTPUT_DIR  ${CMAKE_CURRENT_BINARY_DIR})
set(DOXYGEN_OUTPUT_DIR ${BINARY_OUTPUT_DIR}/doc)
set(DOXYGEN_TAGFILE    ${DOXYGEN_OUTPUT_DIR}/sc-core.tag)

# Generation of doxygen config file
set(DOXYFILE ${BINARY_OUTPUT_DIR}/sc-core.doxyfile)

set(GENERATE_HTMLHELP no)
set(BINARY_TOC no)
set(TOC_EXPAND no)
set(IMAGES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/images)

if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
    find_program(HHC hhc PATHS "c:/Program Files/HTML Help Workshop")
    if(HHC)
        set(GENERATE_HTMLHELP yes)
        set(BINARY_TOC yes)
        set(TOC_EXPAND yes)
        set(HHC_LOCATION ${HHC})
    endif()
endif()

configure_file(sc-core.doxyfile.in ${DOXYFILE})
find_program(DOXYGEN doxygen)

add_custom_target(documentation ALL
    COMMAND ${DOXYGEN} ${DOXYFILE} DEPENDS ${DOXYFILE}
)

install(DIRECTORY ${BINARY_OUTPUT_DIR}/doc/html
    DESTINATION doc/sc-core
    PATTERN "*.hhc" EXCLUDE 
    PATTERN "*.hhk" EXCLUDE
    PATTERN "*.hhp" EXCLUDE
    PATTERN "*.chm" EXCLUDE
    PATTERN "*.tmp" EXCLUDE
    PATTERN "*.chm" EXCLUDE
)

if(GENERATE_HTMLHELP)
    file(GLOB files ${BINARY_OUTPUT_DIR}/doc/html/*.chm)  
    install(FILES ${files}
        DESTINATION doc/sc-core
    )    
endif()
    