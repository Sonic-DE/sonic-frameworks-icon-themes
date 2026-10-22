
find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test)

include(ECMAddTests)

macro(KICONTHEMES_TESTS)
    foreach(_testname ${ARGN})
        set(_srcs ${_testname}.cpp)
        qt_add_resources(_srcs resources.qrc)
        ecm_add_test(${_srcs} TEST_NAME ${_testname} LINK_LIBRARIES Qt6::Test KF6::IconThemes KF6::WidgetsAddons KF6::ConfigCore KF6::IconWidgets)
    endforeach(_testname)
endmacro()

kiconthemes_tests(
  kiconloader_unittest
  kiconloader_resourcethemetest
  kicontheme_unittest
  kiconengine_unittest
  kiconengine_scaled_unittest
  kiconbutton_unittest
  kicondialog_unittest
  kpixmapsequenceloadertest
)

find_package(Qt6 COMPONENTS Quick)
if (TARGET Qt6::Quick)
  kiconthemes_tests(kquickiconprovidertest)
  target_link_libraries(kquickiconprovidertest Qt6::Quick)
endif()

set_tests_properties(kiconloader_unittest PROPERTIES RUN_SERIAL TRUE)

if (USE_BreezeIcons)
  add_executable(kiconthemeinitialization_unittest kiconthemeinitialization_unittest.cpp)
  target_link_libraries(kiconthemeinitialization_unittest PRIVATE Qt6::Test KF6::IconThemes)
  add_test(NAME kiconthemeinitialization_default COMMAND kiconthemeinitialization_unittest)
  add_test(NAME kiconthemeinitialization_configured COMMAND kiconthemeinitialization_unittest)
  set_tests_properties(kiconthemeinitialization_default PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;KICONTHEMES_TEST_THEME="
  )
  set_tests_properties(kiconthemeinitialization_configured PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen;KICONTHEMES_TEST_THEME=RegressionUserTheme"
  )
endif()

# Benchmark, compiled, but not run automatically with ctest
add_executable(kiconloader_benchmark kiconloader_benchmark.cpp)
target_link_libraries(kiconloader_benchmark Qt6::Test KF6::IconThemes KF6::WidgetsAddons KF6::ConfigCore)
