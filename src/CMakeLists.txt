find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Find the Qt6 components
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS REQUIRED Core Gui Widgets Xml Network Concurrent PrintSupport DataVisualization)

qt_standard_project_setup()

#display compiler name
message(STATUS "Compiler being used: ${CMAKE_CXX_COMPILER_ID} - ${CMAKE_CXX_COMPILER} ")
get_filename_component(compiler_path ${CMAKE_CXX_COMPILER} DIRECTORY)
message(STATUS "Compiler directory: ${compiler_path}")

# Find OpenMP
find_package(OpenMP REQUIRED)
message(STATUS "OpenMP_CXX_FOUND: ${OpenMP_CXX_FOUND}")
message(STATUS "OpenMP_CXX_FLAGS: ${OpenMP_CXX_FLAGS}")
message(STATUS "OpenMP_CXX_LIBRARIES: ${OpenMP_CXX_LIBRARIES}")

# Specify the paths for the custom library
set(QSCINTILLA_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/QScintilla_src-2.14.0/src)
set(QWT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/qwt-6.2.0/src)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ASAP2
    ${CMAKE_CURRENT_SOURCE_DIR}/ASAP2/Items
    ${CMAKE_CURRENT_SOURCE_DIR}/ASAP2/Nodes
    ${CMAKE_CURRENT_SOURCE_DIR}/DataContainers
    ${CMAKE_CURRENT_SOURCE_DIR}/DataModels
    ${CMAKE_CURRENT_SOURCE_DIR}/PlotGraph
    ${CMAKE_CURRENT_SOURCE_DIR}/WidgetsViews
    ${CMAKE_CURRENT_SOURCE_DIR}/ui_forms
    ${QSCINTILLA_INCLUDE_DIR}
    ${QWT_INCLUDE_DIR}
    ${OMP_INCLUDE_DIR}
)

set(UI_FORMS
    ui_forms/chooselabel.cpp ui_forms/chooselabel.h ui_forms/chooselabel.ui
    ui_forms/choosesubset.cpp ui_forms/choosesubset.h ui_forms/choosesubset.ui
    ui_forms/deletefiledialog.cpp ui_forms/deletefiledialog.h ui_forms/deletefiledialog.ui
    ui_forms/dialog.cpp ui_forms/dialog.h ui_forms/dialog.ui
    ui_forms/dialogbitdisplay.cpp ui_forms/dialogbitdisplay.h ui_forms/dialogbitdisplay.ui
    ui_forms/dialogchooseexportformat.cpp ui_forms/dialogchooseexportformat.h ui_forms/dialogchooseexportformat.ui
    ui_forms/dialogchoosemodule.cpp ui_forms/dialogchoosemodule.h ui_forms/dialogchoosemodule.ui
    ui_forms/dialogcsv.cpp ui_forms/dialogcsv.h ui_forms/dialogcsv.ui
    ui_forms/dialogdatadimension.cpp ui_forms/dialogdatadimension.h ui_forms/dialogdatadimension.ui
    ui_forms/dialogeditastext.cpp ui_forms/dialogeditastext.h ui_forms/dialogeditastext.ui
    ui_forms/dialogexceedwb.cpp ui_forms/dialogexceedwb.h ui_forms/dialogexceedwb.ui
    ui_forms/dialogprogressdownload.cpp ui_forms/dialogprogressdownload.h ui_forms/dialogprogressdownload.ui
    ui_forms/formcompare.cpp ui_forms/formcompare.h ui_forms/formcompare.ui
    ui_forms/formeditor.cpp ui_forms/formeditor.h ui_forms/formeditor.ui
    ui_forms/graph.cpp ui_forms/graph.h ui_forms/graph.ui
    ui_forms/graphverify.cpp ui_forms/graphverify.h ui_forms/graphverify.ui
    ui_forms/labelproperties.cpp ui_forms/labelproperties.h ui_forms/labelproperties.ui
    ui_forms/mdimain.cpp ui_forms/mdimain.h ui_forms/mdimain.ui
)


