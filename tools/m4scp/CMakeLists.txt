
project(m4scp)

cmake_minimum_required(VERSION 2.6)

install(FILES m4scp.m4
	DESTINATION share/sc-core/
)

if(WIN32)
	install(FILES m4.exe DESTINATION bin)
	install(FILES regex2.dll DESTINATION bin)
	install(FILES m4scp2scs.bat DESTINATION bin)
	install(FILES m4scp2tgf.bat DESTINATION bin)
endif()
