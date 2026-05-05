cmake_minimum_required(VERSION 3.16)
project(WbLSLightYieldToy LANGUAGES CXX)

find_package(Geant4 REQUIRED ui_all vis_all)
find_package(ROOT QUIET COMPONENTS Core Tree Hist RIO)

include(${Geant4_USE_FILE})

file(GLOB sources CONFIGURE_DEPENDS src/src/*.cc)
file(GLOB headers CONFIGURE_DEPENDS src/include/*.hh)

add_executable(wblsSim src/G4Manager.cc ${sources} ${headers})
target_include_directories(wblsSim PRIVATE src/include)
target_compile_features(wblsSim PRIVATE cxx_std_17)
target_link_libraries(wblsSim PRIVATE ${Geant4_LIBRARIES})

if(ROOT_FOUND)
  target_link_libraries(wblsSim PRIVATE ROOT::Core ROOT::Tree ROOT::Hist ROOT::RIO)
endif()

foreach(macro run_Cs137.mac run_Co60.mac run_Na22.mac vis.mac)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/macros/${macro}
                 ${CMAKE_CURRENT_BINARY_DIR}/${macro} COPYONLY)
endforeach()

install(TARGETS wblsSim RUNTIME DESTINATION bin)
install(DIRECTORY macros/ DESTINATION share/WbLSLightYieldToy/macros)
