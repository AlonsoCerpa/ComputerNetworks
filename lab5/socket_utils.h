#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <vector>
#include <string>

void read_from_socket(int ConnectFD, char *buffer, int size);
void write_to_socket(int ConnectFD, char *buffer, int size);
void fill_buffer_w_buffers(std::vector<char> &buffer, std::vector<std::pair<char *, int> > &buffers_sizes);
void copy_int_str_to_char_array(std::string &int_str, char *buffer, int size_buffer);
void append_string_to_vector(std::string &str, std::vector<char> &vec);

#endif

