# Install script for directory: /home/wh/mysql-5.6.14/include

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local/mysql")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

# Install shared libraries without execute permission?
IF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  SET(CMAKE_INSTALL_SO_NO_EXE "0")
ENDIF(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/wh/mysql-5.6.14/include/mysql.h"
    "/home/wh/mysql-5.6.14/include/mysql_com.h"
    "/home/wh/mysql-5.6.14/include/mysql_time.h"
    "/home/wh/mysql-5.6.14/include/my_list.h"
    "/home/wh/mysql-5.6.14/include/my_alloc.h"
    "/home/wh/mysql-5.6.14/include/typelib.h"
    "/home/wh/mysql-5.6.14/include/mysql/plugin.h"
    "/home/wh/mysql-5.6.14/include/mysql/plugin_audit.h"
    "/home/wh/mysql-5.6.14/include/mysql/plugin_ftparser.h"
    "/home/wh/mysql-5.6.14/include/mysql/plugin_validate_password.h"
    "/home/wh/mysql-5.6.14/include/my_dbug.h"
    "/home/wh/mysql-5.6.14/include/m_string.h"
    "/home/wh/mysql-5.6.14/include/my_sys.h"
    "/home/wh/mysql-5.6.14/include/my_xml.h"
    "/home/wh/mysql-5.6.14/include/mysql_embed.h"
    "/home/wh/mysql-5.6.14/include/my_pthread.h"
    "/home/wh/mysql-5.6.14/include/decimal.h"
    "/home/wh/mysql-5.6.14/include/errmsg.h"
    "/home/wh/mysql-5.6.14/include/my_global.h"
    "/home/wh/mysql-5.6.14/include/my_net.h"
    "/home/wh/mysql-5.6.14/include/my_getopt.h"
    "/home/wh/mysql-5.6.14/include/sslopt-longopts.h"
    "/home/wh/mysql-5.6.14/include/my_dir.h"
    "/home/wh/mysql-5.6.14/include/sslopt-vars.h"
    "/home/wh/mysql-5.6.14/include/sslopt-case.h"
    "/home/wh/mysql-5.6.14/include/sql_common.h"
    "/home/wh/mysql-5.6.14/include/keycache.h"
    "/home/wh/mysql-5.6.14/include/m_ctype.h"
    "/home/wh/mysql-5.6.14/include/my_attribute.h"
    "/home/wh/mysql-5.6.14/include/my_compiler.h"
    "/home/wh/mysql-5.6.14/include/mysql_com_server.h"
    "/home/wh/mysql-5.6.14/include/my_byteorder.h"
    "/home/wh/mysql-5.6.14/include/byte_order_generic.h"
    "/home/wh/mysql-5.6.14/include/byte_order_generic_x86.h"
    "/home/wh/mysql-5.6.14/include/byte_order_generic_x86_64.h"
    "/home/wh/mysql-5.6.14/include/little_endian.h"
    "/home/wh/mysql-5.6.14/include/big_endian.h"
    "/home/wh/mysql-5.6.14/include/mysql_version.h"
    "/home/wh/mysql-5.6.14/include/my_config.h"
    "/home/wh/mysql-5.6.14/include/mysqld_ername.h"
    "/home/wh/mysql-5.6.14/include/mysqld_error.h"
    "/home/wh/mysql-5.6.14/include/sql_state.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/mysql" TYPE DIRECTORY FILES "/home/wh/mysql-5.6.14/include/mysql/" REGEX "/[^/]*\\.h$" REGEX "/psi\\_abi[^/]*$" EXCLUDE)
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" STREQUAL "Development")

