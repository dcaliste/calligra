if(SHOULD_BUILD_FILTER_ASCII_TO_WORDS)

include_directories(
    ${CMAKE_SOURCE_DIR}/words/part
)


set(ascii2words_PART_SRCS
    AsciiImport.cpp
    ImportDialog.cc
)
kde4_add_ui_files(ascii2words_PART_SRCS ImportDialogUI.ui )
kde4_add_plugin(calligra_filter_ascii2words ${ascii2words_PART_SRCS})
target_link_libraries(calligra_filter_ascii2words komain kotextlayout wordsprivate KCodecs)
install(TARGETS calligra_filter_ascii2words DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES calligra_filter_ascii2words.desktop DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_ASCII_TO_WORDS)


if(SHOULD_BUILD_FILTER_ODT_TO_ASCII)

include_directories(
    ${CMAKE_SOURCE_DIR}/filters/libodfreader
    ${KOODF2_INCLUDES}
)

set(odt2ascii_PART_SRCS
    AsciiExport.cpp
    OdtReaderAsciiBackend.cpp
    OdfReaderAsciiContext.cpp
)
kde4_add_plugin(calligra_filter_odt2ascii ${odt2ascii_PART_SRCS})
target_link_libraries(calligra_filter_odt2ascii koodfreader komain koodf koodf2)
install(TARGETS calligra_filter_odt2ascii DESTINATION ${PLUGIN_INSTALL_DIR})

install( FILES calligra_filter_odt2ascii.desktop DESTINATION ${SERVICES_INSTALL_DIR})

endif(SHOULD_BUILD_FILTER_ODT_TO_ASCII)
