
set(APP_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Profile/include
    PARENT_SCOPE
)

file(GLOB_RECURSE APP_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/Profile/*.c"
)

# 创建APP静态库
add_library(app_lib STATIC ${APP_SOURCES})

# 设置APP库的头文件路径
target_include_directories(app_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Profile/include
)

target_include_directories(app_lib PRIVATE ${SDK_INCLUDE_DIRS})
set(APP_SOURCES ${APP_SOURCES} PARENT_SCOPE) 