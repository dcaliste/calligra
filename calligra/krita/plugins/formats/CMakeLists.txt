project(kritafilters)

if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  add_definitions( -DCPU_32_BITS )
endif(CMAKE_SIZEOF_VOID_P EQUAL 4)

macro_optional_find_package(PNG)
macro_log_feature(PNG_FOUND "png" "Official PNG reference library" "http://www.libpng.org/pub/png/libpng.html" FALSE "" "Required by the Krita PNG filter")

macro_optional_find_package(OpenEXR)
macro_log_feature(OPENEXR_FOUND "OpenEXR" "High dynamic-range (HDR) image file format" "http://www.openexr.com" FALSE "" "Required by the Krita OpenEXR filter")

macro_optional_find_package(TIFF)
macro_log_feature(TIFF_FOUND "tiff" "TIFF Library and Utilities" "http://www.remotesensing.org/libtiff" FALSE "" "Required by the Krita TIFF filter")

macro_optional_find_package(JPEG)
macro_log_feature(JPEG_FOUND "jpeg" "Free library for JPEG image compression. Note: libjpeg8 is NOT supported." "http://www.libjpeg-turbo.org" FALSE "" "Required by the Krita JPEG filter")

macro_optional_find_package(Kdcraw)
macro_log_feature( KDCRAW_FOUND "kdcraw" "The KDCraw library" "http://www.digikam.org/" FALSE "" "Required by the Krita RAW filter")

macro_optional_find_package(OpenJPEG)
macro_log_feature(OPENJPEG_FOUND "openjpeg" "Free library for JPEG 2000 image compression" "http://www.openjpeg.org" FALSE "" "Required by the Krita JPEG 2000 filter")

if(KDCRAW_FOUND)
    add_subdirectory( raw )
endif(KDCRAW_FOUND)

if(JPEG_FOUND AND HAVE_LCMS2)
	add_subdirectory(jpeg)
endif(JPEG_FOUND AND HAVE_LCMS2)

if(TIFF_FOUND)
	add_subdirectory(tiff)
endif(TIFF_FOUND)

if(PNG_FOUND)
    add_subdirectory(png)
endif(PNG_FOUND)

if(OPENEXR_FOUND)
    add_subdirectory(exr)
endif(OPENEXR_FOUND)

if(POPPLER_FOUND)
    add_subdirectory(pdf)
endif(POPPLER_FOUND)

if(OPENJPEG_FOUND)
    add_subdirectory(jp2)
endif(OPENJPEG_FOUND)

add_subdirectory(bmp)
add_subdirectory(ora)
add_subdirectory(ppm)
add_subdirectory(xcf)
add_subdirectory(psd)
add_subdirectory(odg)
add_subdirectory(flipbook)
add_subdirectory(qml)

#if(OIIO_FOUND)
#    add_subdirectory(oiio)
#endif(OIIO_FOUND)