set (SRCS_ITEMS
    ASAP2/Items/item.cpp ASAP2/Items/item.h
    ASAP2/Items/addr_epk.cpp ASAP2/Items/addr_epk.h
    ASAP2/Items/alignment_byte.cpp ASAP2/Items/alignment_byte.h
    ASAP2/Items/alignment_float32_ieee.cpp ASAP2/Items/alignment_float32_ieee.h
    ASAP2/Items/alignment_float64_ieee.cpp ASAP2/Items/alignment_float64_ieee.h
    ASAP2/Items/alignment_long.cpp ASAP2/Items/alignment_long.h
    ASAP2/Items/alignment_word.cpp  ASAP2/Items/alignment_word.h
    ASAP2/Items/annotation_label.cpp ASAP2/Items/annotation_label.h
    ASAP2/Items/annotation_origin.cpp ASAP2/Items/annotation_origin.h
    ASAP2/Items/array_size.cpp ASAP2/Items/array_size.h
    ASAP2/Items/axis_pts_ref.cpp ASAP2/Items/axis_pts_ref.h
    ASAP2/Items/axis_pts_x.cpp ASAP2/Items/axis_pts_x.h
    ASAP2/Items/axis_pts_y.cpp ASAP2/Items/axis_pts_y.h
    ASAP2/Items/bit_mask.cpp ASAP2/Items/bit_mask.h
    ASAP2/Items/byte_order.cpp ASAP2/Items/byte_order.h
    ASAP2/Items/calibration_handle_text.cpp ASAP2/Items/calibration_handle_text.h
    ASAP2/Items/coeffs.cpp ASAP2/Items/coeffs.h
    ASAP2/Items/coeffs_linear.cpp     ASAP2/Items/coeffs_linear.h
    ASAP2/Items/compu_tab_ref.cpp     ASAP2/Items/compu_tab_ref.h
    ASAP2/Items/cpu_type.cpp    ASAP2/Items/cpu_type.h
    ASAP2/Items/customer.cpp    ASAP2/Items/customer.h
    ASAP2/Items/customer_no.cpp    ASAP2/Items/customer_no.h
    ASAP2/Items/data_size.cpp    ASAP2/Items/data_size.h
    ASAP2/Items/default_value.cpp    ASAP2/Items/default_value.h
    ASAP2/Items/default_value_numeric.cpp    ASAP2/Items/default_value_numeric.h
    ASAP2/Items/deposit.cpp    ASAP2/Items/deposit.h
    ASAP2/Items/display_identifier.cpp    ASAP2/Items/display_identifier.h
    ASAP2/Items/ecu.cpp    ASAP2/Items/ecu.h
    ASAP2/Items/ecu_address.cpp    ASAP2/Items/ecu_address.h
    ASAP2/Items/ecu_address_extension.cpp    ASAP2/Items/ecu_address_extension.h
    ASAP2/Items/epk.cpp    ASAP2/Items/epk.h
    ASAP2/Items/extended_limits.cpp    ASAP2/Items/extended_limits.h
    ASAP2/Items/fix_axis_par.cpp    ASAP2/Items/fix_axis_par.h
    ASAP2/Items/fix_axis_par_dist.cpp    ASAP2/Items/fix_axis_par_dist.h
    ASAP2/Items/fnc_values.cpp    ASAP2/Items/fnc_values.h
    ASAP2/Items/format.cpp    ASAP2/Items/format.h
    ASAP2/Items/formula_inv.cpp    ASAP2/Items/formula_inv.h
    ASAP2/Items/function_version.cpp    ASAP2/Items/function_version.h
    ASAP2/Items/matrix_dim.cpp    ASAP2/Items/matrix_dim.h
    ASAP2/Items/max_refresh.cpp    ASAP2/Items/max_refresh.h
    ASAP2/Items/no_axis_pts_x.cpp    ASAP2/Items/no_axis_pts_x.h
    ASAP2/Items/no_axis_pts_y.cpp    ASAP2/Items/no_axis_pts_y.h
    ASAP2/Items/number.cpp    ASAP2/Items/number.h
    ASAP2/Items/phone_no.cpp    ASAP2/Items/phone_no.h
    ASAP2/Items/project_no.cpp    ASAP2/Items/project_no.h
    ASAP2/Items/read_only.cpp    ASAP2/Items/read_only.h
    ASAP2/Items/ref_unit.cpp    ASAP2/Items/ref_unit.h
    ASAP2/Items/root.cpp    ASAP2/Items/root.h
    ASAP2/Items/s_rec_layout.cpp    ASAP2/Items/s_rec_layout.h
    ASAP2/Items/si_exponents.cpp    ASAP2/Items/si_exponents.h
    ASAP2/Items/src_addr_x.cpp    ASAP2/Items/src_addr_x.h
    ASAP2/Items/src_addr_y.cpp    ASAP2/Items/src_addr_y.h
    ASAP2/Items/static_record_layout.cpp    ASAP2/Items/static_record_layout.h
    ASAP2/Items/status_string_ref.cpp    ASAP2/Items/status_string_ref.h
    ASAP2/Items/supplier.cpp    ASAP2/Items/supplier.h
    ASAP2/Items/system_constant.cpp    ASAP2/Items/system_constant.h
    ASAP2/Items/unit_conversion.cpp    ASAP2/Items/unit_conversion.h
    ASAP2/Items/user.cpp    ASAP2/Items/user.h
    ASAP2/Items/version.cpp    ASAP2/Items/version.h
)

