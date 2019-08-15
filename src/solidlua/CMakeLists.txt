project (solidlua C CXX)

include_directories(
	./
	../../3rdparty
)

set(SOURCES
		stdafx.cpp
		stdafx.h
		include_solid.h
		include_lua.h
		main.cpp

		10.0.9638/solidsdk/SolidFramework.h
		10.0.9638/solidsdk/SolidFramework.cpp
		10.0.9638/glue/glue.h
		10.0.9638/glue/glue.cpp
	)
source_group_by_dir(SOURCES)

add_executable(${PROJECT_NAME}
		${SOURCES}
	)

add_definitions(-DUNICODE -D_UNICODE -DUSE_PRESET_SOLID_CODE)

target_link_libraries(${PROJECT_NAME}
	lua
	)