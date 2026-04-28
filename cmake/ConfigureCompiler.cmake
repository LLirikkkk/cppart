option(CPPART_SANITIZED "Should the build be sanitized" OFF)
option(CPPART_THREAD_SANITIZED "Should the build be thread sanitized" OFF)

function(cppart_configure_compiler TARGET)

    set(isGCC OFF)
    set(isClang OFF)
    set(isMSVC OFF)

    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        set(isGCC ON)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        set(isClang ON)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        set(isMSVC ON)
    endif()

    set(compilerOptions "")
    set(compilerDefinitions "")
    set(linkerOptions "")

    if(CPPART_SANITIZED)
        if(isGCC OR isClang)
            list(APPEND compilerOptions -fsanitize=undefined,address)
            list(APPEND linkerOptions -fsanitize=undefined,address)
            list(APPEND compilerOptions
                    -fno-sanitize-recover=all
                    -fno-optimize-sibling-calls
                    -fno-omit-frame-pointer
            )
            message(STATUS "Enabled UBSan and ASan")
        elseif(isMSVC)
            list(APPEND compilerOptions /fsanitize=address)
            message(STATUS "Enabled ASan")
        else()
            message(WARNING "Sanitized builds are not supported for CXX compiler: '${CMAKE_CXX_COMPILER_ID}'")
        endif()
    endif()

    if(CPPART_THREAD_SANITIZED)
        if(isGCC OR isClang)
            list(APPEND compilerOptions -DTSAN -fsanitize=thread)
            list(APPEND linkerOptions -fsanitize=thread)
            list(APPEND compilerOptions
                    -fno-sanitize-recover=all
                    -fno-optimize-sibling-calls
                    -fno-omit-frame-pointer
            )
            message(STATUS "Enabled TSan")
        else()
            message(WARNING "Sanitized builds are not supported for CXX compiler: '${CMAKE_CXX_COMPILER_ID}'")
        endif()
    endif()

    message(STATUS "Setting compiler options for target: ${compilerOptions}")
    message(STATUS "Setting compiler definitions for target: ${compilerDefinitions}")
    message(STATUS "Setting linker options for target: ${linkerOptions}")

    target_compile_options(${TARGET} PRIVATE ${compilerOptions})
    target_compile_definitions(${TARGET} PRIVATE ${compilerDefinitions})
    target_link_options(${TARGET} PRIVATE ${linkerOptions})

endfunction()
