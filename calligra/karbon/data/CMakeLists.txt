install( FILES  simple.kgr allcolors.kgr  DESTINATION ${DATA_INSTALL_DIR}/karbon/gradients)
install( FILES  palettes/svg-colors.gpl DESTINATION ${DATA_INSTALL_DIR}/karbon/palettes)
install( FILES  karbon_viewplugin.desktop  DESTINATION ${SERVICETYPES_INSTALL_DIR})
install( FILES  karbonpart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  karbon.rc karbon_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/karbon)
install( PROGRAMS  karbon.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES  karbonrc DESTINATION ${CONFIG_INSTALL_DIR} )

