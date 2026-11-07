set( ProxySwitcher_SRCS
	proxyswitcherplugin.cpp
	ps_manager.cpp
	ps_profile.cpp
	ps_controlbutton.cpp
	ps_managerdialog.cpp
	ps_editdialog.cpp

        proxyswitcherplugin.h
        ps_manager.h
        ps_profile.h
        ps_controlbutton.h
        ps_managerdialog.h
        ps_editdialog.h
    )

set( ProxySwitcher_RES
	proxyswitcher.json
    )

add_library(ProxySwitcher MODULE ${ProxySwitcher_SRCS} ${ProxySwitcher_RES})
install(TARGETS ProxySwitcher DESTINATION ${FALKON_INSTALL_PLUGINDIR})
target_link_libraries(ProxySwitcher FalkonPrivate)
