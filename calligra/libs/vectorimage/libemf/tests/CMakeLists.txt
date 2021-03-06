INCLUDE_DIRECTORIES(
	${ENHMETAFILE_SOURCE_DIR}/tests/
	${CMAKE_CURRENT_BINARY_DIR}
	${QT_INCLUDES}
)

LINK_DIRECTORIES(${ENHMETAFILE_BINARY_DIR})

SET( snp_tests_bin_SRCS snp_tests.cpp )  
QT4_AUTOMOC( ${snp_tests_bin_SRCS} )
ADD_EXECUTABLE( snp_tests ${snp_tests_bin_SRCS} )
TARGET_LINK_LIBRARIES( snp_tests ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} EnhMetaFile )
ADD_TEST( SnpTests ${CMAKE_CURRENT_BINARY_DIR}/snp_tests )
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/snp-1.emf ${CMAKE_CURRENT_BINARY_DIR}/snp-1.emf COPYONLY )
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/snp-2.emf ${CMAKE_CURRENT_BINARY_DIR}/snp-2.emf COPYONLY )
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/snp-3.emf ${CMAKE_CURRENT_BINARY_DIR}/snp-3.emf COPYONLY )

SET( no_such_bin_SRCS no_such.cpp )  
QT4_AUTOMOC( ${no_such_bin_SRCS} )
ADD_EXECUTABLE( no_such_file ${no_such_bin_SRCS} )
TARGET_LINK_LIBRARIES( no_such_file ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} EnhMetaFile )
ADD_TEST( NoSuchFile ${CMAKE_CURRENT_BINARY_DIR}/no_such_file )

SET( bad_header_bin_SRCS bad_header.cpp )  
QT4_AUTOMOC( ${bad_header_bin_SRCS} )
ADD_EXECUTABLE( bad_header ${bad_header_bin_SRCS} )
TARGET_LINK_LIBRARIES( bad_header ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} EnhMetaFile )
ADD_TEST( BadHeader ${CMAKE_CURRENT_BINARY_DIR}/bad_header )
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/cof.wmf ${CMAKE_CURRENT_BINARY_DIR}/cof.wmf COPYONLY)

SET( pyemf_tests_bin_SRCS pyemf_tests.cpp )  
QT4_AUTOMOC( ${pyemf_tests_bin_SRCS} )
ADD_EXECUTABLE( pyemf_tests ${pyemf_tests_bin_SRCS} )
TARGET_LINK_LIBRARIES( pyemf_tests ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} EnhMetaFile )
ADD_TEST(PyEmfTests ${CMAKE_CURRENT_BINARY_DIR}/pyemf_tests)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pyemf-1.emf ${CMAKE_CURRENT_BINARY_DIR}/pyemf-1.emf COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pyemf-arc-chord-pie.emf ${CMAKE_CURRENT_BINARY_DIR}/pyemf-arc-chord-pie.emf COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pyemf-deleteobject.emf ${CMAKE_CURRENT_BINARY_DIR}/pyemf-deleteobject.emf COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pyemf-drawing1.emf ${CMAKE_CURRENT_BINARY_DIR}/pyemf-drawing1.emf COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pyemf-fontbackground.emf ${CMAKE_CURRENT_BINARY_DIR}/pyemf-fontbackground.emf COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pyemf-optimize16bit.emf ${CMAKE_CURRENT_BINARY_DIR}/pyemf-optimize16bit.emf COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pyemf-paths1.emf ${CMAKE_CURRENT_BINARY_DIR}/pyemf-paths1.emf COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pyemf-poly1.emf ${CMAKE_CURRENT_BINARY_DIR}/pyemf-poly1.emf COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pyemf-poly2.emf ${CMAKE_CURRENT_BINARY_DIR}/pyemf-poly2.emf COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pyemf-selectclippath1.emf ${CMAKE_CURRENT_BINARY_DIR}/pyemf-selectclippath1.emf COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pyemf-setpixel.emf ${CMAKE_CURRENT_BINARY_DIR}/pyemf-setpixel.emf COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pyemf-viewport-window-origin.emf ${CMAKE_CURRENT_BINARY_DIR}/pyemf-viewport-window-origin.emf COPYONLY)
CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/pyemf-worldtransform1.emf ${CMAKE_CURRENT_BINARY_DIR}/pyemf-worldtransform1.emf COPYONLY)

SET( visio_tests_bin_SRCS visio_tests.cpp )  
QT4_AUTOMOC( ${visio_tests_bin_SRCS} )
ADD_EXECUTABLE( visio_tests ${visio_tests_bin_SRCS} )
TARGET_LINK_LIBRARIES( visio_tests ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY} EnhMetaFile )
ADD_TEST( VisioTests ${CMAKE_CURRENT_BINARY_DIR}/visio_tests )
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/visio-1.emf ${CMAKE_CURRENT_BINARY_DIR}/visio-1.emf COPYONLY )
CONFIGURE_FILE( ${CMAKE_CURRENT_SOURCE_DIR}/visio-kde41.emf ${CMAKE_CURRENT_BINARY_DIR}/visio-kde41.emf COPYONLY )

SET( render_bin_SRCS render.cpp )  
QT4_AUTOMOC( ${render_bin_SRCS} )
ADD_EXECUTABLE( render ${render_bin_SRCS} )
TARGET_LINK_LIBRARIES( render ${QT_QTGUI_LIBRARY} EnhMetaFile )
ADD_TEST( QPainter-Render ${CMAKE_CURRENT_BINARY_DIR}/render )

