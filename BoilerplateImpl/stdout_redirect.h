#pragma once

#include <iostream>
#include <string>

void PipeToString();
void DefaultStdOut();
std::string& GetBufferString();
void SetStringBuffer(std::string& buf);
bool IsPipedToString();
void EraseBuffer();
