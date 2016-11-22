#include <Project_init.h>

int get_file_size(std::string filename) {
    FILE *p_file = NULL;
    p_file = fopen(filename.c_str(), "rb");
    fseek(p_file, 0, SEEK_END);
    int size = ftell(p_file);
    fclose(p_file);
    return size;
}