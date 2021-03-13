# - Find mysqlclient
# Find the native MySQL includes and library
#
#  MYSQL_INCLUDE   - where to find mysql.h, etc.
#  MYSQL_LIBRARY   - List of libraries when using MySQL.
#  CPPCONN_INCLUDE - where to find connection.h, etc.
#  CPPCONN_LIBRARY - List of libraries when using CppConn.
#  MYSQL_FOUND     - True if MySQL found.
#  CPPCONN_FOUND   - True if CppConn found.

FIND_PATH(MYSQL_INCLUDE mysql.h
  /usr/local/include/mysql
  /usr/include/mysql
)
FIND_LIBRARY(MYSQL_LIBRARY
  NAMES mysqlclient
  PATHS /usr/lib /usr/local/lib 
    /usr/lib/x86_64-linux-gnu /usr/lib/i386-linux-gnu/
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MYSQL DEFAULT_MSG
MYSQL_INCLUDE MYSQL_LIBRARY)



FIND_PATH(CPPCONN_INCLUDE connection.h
  /usr/local/include/cppconn
  /usr/include/cppconn
)
FIND_LIBRARY(CPPCONN_LIBRARY
  NAMES mysqlcppconn
  PATHS /usr/lib /usr/local/lib 
    /usr/lib/x86_64-linux-gnu /usr/lib/i386-linux-gnu/
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CPPCONN DEFAULT_MSG
CPPCONN_INCLUDE CPPCONN_LIBRARY)

MARK_AS_ADVANCED(
  MYSQL_LIBRARY
  MYSQL_INCLUDE
  CPPCONN_LIBRARY
  CPPCONN_INCLUDE
  )
