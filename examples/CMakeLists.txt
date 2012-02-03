

MESSAGE("GUI system is ${GUI}")


ADD_EXECUTABLE(browser ../examples/browser.cpp)
TARGET_LINK_LIBRARIES(browser amanita ${GTK_LIBRARIES} ${WEBKIT_LIBRARIES})
ADD_EXECUTABLE(client ../examples/client.cpp)
TARGET_LINK_LIBRARIES(client amanita ${CMAKE_THREAD_LIBS_INIT})
ADD_EXECUTABLE(horoscope ../examples/horoscope.c)
TARGET_LINK_LIBRARIES(horoscope esoteric m)
ADD_EXECUTABLE(http ../examples/http.cpp)
TARGET_LINK_LIBRARIES(http amanita ${CMAKE_THREAD_LIBS_INIT})
ADD_EXECUTABLE(server ../examples/server.cpp)
TARGET_LINK_LIBRARIES(server amanita ${CMAKE_THREAD_LIBS_INIT})

