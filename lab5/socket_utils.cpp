#include "socket_utils.h"

#include <unistd.h>
#include <stdio.h>
#include <string>
#include <string.h>

void read_from_socket(int ConnectFD, char *buffer, int size) {
    bzero(buffer, size);
    int n = read(ConnectFD, buffer, size);
    if (n < 0)
        perror("ERROR reading from socket");
}

void write_to_socket(int ConnectFD, char *buffer, int size) {
    int n = write(ConnectFD, buffer, size);
    if (n < 0)
        perror("ERROR writing to socket");
}

void fill_buffer_w_buffers(std::vector<char> &buffer, std::vector<std::pair<char *, int> > &buffers_sizes) {
    for (int i = 0; i < buffers_sizes.size(); ++i) {
        for (int j = 0; j < buffers_sizes[i].second; ++j) {
            buffer.push_back(buffers_sizes[i].first[j]);
        }
    }
}

void copy_int_str_to_char_array(std::string &int_str, char *buffer, int size_buffer) {
    memset(buffer, '0', size_buffer);
    int j = size_buffer - 1;
    for (int i = int_str.size() - 1; i >= 0; --i) {
        buffer[j] = int_str[i];
        --j;
    }
}

void append_string_to_vector(std::string &str, std::vector<char> &vec) {
    for (int i = 0; i < str.size(); ++i) {
        vec.push_back(str[i]);
    }
}
