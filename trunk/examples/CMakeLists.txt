

message("GUI system is ${GUI}")

add_executable(browser ../examples/browser.cpp)
target_link_libraries(browser amanita ${GTK2_LIBRARIES} ${WEBKIT_LIBRARIES})
add_executable(client ../examples/client.cpp)
target_link_libraries(client amanita ${CMAKE_THREAD_LIBS_INIT})
add_executable(horoscope ../examples/horoscope.c)
target_link_libraries(horoscope esoteric m)
add_executable(http ../examples/http.cpp)
target_link_libraries(http amanita ${CMAKE_THREAD_LIBS_INIT})
add_executable(server ../examples/server.cpp)
target_link_libraries(server amanita ${CMAKE_THREAD_LIBS_INIT})

