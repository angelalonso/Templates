# CMake generated Testfile for 
# Source directory: /home/aaf/Software/Dev/robot/test/first_package
# Build directory: /home/aaf/Software/Dev/robot/test/first_package/build/first_package
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(cppcheck "/usr/bin/python3.8" "-u" "/opt/ros/rolling/share/ament_cmake_test/cmake/run_test.py" "/home/aaf/Software/Dev/robot/test/first_package/build/first_package/test_results/first_package/cppcheck.xunit.xml" "--package-name" "first_package" "--output-file" "/home/aaf/Software/Dev/robot/test/first_package/build/first_package/ament_cppcheck/cppcheck.txt" "--command" "/opt/ros/rolling/bin/ament_cppcheck" "--xunit-file" "/home/aaf/Software/Dev/robot/test/first_package/build/first_package/test_results/first_package/cppcheck.xunit.xml" "--include_dirs" "/home/aaf/Software/Dev/robot/test/first_package/include")
set_tests_properties(cppcheck PROPERTIES  LABELS "cppcheck;linter" TIMEOUT "300" WORKING_DIRECTORY "/home/aaf/Software/Dev/robot/test/first_package" _BACKTRACE_TRIPLES "/opt/ros/rolling/share/ament_cmake_test/cmake/ament_add_test.cmake;125;add_test;/opt/ros/rolling/share/ament_cmake_cppcheck/cmake/ament_cppcheck.cmake;66;ament_add_test;/opt/ros/rolling/share/ament_cmake_cppcheck/cmake/ament_cmake_cppcheck_lint_hook.cmake;83;ament_cppcheck;/opt/ros/rolling/share/ament_cmake_cppcheck/cmake/ament_cmake_cppcheck_lint_hook.cmake;0;;/opt/ros/rolling/share/ament_cmake_core/cmake/core/ament_execute_extensions.cmake;48;include;/opt/ros/rolling/share/ament_lint_auto/cmake/ament_lint_auto_package_hook.cmake;21;ament_execute_extensions;/opt/ros/rolling/share/ament_lint_auto/cmake/ament_lint_auto_package_hook.cmake;0;;/opt/ros/rolling/share/ament_cmake_core/cmake/core/ament_execute_extensions.cmake;48;include;/opt/ros/rolling/share/ament_cmake_core/cmake/core/ament_package.cmake;66;ament_execute_extensions;/home/aaf/Software/Dev/robot/test/first_package/CMakeLists.txt;35;ament_package;/home/aaf/Software/Dev/robot/test/first_package/CMakeLists.txt;0;")
add_test(lint_cmake "/usr/bin/python3.8" "-u" "/opt/ros/rolling/share/ament_cmake_test/cmake/run_test.py" "/home/aaf/Software/Dev/robot/test/first_package/build/first_package/test_results/first_package/lint_cmake.xunit.xml" "--package-name" "first_package" "--output-file" "/home/aaf/Software/Dev/robot/test/first_package/build/first_package/ament_lint_cmake/lint_cmake.txt" "--command" "/opt/ros/rolling/bin/ament_lint_cmake" "--xunit-file" "/home/aaf/Software/Dev/robot/test/first_package/build/first_package/test_results/first_package/lint_cmake.xunit.xml")
set_tests_properties(lint_cmake PROPERTIES  LABELS "lint_cmake;linter" TIMEOUT "60" WORKING_DIRECTORY "/home/aaf/Software/Dev/robot/test/first_package" _BACKTRACE_TRIPLES "/opt/ros/rolling/share/ament_cmake_test/cmake/ament_add_test.cmake;125;add_test;/opt/ros/rolling/share/ament_cmake_lint_cmake/cmake/ament_lint_cmake.cmake;47;ament_add_test;/opt/ros/rolling/share/ament_cmake_lint_cmake/cmake/ament_cmake_lint_cmake_lint_hook.cmake;21;ament_lint_cmake;/opt/ros/rolling/share/ament_cmake_lint_cmake/cmake/ament_cmake_lint_cmake_lint_hook.cmake;0;;/opt/ros/rolling/share/ament_cmake_core/cmake/core/ament_execute_extensions.cmake;48;include;/opt/ros/rolling/share/ament_lint_auto/cmake/ament_lint_auto_package_hook.cmake;21;ament_execute_extensions;/opt/ros/rolling/share/ament_lint_auto/cmake/ament_lint_auto_package_hook.cmake;0;;/opt/ros/rolling/share/ament_cmake_core/cmake/core/ament_execute_extensions.cmake;48;include;/opt/ros/rolling/share/ament_cmake_core/cmake/core/ament_package.cmake;66;ament_execute_extensions;/home/aaf/Software/Dev/robot/test/first_package/CMakeLists.txt;35;ament_package;/home/aaf/Software/Dev/robot/test/first_package/CMakeLists.txt;0;")
add_test(uncrustify "/usr/bin/python3.8" "-u" "/opt/ros/rolling/share/ament_cmake_test/cmake/run_test.py" "/home/aaf/Software/Dev/robot/test/first_package/build/first_package/test_results/first_package/uncrustify.xunit.xml" "--package-name" "first_package" "--output-file" "/home/aaf/Software/Dev/robot/test/first_package/build/first_package/ament_uncrustify/uncrustify.txt" "--command" "/opt/ros/rolling/bin/ament_uncrustify" "--xunit-file" "/home/aaf/Software/Dev/robot/test/first_package/build/first_package/test_results/first_package/uncrustify.xunit.xml")
set_tests_properties(uncrustify PROPERTIES  LABELS "uncrustify;linter" TIMEOUT "60" WORKING_DIRECTORY "/home/aaf/Software/Dev/robot/test/first_package" _BACKTRACE_TRIPLES "/opt/ros/rolling/share/ament_cmake_test/cmake/ament_add_test.cmake;125;add_test;/opt/ros/rolling/share/ament_cmake_uncrustify/cmake/ament_uncrustify.cmake;70;ament_add_test;/opt/ros/rolling/share/ament_cmake_uncrustify/cmake/ament_cmake_uncrustify_lint_hook.cmake;34;ament_uncrustify;/opt/ros/rolling/share/ament_cmake_uncrustify/cmake/ament_cmake_uncrustify_lint_hook.cmake;0;;/opt/ros/rolling/share/ament_cmake_core/cmake/core/ament_execute_extensions.cmake;48;include;/opt/ros/rolling/share/ament_lint_auto/cmake/ament_lint_auto_package_hook.cmake;21;ament_execute_extensions;/opt/ros/rolling/share/ament_lint_auto/cmake/ament_lint_auto_package_hook.cmake;0;;/opt/ros/rolling/share/ament_cmake_core/cmake/core/ament_execute_extensions.cmake;48;include;/opt/ros/rolling/share/ament_cmake_core/cmake/core/ament_package.cmake;66;ament_execute_extensions;/home/aaf/Software/Dev/robot/test/first_package/CMakeLists.txt;35;ament_package;/home/aaf/Software/Dev/robot/test/first_package/CMakeLists.txt;0;")
add_test(xmllint "/usr/bin/python3.8" "-u" "/opt/ros/rolling/share/ament_cmake_test/cmake/run_test.py" "/home/aaf/Software/Dev/robot/test/first_package/build/first_package/test_results/first_package/xmllint.xunit.xml" "--package-name" "first_package" "--output-file" "/home/aaf/Software/Dev/robot/test/first_package/build/first_package/ament_xmllint/xmllint.txt" "--command" "/opt/ros/rolling/bin/ament_xmllint" "--xunit-file" "/home/aaf/Software/Dev/robot/test/first_package/build/first_package/test_results/first_package/xmllint.xunit.xml")
set_tests_properties(xmllint PROPERTIES  LABELS "xmllint;linter" TIMEOUT "60" WORKING_DIRECTORY "/home/aaf/Software/Dev/robot/test/first_package" _BACKTRACE_TRIPLES "/opt/ros/rolling/share/ament_cmake_test/cmake/ament_add_test.cmake;125;add_test;/opt/ros/rolling/share/ament_cmake_xmllint/cmake/ament_xmllint.cmake;50;ament_add_test;/opt/ros/rolling/share/ament_cmake_xmllint/cmake/ament_cmake_xmllint_lint_hook.cmake;18;ament_xmllint;/opt/ros/rolling/share/ament_cmake_xmllint/cmake/ament_cmake_xmllint_lint_hook.cmake;0;;/opt/ros/rolling/share/ament_cmake_core/cmake/core/ament_execute_extensions.cmake;48;include;/opt/ros/rolling/share/ament_lint_auto/cmake/ament_lint_auto_package_hook.cmake;21;ament_execute_extensions;/opt/ros/rolling/share/ament_lint_auto/cmake/ament_lint_auto_package_hook.cmake;0;;/opt/ros/rolling/share/ament_cmake_core/cmake/core/ament_execute_extensions.cmake;48;include;/opt/ros/rolling/share/ament_cmake_core/cmake/core/ament_package.cmake;66;ament_execute_extensions;/home/aaf/Software/Dev/robot/test/first_package/CMakeLists.txt;35;ament_package;/home/aaf/Software/Dev/robot/test/first_package/CMakeLists.txt;0;")