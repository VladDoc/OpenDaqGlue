#pragma once

#include <string>
#include <fstream>

template <typename T, typename ...Args>
T coalesce(const T& first)
{
    return first;
}

template <typename T, typename ...Args>
auto coalesce(const T& first, const Args& ...args)
{
    return first ? first : coalesce(args...);
}



template <typename Container, typename Key>
bool contains(Container&& c, const Key& key )
{
   return c.find(key) != c.end();
}

template <typename Container, typename Key>
bool contains_uptr(Container&& c, const Key& key)
{
   std::unique_ptr<std::remove_pointer_t<Key>> compare_with {key};
   auto found = c.find(compare_with) != c.end();
   compare_with.release();

   return found;
}

template <typename Container, typename Key>
auto find_uptr(Container&& c, const Key& key)
{
   std::unique_ptr<std::remove_pointer_t<Key>> compare_with {key};
   auto found = c.find(compare_with);
   compare_with.release();

   return found;
}

template <typename Container, typename Key>
size_t erase_uptr(Container&& c, const Key& key)
{
   std::unique_ptr<std::remove_pointer_t<Key>> compare_with {key};
   auto index = c.erase(compare_with);
   compare_with.release();

   return index;
}

std::string read_file(std::string_view path)
{
    constexpr size_t read_size = 4096;
    auto stream = std::ifstream{path.data()};
    stream.exceptions(std::ios_base::badbit);

    auto out = std::string{};
    char buf[read_size]{};
    while (stream.read(&buf[0], read_size)) {
        out.append(buf, 0, stream.gcount());
    }
    out.append(buf, 0, stream.gcount());
    return out;
}