set(SRCS_NODES
    ASAP2/a2l.cpp ASAP2/a2l.h
    ASAP2/node.cpp    ASAP2/node.h
    ASAP2/worker.cpp  ASAP2/worker.h
    ASAP2/Nodes/a2lfile.cpp    ASAP2/Nodes/a2lfile.h
    ASAP2/Nodes/a2ml.cpp    ASAP2/Nodes/a2ml.h
    ASAP2/Nodes/annotation.cpp    ASAP2/Nodes/annotation.h
    ASAP2/Nodes/annotation_text.cpp    ASAP2/Nodes/annotation_text.h
    ASAP2/Nodes/axis_descr.cpp    ASAP2/Nodes/axis_descr.h
    ASAP2/Nodes/axis_pts.cpp    ASAP2/Nodes/axis_pts.h
    ASAP2/Nodes/calibration_handle.cpp    ASAP2/Nodes/calibration_handle.h
    ASAP2/Nodes/calibration_method.cpp    ASAP2/Nodes/calibration_method.h
    ASAP2/Nodes/characteristic.cpp    ASAP2/Nodes/characteristic.h
    ASAP2/Nodes/compu_method.cpp    ASAP2/Nodes/compu_method.h
    ASAP2/Nodes/compu_tab.cpp    ASAP2/Nodes/compu_tab.h
    ASAP2/Nodes/compu_vtab.cpp    ASAP2/Nodes/compu_vtab.h
    ASAP2/Nodes/compu_vtab_range.cpp    ASAP2/Nodes/compu_vtab_range.h
    ASAP2/Nodes/dbfile.cpp    ASAP2/Nodes/dbfile.h
    ASAP2/Nodes/def_characteristic.cpp    ASAP2/Nodes/def_characteristic.h
    ASAP2/Nodes/fix_axis_par_list.cpp    ASAP2/Nodes/fix_axis_par_list.h
    ASAP2/Nodes/formula.cpp    ASAP2/Nodes/formula.h
    ASAP2/Nodes/function.cpp    ASAP2/Nodes/function.h
    ASAP2/Nodes/function_list.cpp    ASAP2/Nodes/function_list.h
    ASAP2/Nodes/group.cpp    ASAP2/Nodes/group.h
    ASAP2/Nodes/header.cpp    ASAP2/Nodes/header.h
    ASAP2/Nodes/if_data.cpp    ASAP2/Nodes/if_data.h
    ASAP2/Nodes/in_measurement.cpp
    ASAP2/Nodes/in_measurement.h    ASAP2/Nodes/loc_measurement.cpp
    ASAP2/Nodes/loc_measurement.h    ASAP2/Nodes/measurement.cpp
    ASAP2/Nodes/measurement.h    ASAP2/Nodes/memory_layout.cpp
    ASAP2/Nodes/memory_layout.h    ASAP2/Nodes/memory_segment.cpp
    ASAP2/Nodes/memory_segment.h    ASAP2/Nodes/mod_common.cpp
    ASAP2/Nodes/mod_common.h    ASAP2/Nodes/mod_par.cpp
    ASAP2/Nodes/mod_par.h    ASAP2/Nodes/module.cpp
    ASAP2/Nodes/module.h    ASAP2/Nodes/out_measurement.cpp
    ASAP2/Nodes/out_measurement.h    ASAP2/Nodes/project.cpp
    ASAP2/Nodes/project.h    ASAP2/Nodes/record_layout.cpp
    ASAP2/Nodes/record_layout.h    ASAP2/Nodes/ref_characteristic.cpp
    ASAP2/Nodes/ref_characteristic.h    ASAP2/Nodes/ref_measurement.cpp
    ASAP2/Nodes/ref_measurement.h    ASAP2/Nodes/sub_function.cpp
    ASAP2/Nodes/sub_function.h    ASAP2/Nodes/sub_group.cpp
    ASAP2/Nodes/sub_group.h    ASAP2/Nodes/unit.cpp
    ASAP2/Nodes/unit.h    ASAP2/a2lgrammar.cpp
)

