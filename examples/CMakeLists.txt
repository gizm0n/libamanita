

message("GUI system is ${GUI}")

add_executable(browser browser.cpp)
target_link_libraries(browser amanita ${GTK2_LIBRARIES} ${WEBKIT_LIBRARIES})
add_executable(client client.cpp)
target_link_libraries(client amanita ${CMAKE_THREAD_LIBS_INIT} ${GTK2_LIBRARIES})
add_executable(entropy entropy.cpp)
target_link_libraries(entropy amanita)
add_executable(gui gui.cpp)
target_link_libraries(gui amanita ${GTK2_LIBRARIES} ${WEBKIT_LIBRARIES})
add_executable(horoscope horoscope.c)
target_link_libraries(horoscope esoteric m)
add_executable(http http.cpp)
target_link_libraries(http amanita ${CMAKE_THREAD_LIBS_INIT} ${GTK2_LIBRARIES})
add_executable(regex regex.cpp)
target_link_libraries(regex amanita)
add_executable(server server.cpp)
target_link_libraries(server amanita ${CMAKE_THREAD_LIBS_INIT} ${GTK2_LIBRARIES})

