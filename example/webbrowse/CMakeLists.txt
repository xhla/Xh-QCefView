project(QWebBrowse)

set(CMAKE_FOLDER "Example")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)

get_target_property(_qmake_executable Qt${QT_VERSION_MAJOR}::qmake IMPORTED_LOCATION)
get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)

if (OS_WINDOWS)
    find_program(DEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
elseif (OS_MACOS)
    find_program(DEPLOYQT_EXECUTABLE macdeployqt HINTS "${_qt_bin_dir}")
elseif (OS_LINUX)
elseif (OS_MACOS)
else ()
endif ()


include_directories(
        ${CMAKE_SOURCE_DIR}/include
)


if (OS_WINDOWS)
    if (USE_GPU_OPTIMUS)
        add_definitions(-DENABLE_GPU_OPTIMUS=1)
    endif ()


    add_executable(${PROJECT_NAME} WIN32
            src/main.cpp
            src/MainWindow.cpp
            src/MainWindow.h
    )

    target_link_libraries(${PROJECT_NAME}
            PRIVATE
            Qt${QT_VERSION_MAJOR}::Core
            Qt${QT_VERSION_MAJOR}::Gui
            Qt${QT_VERSION_MAJOR}::Widgets
            QCefView
    )

    target_compile_definitions(${PROJECT_NAME} PRIVATE
            UNICODE
            _UNICODE
    )

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            PROPERTY
            VS_STARTUP_PROJECT ${PROJECT_NAME}
    )

    set_target_properties(${PROJECT_NAME}
            PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
    )

    add_custom_command(TARGET ${PROJECT_NAME}

            POST_BUILD

            # Copy QCefView binary to output folder
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            $<TARGET_FILE_DIR:QCefView>


            $<TARGET_FILE_DIR:${PROJECT_NAME}>


            # Deploy the Qt Application
            COMMAND ${DEPLOYQT_EXECUTABLE}
            --no-svg
            --no-translations
            --no-compiler-runtime
            $<TARGET_FILE:${PROJECT_NAME}>
    )
endif () # OS_WINDOWS

set_target_properties(${PROJECT_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/output/$<CONFIG>/bin
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/output/$<CONFIG>/lib
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/output/$<CONFIG>/lib
)

install(DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>/" DESTINATION "${PROJECT_NAME}$<$<CONFIG:Debug>:/Debug>")
