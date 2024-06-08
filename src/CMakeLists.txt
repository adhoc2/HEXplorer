
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Find the Qt6 components
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS REQUIRED Core Gui Widgets Xml Network Concurrent PrintSupport DataVisualization)

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
    .
    ASAP2
    ASAP2/Items
    ASAP2/Nodes
    DataContainers
    ${QSCINTILLA_INCLUDE_DIR}
    ${QWT_INCLUDE_DIR}
    ${OMP_INCLUDE_DIR}
)

set (UI_FORMS
    chooselabel.ui choosesubset.ui
    deletefiledialog.ui  dialog.ui dialogbitdisplay.ui
    dialogchooseexportformat.ui dialogchoosemodule.ui
    dialogcsv.ui dialogdatadimension.ui dialogeditastext.ui
    dialogexceedwb.ui dialogprogressdownload.ui
    formcompare.ui formeditor.ui graph.ui graphverify.ui
    labelproperties.ui mdimain.ui)


set (SRCS
    ASAP2/Items/addr_epk.cpp
    ASAP2/Items/addr_epk.h
    ASAP2/Items/alignment_byte.cpp
    ASAP2/Items/alignment_byte.h
    ASAP2/Items/alignment_float32_ieee.cpp
    ASAP2/Items/alignment_float32_ieee.h
    ASAP2/Items/alignment_float64_ieee.cpp
    ASAP2/Items/alignment_float64_ieee.h
    ASAP2/Items/alignment_long.cpp
    ASAP2/Items/alignment_long.h
    ASAP2/Items/alignment_word.cpp
    ASAP2/Items/alignment_word.h
    ASAP2/Items/annotation_label.cpp
    ASAP2/Items/annotation_label.h
    ASAP2/Items/annotation_origin.cpp
    ASAP2/Items/annotation_origin.h
    ASAP2/Items/array_size.cpp
    ASAP2/Items/array_size.h
    ASAP2/Items/axis_pts_ref.cpp
    ASAP2/Items/axis_pts_ref.h
    ASAP2/Items/axis_pts_x.cpp
    ASAP2/Items/axis_pts_x.h
    ASAP2/Items/axis_pts_y.cpp
    ASAP2/Items/axis_pts_y.h
    ASAP2/Items/bit_mask.cpp
    ASAP2/Items/bit_mask.h
    ASAP2/Items/byte_order.cpp
    ASAP2/Items/byte_order.h
    ASAP2/Items/calibration_handle_text.cpp
    ASAP2/Items/calibration_handle_text.h
    ASAP2/Items/coeffs.cpp
    ASAP2/Items/coeffs.h
    ASAP2/Items/coeffs_linear.cpp
    ASAP2/Items/coeffs_linear.h
    ASAP2/Items/compu_tab_ref.cpp
    ASAP2/Items/compu_tab_ref.h
    ASAP2/Items/cpu_type.cpp
    ASAP2/Items/cpu_type.h
    ASAP2/Items/customer.cpp
    ASAP2/Items/customer.h
    ASAP2/Items/customer_no.cpp
    ASAP2/Items/customer_no.h
    ASAP2/Items/data_size.cpp
    ASAP2/Items/data_size.h
    ASAP2/Items/default_value.cpp
    ASAP2/Items/default_value.h
    ASAP2/Items/default_value_numeric.cpp
    ASAP2/Items/default_value_numeric.h
    ASAP2/Items/deposit.cpp
    ASAP2/Items/deposit.h
    ASAP2/Items/display_identifier.cpp
    ASAP2/Items/display_identifier.h
    ASAP2/Items/ecu.cpp
    ASAP2/Items/ecu.h
    ASAP2/Items/ecu_address.cpp
    ASAP2/Items/ecu_address.h
    ASAP2/Items/ecu_address_extension.cpp
    ASAP2/Items/ecu_address_extension.h
    ASAP2/Items/epk.cpp
    ASAP2/Items/epk.h
    ASAP2/Items/extended_limits.cpp
    ASAP2/Items/extended_limits.h
    ASAP2/Items/fix_axis_par.cpp
    ASAP2/Items/fix_axis_par.h
    ASAP2/Items/fix_axis_par_dist.cpp
    ASAP2/Items/fix_axis_par_dist.h
    ASAP2/Items/fnc_values.cpp
    ASAP2/Items/fnc_values.h
    ASAP2/Items/format.cpp
    ASAP2/Items/format.h
    ASAP2/Items/formula_inv.cpp
    ASAP2/Items/formula_inv.h
    ASAP2/Items/function_version.cpp
    ASAP2/Items/function_version.h
    ASAP2/Items/matrix_dim.cpp
    ASAP2/Items/matrix_dim.h
    ASAP2/Items/max_refresh.cpp
    ASAP2/Items/max_refresh.h
    ASAP2/Items/no_axis_pts_x.cpp
    ASAP2/Items/no_axis_pts_x.h
    ASAP2/Items/no_axis_pts_y.cpp
    ASAP2/Items/no_axis_pts_y.h
    ASAP2/Items/number.cpp
    ASAP2/Items/number.h
    ASAP2/Items/phone_no.cpp
    ASAP2/Items/phone_no.h
    ASAP2/Items/project_no.cpp
    ASAP2/Items/project_no.h
    ASAP2/Items/read_only.cpp
    ASAP2/Items/read_only.h
    ASAP2/Items/ref_unit.cpp
    ASAP2/Items/ref_unit.h
    ASAP2/Items/root.cpp
    ASAP2/Items/root.h
    ASAP2/Items/s_rec_layout.cpp
    ASAP2/Items/s_rec_layout.h
    ASAP2/Items/si_exponents.cpp
    ASAP2/Items/si_exponents.h
    ASAP2/Items/src_addr_x.cpp
    ASAP2/Items/src_addr_x.h
    ASAP2/Items/src_addr_y.cpp
    ASAP2/Items/src_addr_y.h
    ASAP2/Items/static_record_layout.cpp
    ASAP2/Items/static_record_layout.h
    ASAP2/Items/status_string_ref.cpp
    ASAP2/Items/status_string_ref.h
    ASAP2/Items/supplier.cpp
    ASAP2/Items/supplier.h
    ASAP2/Items/system_constant.cpp
    ASAP2/Items/system_constant.h
    ASAP2/Items/unit_conversion.cpp
    ASAP2/Items/unit_conversion.h
    ASAP2/Items/user.cpp
    ASAP2/Items/user.h
    ASAP2/Items/version.cpp
    ASAP2/Items/version.h
    ASAP2/Nodes/a2lfile.cpp
    ASAP2/Nodes/a2lfile.h
    ASAP2/Nodes/a2ml.cpp
    ASAP2/Nodes/a2ml.h
    ASAP2/Nodes/annotation.cpp
    ASAP2/Nodes/annotation.h
    ASAP2/Nodes/annotation_text.cpp
    ASAP2/Nodes/annotation_text.h
    ASAP2/Nodes/axis_descr.cpp
    ASAP2/Nodes/axis_descr.h
    ASAP2/Nodes/axis_pts.cpp
    ASAP2/Nodes/axis_pts.h
    ASAP2/Nodes/calibration_handle.cpp
    ASAP2/Nodes/calibration_handle.h
    ASAP2/Nodes/calibration_method.cpp
    ASAP2/Nodes/calibration_method.h
    ASAP2/Nodes/characteristic.cpp
    ASAP2/Nodes/characteristic.h
    ASAP2/Nodes/compu_method.cpp
    ASAP2/Nodes/compu_method.h
    ASAP2/Nodes/compu_tab.cpp
    ASAP2/Nodes/compu_tab.h
    ASAP2/Nodes/compu_vtab.cpp
    ASAP2/Nodes/compu_vtab.h
    ASAP2/Nodes/compu_vtab_range.cpp
    ASAP2/Nodes/compu_vtab_range.h
    ASAP2/Nodes/dbfile.cpp
    ASAP2/Nodes/dbfile.h
    ASAP2/Nodes/def_characteristic.cpp
    ASAP2/Nodes/def_characteristic.h
    ASAP2/Nodes/fix_axis_par_list.cpp
    ASAP2/Nodes/fix_axis_par_list.h
    ASAP2/Nodes/formula.cpp
    ASAP2/Nodes/formula.h
    ASAP2/Nodes/function.cpp
    ASAP2/Nodes/function.h
    ASAP2/Nodes/function_list.cpp
    ASAP2/Nodes/function_list.h
    ASAP2/Nodes/group.cpp
    ASAP2/Nodes/group.h
    ASAP2/Nodes/header.cpp
    ASAP2/Nodes/header.h
    ASAP2/Nodes/if_data.cpp
    ASAP2/Nodes/if_data.h
    ASAP2/Nodes/in_measurement.cpp
    ASAP2/Nodes/in_measurement.h
    ASAP2/Nodes/loc_measurement.cpp
    ASAP2/Nodes/loc_measurement.h
    ASAP2/Nodes/measurement.cpp
    ASAP2/Nodes/measurement.h
    ASAP2/Nodes/memory_layout.cpp
    ASAP2/Nodes/memory_layout.h
    ASAP2/Nodes/memory_segment.cpp
    ASAP2/Nodes/memory_segment.h
    ASAP2/Nodes/mod_common.cpp
    ASAP2/Nodes/mod_common.h
    ASAP2/Nodes/mod_par.cpp
    ASAP2/Nodes/mod_par.h
    ASAP2/Nodes/module.cpp
    ASAP2/Nodes/module.h
    ASAP2/Nodes/out_measurement.cpp
    ASAP2/Nodes/out_measurement.h
    ASAP2/Nodes/project.cpp
    ASAP2/Nodes/project.h
    ASAP2/Nodes/record_layout.cpp
    ASAP2/Nodes/record_layout.h
    ASAP2/Nodes/ref_characteristic.cpp
    ASAP2/Nodes/ref_characteristic.h
    ASAP2/Nodes/ref_measurement.cpp
    ASAP2/Nodes/ref_measurement.h
    ASAP2/Nodes/sub_function.cpp
    ASAP2/Nodes/sub_function.h
    ASAP2/Nodes/sub_group.cpp
    ASAP2/Nodes/sub_group.h
    ASAP2/Nodes/unit.cpp
    ASAP2/Nodes/unit.h
    ASAP2/a2lgrammar.cpp
    ASAP2/a2lgrammar.h
    ASAP2/basefactory.h
    ASAP2/lexer.cpp
    ASAP2/lexer.h
    DataContainers/cdfxfile.cpp
    DataContainers/cdfxfile.h
    DataContainers/csv.cpp
    DataContainers/csv.h
    DataContainers/data.cpp
    DataContainers/data.h
    DataContainers/datacontainer.cpp
    DataContainers/datacontainer.h
    DataContainers/dcmfile.cpp
    DataContainers/dcmfile.h
    DataContainers/errorcode.cpp
    DataContainers/errorcode.h
    DataContainers/hexfile.cpp
    DataContainers/hexfile.h
    DataContainers/lexerCsv.cpp
    DataContainers/lexerCsv.h
    DataContainers/lexerDcm.cpp
    DataContainers/lexerDcm.h
    DataContainers/srecfile.cpp
    DataContainers/srecfile.h
    a2l.cpp
    a2l.h
    a2ltreemodel.cpp
    a2ltreemodel.h
    canvaspicker.cpp
    canvaspicker.h
    charmodel.cpp
    charmodel.h
    chooselabel.cpp
    chooselabel.h
    choosesubset.cpp
    choosesubset.h
    chtextedit.cpp
    chtextedit.h


    comboboxdelegate.cpp
    comboboxdelegate.h
    comparemodel.cpp
    comparemodel.h
    ctabwidget.cpp
    ctabwidget.h
    deletefiledialog.cpp
    deletefiledialog.h
    dialog.cpp
    dialog.h
    dialogbitdisplay.cpp
    dialogbitdisplay.h
    dialogchooseexportformat.cpp
    dialogchooseexportformat.h
    dialogchoosemodule.cpp
    dialogchoosemodule.h
    dialogcsv.cpp
    dialogcsv.h
    dialogdatadimension.cpp
    dialogdatadimension.h
    dialogeditastext.cpp
    dialogeditastext.h
    dialogexceedwb.cpp
    dialogexceedwb.h
    dialoghttpupdate.cpp
    dialoghttpupdate.h
    dialogprogressdownload.cpp
    dialogprogressdownload.h
    dialogupdate.cpp
    dialogupdate.h
    FandRmodel.cpp
    FandRModel.h
    finddialog.cpp
    finddialog.h
    formcompare.cpp
    formcompare.h
    formeditor.cpp
    formeditor.h
    freezetablewidget.cpp
    freezetablewidget.h
    graph.cpp
    graph.h
    graphmodel.cpp
    graphmodel.h
    graphverify.cpp
    graphverify.h
    icones.qrc
    item.cpp
    item.h
    labelproperties.cpp
    labelproperties.h
    LinkDef.h
    locationdialog.cpp
    locationdialog.h
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mdimain.cpp
    mdimain.h
    measmodel.cpp
    measmodel.h
    memapi.h
    myappicon.rc
    myqlineedit.cpp
    myqlineedit.h
    mytreeview.cpp
    mytreeview.h
    node.cpp
    node.h
    obdMergeModel.cpp
    obdMergeModel.h
    obdsortfilterproxymodel.cpp
    obdsortfilterproxymodel.h
    plot.cpp
    plot.h
    projectlistwidget.cpp
    projectlistwidget.h
    resource.h
    scrollbar.cpp
    scrollbar.h
    scrollzoomer.cpp
    scrollzoomer.h
    settingstree.cpp
    settingstree.h
    space_shuttle.ico
    spreadsheetview.cpp
    spreadsheetview.h
    sptablemodel.cpp
    sptablemodel.h
    sptablemodelHex.cpp
    sptablemodelHex.h
    surfacegraph.cpp
    surfacegraph.h
    treedirectory.cpp
    treedirectory.h
    treemodelcompleter.cpp
    treemodelcompleter.h
    update.xml
    variantdelegate.cpp
    variantdelegate.h
    workingdirectory.cpp
    workingdirectory.h
    workproject.cpp
    workproject.h  
)


