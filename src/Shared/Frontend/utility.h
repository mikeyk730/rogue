#pragma once
#include <iosfwd>
#include <string>

template <typename T>
std::ostream& Write(std::ostream& out, T t) {
    out.write((char*)&t, sizeof(T));
    return out;
}

template <typename T>
std::istream& Read(std::istream& in, T* t) {
    in.read((char*)t, sizeof(T));
    return in;
}

std::ostream& WriteShortString(std::ostream& out, const std::string& s);
std::istream& ReadShortString(std::istream& in, std::string* s);

void throw_error(const std::string &msg);

void Delay(int ms);

uint32_t CharText(uint32_t ch);
uint32_t CharColor(uint32_t ch);
bool IsText(uint32_t ch);
char GetUnixChar(unsigned char c);
char GetRawCharFromData(uint32_t* data, int r, int c, int cols);
