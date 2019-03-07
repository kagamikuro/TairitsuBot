#pragma once

#include <ostream>
#include <istream>

#include "concurrency/guarded_container.h"

namespace utils
{
    size_t read_size(std::istream& stream);
    void write_size(std::ostream& stream, size_t size);

    namespace detail
    {
        template <typename T>
        struct BinaryIOImpl final
        {
            using Type = T;
            constexpr static bool specialized = false;
            static Type read(std::istream& stream)
            {
                Type result;
                stream.read(reinterpret_cast<char*>(&result), sizeof(Type));
                return result;
            }
            static void write(std::ostream& stream, const Type& value)
            {
                stream.write(reinterpret_cast<const char*>(&value), sizeof(Type));
            }
        };

        template <>
        struct BinaryIOImpl<const char*> final
        {
            using Type = const char*;
            constexpr static bool specialized = true;
            static Type read(std::istream& stream) = delete; // No read function for const char*
            static void write(std::ostream& stream, const Type& value)
            {
                const size_t length = std::strlen(value);
                write_size(stream, length);
                stream.write(value, length * sizeof(char));
            }
        };

        template <typename Char>
        struct BinaryIOImpl<std::basic_string<Char>> final
        {
            using Type = std::basic_string<Char>;
            constexpr static bool specialized = true;
            static Type read(std::istream& stream)
            {
                const size_t length = read_size(stream);
                if (!stream.good()) return {};
                Type result(length, 0);
                stream.read(result.data(), length * sizeof(Char));
                if (!stream.good()) return {};
                return result;
            }
            static void write(std::ostream& stream, const Type& value)
            {
                const size_t length = value.length();
                write_size(stream, length);
                stream.write(value.data(), length * sizeof(Char));
            }
        };

        template <typename Element>
        struct BinaryIOImpl<std::vector<Element>> final
        {
            using Type = std::vector<Element>;
            constexpr static bool specialized = true;
            static Type read(std::istream& stream)
            {
                const size_t size = read_size(stream);
                if (!stream.good()) return {};
                Type result(size);
                if constexpr (!BinaryIOImpl<Element>::specialized)
                {
                    stream.read(result.data(), size * sizeof(Element));
                    if (!stream.good()) return {};
                }
                else
                    for (Element& element : result)
                    {
                        element = BinaryIOImpl<Element>::read(stream);
                        if (!stream.good()) return {};
                    }
                return result;
            }
            static void write(std::ostream& stream, const Type& value)
            {
                const size_t size = value.size();
                write_size(stream, size);
                if constexpr (!BinaryIOImpl<Element>::specialized)
                    stream.write(value.data(), size * sizeof(Element));
                else
                    for (const Element& element : value)
                        BinaryIOImpl<Element>::write(stream, element);
            }
        };

        template <typename Element>
        struct BinaryIOImpl<std::unordered_set<Element>> final
        {
            using Type = std::unordered_set<Element>;
            constexpr static bool specialized = true;
            static Type read(std::istream& stream)
            {
                const size_t size = read_size(stream);
                if (!stream.good()) return {};
                Type result;
                for (size_t i = 0; i < size; i++)
                {
                    result.insert(BinaryIOImpl<Element>::read(stream));
                    if (!stream.good()) return {};
                }
                return result;
            }
            static void write(std::ostream& stream, const Type& value)
            {
                const size_t size = value.size();
                write_size(stream, size);
                for (const Element& element : value) BinaryIOImpl<Element>::write(stream, element);
            }
        };

        template <typename Key, typename Value>
        struct BinaryIOImpl<std::unordered_map<Key, Value>> final
        {
            using Type = std::unordered_map<Key, Value>;
            constexpr static bool specialized = true;
            static Type read(std::istream& stream)
            {
                const size_t size = read_size(stream);
                if (!stream.good()) return {};
                Type result;
                for (size_t i = 0; i < size; i++)
                {
                    result.insert({ BinaryIOImpl<Key>::read(stream), BinaryIOImpl<Value>::read(stream) });
                    if (!stream.good()) return {};
                }
                return result;
            }
            static void write(std::ostream& stream, const Type& value)
            {
                const size_t size = value.size();
                write_size(stream, size);
                for (const auto&[key, val] : value)
                {
                    BinaryIOImpl<Key>::write(stream, key);
                    BinaryIOImpl<Value>::write(stream, val);
                }
            }
        };

        template <typename Data>
        struct BinaryIOImpl<con::GuardedData<Data>> final
        {
            using Type = con::GuardedData<Data>;
            constexpr static bool specialized = true;
            static Type read(std::istream& stream)
            {
                Type result;
                const auto data = result.to_write();
                *data = BinaryIOImpl<Data>::read(stream);
                return result;
            }
            static void write(std::ostream& stream, const Type& value)
            {
                const auto data = value.to_read();
                BinaryIOImpl<Data>::write(stream, *data);
            }
        };
    }

    template <typename T> T read(std::istream& stream) { return detail::BinaryIOImpl<T>::read(stream); }
    template <typename T> void write(std::ostream& stream, const T& value) { return detail::BinaryIOImpl<T>::write(stream, value); }

    inline size_t read_size(std::istream& stream) { return size_t(read<uint64_t>(stream)); }
    inline void write_size(std::ostream& stream, const size_t size) { write(stream, uint64_t(size)); }
}
