libfind_pkg_check_modules(zip_PKGCONF libzip)
find_path(zip_INCLUDE_DIR
	NAMES zip.h
	PATHS ${zip_PKGCONF_INCLUDE_DIRS})
find_library(zip_LIBRARIES
	NAMES zip
	PATHS ${zip_PKGCONF_LIBRARY_DIRS})
set(zip_PROCESS_INCLUDES zip_INCLUDE_DIR)
set(zip_PROCESS_LIBS zip_LIBRARIES)
libfind_process(zip)
