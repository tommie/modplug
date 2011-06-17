libfind_pkg_check_modules(modplug_PKGCONF libmodplug)
find_path(modplug_INCLUDE_DIR
	NAMES modplug.h
	PATHS ${modplug_PKGCONF_INCLUDE_DIRS})
find_library(modplug_LIBRARIES
	NAMES modplug
	PATHS ${modplug_PKGCONF_LIBRARY_DIRS})
set(modplug_PROCESS_INCLUDES modplug_INCLUDE_DIR)
set(modplug_PROCESS_LIBS modplug_LIBRARIES)
libfind_process(modplug)
