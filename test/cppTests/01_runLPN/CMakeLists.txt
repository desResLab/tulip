ADD_EXECUTABLE(runLPN ${CMAKE_SOURCE_DIR}/test/cppTests/01_runLPN/test_RunLPNModel.cpp)
TARGET_LINK_LIBRARIES(runLPN tulip armadillo)
