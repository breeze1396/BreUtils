### 加密需要依赖opnssl

```
# openssl
set(OPENSSL_DIR_PATH "your/path/openssl")
include_directories(${OPENSSL_DIR_PATH}/include)
link_directories(${OPENSSL_DIR_PATH}/lib)
#库文件
set(OPENSSL_LIB ssl crypto)
foreach(lib ${OPENSSL_LIB})
    find_library(${lib}_LIB ${lib} PATHS ${OPENSSL_DIR_PATH}/lib NO_DEFAULT_PATH)
    if (${lib}_LIB)
        message(STATUS "Found library: ${lib} at ${${lib}_LIB}")
        list(APPEND OPENSSL_LIBS ${${lib}_LIB})
    else()
        message(FATAL_ERROR "Failed to find library: ${lib}")
    endif()
endforeach()

target_link_libraries(${PROJECT_NAME} 
    PRIVATE    
        ${OPENSSL_LIBS}
)
```