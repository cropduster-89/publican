@echo off
clang -Wall -g -gcodeview -std=c18 -O0 -Iinc -Llib test/tests.c src/file_parser.c src/file_builder.c -o test.exe -lcmocka -llibxml2