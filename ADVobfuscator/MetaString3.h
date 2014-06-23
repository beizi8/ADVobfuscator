//
//  MetaString3.h
//  ADVobfuscator
//
// Copyright (c) 2010-2014, Sebastien Andrivet
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef MetaString3_h
#define MetaString3_h

#include "Inline.h"
#include "Indexes.h"
#include "MetaRandom.h"

// Third implementation of an obfuscated string
// Limitation:
// - Hard-coded algorithm

namespace andrivet { namespace ADVobfuscator {

// Represents an obfuscated string, parametrized with a list of indexes and a key

template<typename Indexes, int K>
struct MetaString3;
    
// Partial specialization with a list of indexes I and a key K
    
template<int... I, int K>
struct MetaString3<Indexes<I...>, K>    // List of indexes generated by MakeIndexes
{
    // Constructor. Evaluated at compile time. Key is stored as the first element of the buffer
    constexpr ALWAYS_INLINE MetaString3(const char* str)
    : buffer_ {static_cast<char>(K), encrypt(str[I])...} { }
    
    // Runtime decryption. Most of the time, inlined
    inline const char* decrypt()
    {
        for(int i = 0; i < sizeof...(I); ++i)
            buffer_[i + 1] = decrypt(buffer_[i + 1]);
        buffer_[sizeof...(I) + 1] = 0;
        return buffer_ + 1;
    }
    
private:
    // Encrypt / decrypt a character of the original string with the key
    constexpr char key() const { return buffer_[0]; }
    constexpr char encrypt(char c) const { return c ^ key(); }
    constexpr char decrypt(char c) const { return encrypt(c); }
    
private:
    // Buffer to store the encrypted string + terminating null byte + key
    char buffer_[sizeof...(I) + 2];
};

// Helper to generate a key
template<int N>
struct MetaRandomChar3
{
    static const char value = static_cast<char>(1 + MetaRandom<N, 0xFF - 1>::value);
};
    
}}

// Prefix notation
#define OBFUSCATED3(str) (MetaString3<Make_Indexes<sizeof(str) - 1>::type, MetaRandomChar3<__COUNTER__>::value>(str).decrypt())

// It is still possible to define operator "" but it will always use the same key for all encrypted string.
// This is not what we want so use instead the macro ENCRYPTED3.

/*constexpr ALWAYS_INLINE const char* operator "" _obfuscated3(const char* str, size_t n)
{
    using I = Make_Indexes<32>::type;
    return MetaString3<I>(str).decrypt();
}*/

#endif
