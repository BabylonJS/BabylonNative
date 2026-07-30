base-n provides encoding/decoding support for BaseX encoding schemes accessible through a standard STL-like iterator interface. Standard Base16, Base32, and Base64 are available out-of-the-box. Adding or modifying custom schemes is supported.

# Usage overview #

base-n is a small, single-header library which provides standard Base16, Base32, Base64, and custom Base-N encoding support.

The main functionality is delivered by the following functions in `bn` namespace:
```
template<class Iter1, class Iter2>
void encode_b16(Iter1 start, Iter1 end, Iter2 out);


template<class Iter1, class Iter2>
void encode_b32(Iter1 start, Iter1 end, Iter2 out);


template<class Iter1, class Iter2>
void encode_b64(Iter1 start, Iter1 end, Iter2 out);


template<class Iter1, class Iter2>
void decode_b16(Iter1 start, Iter1 end, Iter2 out);


template<class Iter1, class Iter2>
void decode_b32(Iter1 start, Iter1 end, Iter2 out);


template<class Iter1, class Iter2>
void decode_b64(Iter1 start, Iter1 end, Iter2 out);
```

In order to encode and decode data in `std::string` variable `in`, you can do the following:
```
bn::encode_b64(in.begin(), in.end(), back_inserter(encoded));
bn::decode_b64(encoded.begin(), encoded.end(), ostream_iterator<char>(cout, ""));
```

Should you find yourself lacking some encoding scheme or the default character mapping rules are not good for your use case, you can easily provide your own encoder. For that, you need to define a struct type which will describe the new encoding. Sample below:
```
struct b8_custom
{
        static size_t group_length()
        {
                return 3;
        }

        static char encode(int index)
        {
                const char* const dictionary = "01234567";
                assert(index < strlen(dictionary));
                return dictionary[index];
        }

        static char decode(char c)
        {
                if (c >= '0' && c <= '7') {
                        return c - '0';
                }
                return -1;
        }
};
...
string encoded;
bn::impl::encode<b8_custom>(in.begin(), in.end(), back_inserter(encoded));
bn::impl::decode<b8_custom>(encoded.begin(), encoded.end(), ostream_iterator<char>(cout, ""));
```

For a full working example, see `basen_example.cpp` file in `example` directory.
