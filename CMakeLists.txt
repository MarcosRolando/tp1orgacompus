cmake_minimum_required(VERSION 3.16)
project(tp1orga C)

add_executable(conway main.c Juego.c Juego.h JuegoDeLaVida.c JuegoDeLaVida.h
        Estado.h
        VersionC.c Cursor.c Cursor.h Coordenada.h)

target_compile_options (conway PRIVATE -std=c99 -Wall -Werror -pedantic -pedantic-errors
        -O0 -ggdb -DDEBUG -fno-inline)

target_link_options(conway PRIVATE -lm)