set(LIB_SOURCES
  a_star.c
  mouse.c
)

set(MAIN_SOURCES
  main.c
)

set(HEADERS 
  ${CONFIG_HEADER}
  a_star.h
  mouse.h
  queue.h
)

add_library(micromouse STATIC ${LIB_SOURCES} ${HEADERS})
