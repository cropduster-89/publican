@echo off
clang -O0 -Wall -g -gcodeview -Iinc -Llib -std=c18 -Iinc -Llib src/main.c src/file_parser.c src/file_builder.c -o asset_builder.exe -llibxml2