set(SRCS_LEXER
    ASAP2/a2lgrammar.h
    ASAP2/basefactory.h
    ASAP2/lexer.cpp
    ASAP2/lexer.h
)

set(SRCS_DATACONTAINERS
    DataContainers/cdfxfile.cpp    DataContainers/cdfxfile.h
    DataContainers/csv.cpp    DataContainers/csv.h
    DataContainers/data.cpp    DataContainers/data.h
    DataContainers/datacontainer.cpp    DataContainers/datacontainer.h
    DataContainers/dcmfile.cpp    DataContainers/dcmfile.h
    DataContainers/errorcode.cpp    DataContainers/errorcode.h
    DataContainers/hexfile.cpp    DataContainers/hexfile.h
    DataContainers/lexerCsv.cpp    DataContainers/lexerCsv.h
    DataContainers/lexerDcm.cpp    DataContainers/lexerDcm.h
    DataContainers/srecfile.cpp    DataContainers/srecfile.h
)

set(SRCS_DATAMODELS
    DataModels/a2ltreemodel.cpp  DataModels/a2ltreemodel.h
    DataModels/charmodel.cpp   DataModels/charmodel.h
    DataModels/comboboxdelegate.cpp DataModels/comboboxdelegate.h
    DataModels/comparemodel.cpp  DataModels/comparemodel.h
    DataModels/FandRmodel.cpp    DataModels/FandRModel.h
    DataModels/graphmodel.cpp    DataModels/graphmodel.h
    DataModels/measmodel.cpp DataModels/measmodel.h
    DataModels/obdMergeModel.cpp    DataModels/obdMergeModel.h
    DataModels/obdsortfilterproxymodel.cpp  DataModels/obdsortfilterproxymodel.h
    DataModels/sptablemodel.cpp  DataModels/sptablemodel.h
    DataModels/sptablemodelHex.cpp   DataModels/sptablemodelHex.h
    DataModels/treemodelcompleter.cpp    DataModels/treemodelcompleter.h)

set(SRCS_WIDGETSVIEWS
    WidgetsViews/ctabwidget.cpp WidgetsViews/ctabwidget.h
    WidgetsViews/mainwindow.cpp    WidgetsViews/mainwindow.h
    WidgetsViews/settingstree.cpp    WidgetsViews/settingstree.h
    WidgetsViews/treedirectory.cpp    WidgetsViews/treedirectory.h
    WidgetsViews/variantdelegate.cpp    WidgetsViews/variantdelegate.h
    WidgetsViews/workingdirectory.cpp    WidgetsViews/workingdirectory.h
    WidgetsViews/workproject.cpp    WidgetsViews/workproject.h
    WidgetsViews/chtextedit.cpp    WidgetsViews/chtextedit.h
    WidgetsViews/dialoghttpupdate.cpp    WidgetsViews/dialoghttpupdate.h
    WidgetsViews/dialogupdate.cpp    WidgetsViews/dialogupdate.h
    WidgetsViews/finddialog.cpp    WidgetsViews/finddialog.h
    WidgetsViews/locationdialog.cpp   WidgetsViews/locationdialog.h
    WidgetsViews/scrollbar.cpp    WidgetsViews/scrollbar.h
    WidgetsViews/freezetablewidget.cpp    WidgetsViews/freezetablewidget.h
    WidgetsViews/myqlineedit.cpp    WidgetsViews/myqlineedit.h
    WidgetsViews/projectlistwidget.cpp    WidgetsViews/projectlistwidget.h
    WidgetsViews/mytreeview.cpp WidgetsViews/mytreeview.h
    WidgetsViews/spreadsheetview.cpp   WidgetsViews/spreadsheetview.h
)

set(SRCS_PLOT
    PlotGraph/canvaspicker.cpp     PlotGraph/canvaspicker.h
    PlotGraph/plot.cpp    PlotGraph/plot.h
    PlotGraph/scrollzoomer.cpp    PlotGraph/scrollzoomer.h
    PlotGraph/surfacegraph.cpp    PlotGraph/surfacegraph.h
    PlotGraph/zoomer.h PlotGraph/zoomer.cpp)

set(SRCS 
    main.cpp
    icones.qrc
    myappicon.rc
    space_shuttle.ico
)


