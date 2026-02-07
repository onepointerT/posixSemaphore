
cmake_minimum_required( VERSION 3.22 )

project( posixSemaphore C CXX )

set( LIBPSEMAPHORE_STABLE_VERSION 0 )
set( LIBPSEMAPHORE_RELEASE 0 )
set( LIBPSEMAPHORE_PATCH_VERSION 1 )
set( LIBPSEMAPHORE_VERSION "${LIBPSEMAPHORE_STABLE_VERSION}.${LIBPSEMAPHORE_RELEASE}.${LIBPSEMAPHORE_PATCH_VERSION}" )

set( LIBPSEMAPHORE_SRC_DIR ${PROJECT_SOURCE_DIR}/src )

set ( PROJECT_NAME "${CMAKE_PROJECT_NAME}" )
set ( PROJECT_VERSION "${LIBPSEMAPHORE_VERSION}" )
set ( PROJECT_DESCRIPTION "A advanced POSIX-semaphore for C. In advance to normal semaphore, the handle can be locked and queued." )


set ( CMAKE_C_STANDARD 23 )
set ( CMAKE_CXX_STANDARD 20 )


if( PSEMAPHORE_DESTDIR )
else()
	set( PSEMAPHORE_DESTDIR "${CMAKE_INSTALL_PREFIX}" )
endif()


add_subdirectory( lib )
include_directories( lib/pthreads )


include_directories( ${PROJECT_SOURCE_DIR} )
file ( GLOB LIBPSEMAPHORE_SOURCE ${PROJECT_SOURCE_DIR}/*.c )
if( PSEMAPHORE_SHARED )
	add_library( psem SHARED ${LIBPSEMAPHORE_SOURCE} )
else()
	add_library( psem STATIC ${LIBPSEMAPHORE_SOURCE} )
endif()
target_link_libraries( psem pthread m )


file ( GLOB LIBPSEMAPHORE_SOURCE_CPP ${PROJECT_SOURCE_DIR}/*.cpp )
if( PSEMAPHORE_SHARED )
	add_library( psemcpp ${LIBPSEMAPHORE_SOURCE_CPP} )
else()
	add_library( psemcpp STATIC ${LIBPSEMAPHORE_SOURCE_CPP} )
endif()
target_link_libraries( psemcpp psem )