# Add the executable target
add_executable(${CMAKE_PROJECT_NAME} ${SRCS} ${UI_FORMS})

# Link the Qt6 and openMP libraries to the executable
target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Xml
Qt${QT_VERSION_MAJOR}::Network Qt${QT_VERSION_MAJOR}::Concurrent Qt${QT_VERSION_MAJOR}::PrintSupport Qt${QT_VERSION_MAJOR}::DataVisualization
OpenMP::OpenMP_CXX)

# Set compiler options
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_features(${CMAKE_PROJECT_NAME} PRIVATE cxx_std_17)
    #target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -Wno-deprecated-declarations)
    #target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE -w)
elseif (MSVC)
    target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /std:c++17)
    #target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE /W0)
endif()

if(WIN32)
    message(STATUS "Building on Windows")

    # Specify the paths of the external libs
    set(LIB_DIR "/libs/build/")
    set(QSCINTILLA_LIB "libqscintilla2_qt6.a")
    set(QWT_LIB "libqwt.a")

    # Link the custom library to your target
    target_link_libraries(
        ${CMAKE_PROJECT_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/${LIB_DIR}/${QSCINTILLA_LIB}
        ${CMAKE_SOURCE_DIR}/${LIB_DIR}/${QWT_LIB})

elseif(APPLE)
    message(STATUS "Building on macOS")

    # Specify the paths of the external libs
    set(LIB_DIR "/libs/build/")
    set(QSCINTILLA_LIB "libqscintilla2_qt6.15.dylib")
    set(QWT_FRAMEWORK_PATH /Users/tof/Documents/Programmation/Qt/HEXplorer/HEXplorer_qt6/libs/build/qwt.framework)

    # Link the custom library to your target
    target_link_libraries(
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


include(GNUInstallDirs)
install(TARGETS ${CMAKE_PROJECT_NAME}
    BUNDLE DESTINATION .
    RESOURCE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(${CMAKE_PROJECT_NAME})
endif()

