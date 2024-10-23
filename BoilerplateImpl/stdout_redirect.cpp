#include "stdout_redirect.h"

#include <sstream>

static std::stringstream bufstream;
static std::streambuf*   coutbuf = nullptr;

void PipeToString()
{
    if(coutbuf) return; // no double piping
    coutbuf = std::cout.rdbuf(); //save old buf
    std::cout.rdbuf(bufstream.rdbuf());
}
void DefaultStdOut()
{
    if(coutbuf) std::cout.rdbuf(coutbuf);
    coutbuf = nullptr;
}

std::string& GetBufferString()
{
    static std::string static_str;
    return static_str = bufstream.str();
}

void EraseBuffer()
{
    bufstream.str("");
}

void SetStringBuffer(std::string& buf)
{
    bufstream.str(buf);
}

bool IsPipedToString()
{
    return coutbuf != nullptr;
}