# Add the executable target
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(${CMAKE_PROJECT_NAME} MANUAL_FINALIZATION  ${SRCS} ${SRCS_LEXER} ${SRCS_NODES} ${SRCS_ITEMS} ${SRCS_DATACONTAINERS} ${SRCS_DATAMODELS} ${SRCS_WIDGETSVIEWS} ${SRCS_PLOT} ${UI_FORMS} )
else()
    add_executable(${CMAKE_PROJECT_NAME} ${SRCS} ${SRCS_LEXER} ${SRCS_NODES} ${SRCS_ITEMS} ${SRCS_DATACONTAINERS} ${SRCS_DATAMODELS} ${SRCS_WIDGETSVIEWS} ${SRCS_PLOT} ${UI_FORMS})
endif()


# Link the Qt6 and openMP libraries to the executable
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Xml
Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Concurrent Qt${QT_VERSION_MAJOR}::PrintSupport Qt${QT_VERSION_MAJOR}::DataVisualization
OpenMP::OpenMP_CXX)

if(WIN32)
    message(STATUS "Building on Windows")

    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
        target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_17)
        set(LIB_DIR "/libs/build/mingw/")
        set(QSCINTILLA_LIB "libqscintilla2_qt6.a")
        set(QWT_LIB "libqwt.a")
    elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_17)
        set(LIB_DIR "/libs/build/llvm/")
        set(QSCINTILLA_LIB "libqscintilla2_qt6.a")
        set(QWT_LIB "libqwt.a")
    elseif (MSVC)
        target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /std:c++17)
        set(LIB_DIR "/libs/build/msvc/")
        set(QSCINTILLA_LIB "qscintilla2_qt6.lib")
        set(QWT_LIB "qwt.lib")
    endif()

    link_directories(${CMAKE_SOURCE_DIR}/${LIB_DIR})

    # Link the custom library to your target
    target_link_libraries(
        ${CMAKE_PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
        ${CMAKE_SOURCE_DIR}/${LIB_DIR}/${QSCINTILLA_LIB}
        ${CMAKE_SOURCE_DIR}/${LIB_DIR}/${QWT_LIB})

    # Add preprocessor definitions
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE QT_DLL QSCINTILLA_DLL QWT_DLL)

elseif(APPLE)
    message(STATUS "Building on macOS")

    # Set compiler options
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_17)
    endif()

    # Specify the paths of the external libs
    set(LIB_DIR "/libs/build/")
    set(QSCINTILLA_LIB "libqscintilla2_qt6.15.dylib")
    set(QWT_FRAMEWORK_PATH /Users/tof/Documents/Programmation/Qt/HEXplorer/HEXplorer_qt6/libs/build/qwt.framework)

    # Link the custom library to your target
    target_link_libraries(
        ${CMAKE_PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
        ${CMAKE_PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/${LIB_DIR}/${QSCINTILLA_LIB}
        "-F${QWT_FRAMEWORK_PATH}/.." "-framework qwt"
    )
endif()

# Set properties for the app bundle
set_target_properties(${CMAKE_PROJECT_NAME} PROPERTIES
    ${BUNDLE_ID_OPTION}
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_BUNDLE_NAME ${CMAKE_PROJECT_NAME}
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.adhoc2.hexplorer"
    MACOSX_BUNDLE_VERSION_STRING "${PROJECT_VERSION}"
    MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}"
    MACOSX_BUNDLE_ICON_FILE "space_shuttle.ico"
    RESOURCE "${CMAKE_SOURCE_DIR}/src/space_shuttle.ico"
)

    # Steps for deployment of the app bundle
    #  1- macdeployqt /Users/tof/Documents/Programmation/Qt/HEXplorer/HEXplorer_qt6/build/cmake/build-HEXplorer_qt6-Qt_6_7_1_for_macOS-Release/src/HEXplorer.app
    #  2- copy the libraries into content/FrameWorks
    #  3- install_name_tool -change "qwt.framework/Versions/6/qwt" "@executable_path/../Frameworks/qwt.framework/Versions/6/qwt" /Users/tof/Documents/Programmation/Qt/HEXplorer/HEXplorer_qt6/build/cmake/build-HEXplorer_qt6-Qt_6_7_1_for_macOS-Release/src/HEXplorer.app/Contents/MacOS/HEXplorer


install(TARGETS ${CMAKE_PROJECT_NAME}
    BUNDLE DESTINATION .
    RESOURCE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Generate deployment script
qt_generate_deploy_app_script(
    TARGET ${CMAKE_PROJECT_NAME}
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

message(STATUS "Deploy script: ${deploy_script} ")
message(STATUS "Install bin dir: ${CMAKE_INSTALL_BINDIR} ")

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${CMAKE_PROJECT_NAME})
endif()



