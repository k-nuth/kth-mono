// Copyright (c) 2016-2025 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_TEST_HELPERS_HPP
#define KTH_INFRASTRUCTURE_TEST_HELPERS_HPP

#include <ostream>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>

#include <kth/infrastructure/hash_define.hpp>
#include <kth/infrastructure/formats/base_16.hpp>
#include <kth/infrastructure/utility/data.hpp>
#include <kth/infrastructure/utility/byte_reader.hpp>
#include <kth/infrastructure/utility/istream_reader.hpp>

// Test macros
#define CHECK_MESSAGE(cond, msg) do { INFO(msg); CHECK(cond); } while((void)0, 0)
#define REQUIRE_MESSAGE(cond, msg) do { INFO(msg); REQUIRE(cond); } while((void)0, 0)

// Pretty printing for infrastructure types
namespace kth {

inline
std::ostream& operator<<(std::ostream& os, hash_digest const& x) {
    os << encode_hash(x);
    return os;
}

} // namespace kth

// Generic test helpers for infrastructure types
namespace kth {

// Generic entity_from_data that auto-detects the correct from_data signature
template <typename T, typename... Args>
bool entity_from_data(T& result, kth::data_chunk const& data, Args&&... args) {
    kth::byte_reader reader(data);
    
    // Try with provided args first (e.g., wire=true, version, etc.)
    if constexpr (sizeof...(args) > 0) {
        if constexpr (requires { T::from_data(reader, std::forward<Args>(args)...); }) {
            auto exp_result = T::from_data(reader, std::forward<Args>(args)...);
            if (exp_result) {
                // Use placement new to handle types without copy assignment
                result.~T();
                new (&result) T(std::move(*exp_result));
                return true;
            }
            return false;
        }
    }
    
    // Try with default wire=true for chain types that need it
    if constexpr (requires { T::from_data(reader, true); }) {
        auto exp_result = T::from_data(reader, true);
        if (exp_result) {
            result.~T();
            new (&result) T(std::move(*exp_result));
            return true;
        }
        return false;
    }
    // Try without extra args for simple types
    else if constexpr (requires { T::from_data(reader); }) {
        auto exp_result = T::from_data(reader);
        if (exp_result) {
            result.~T();
            new (&result) T(std::move(*exp_result));
            return true;
        }
        return false;
    }
    
    return false;
}

// Overload for stream-based deserialization (std::istream)
template <typename T, typename... Args>
bool entity_from_data(T& result, std::istream& stream, Args&&... args) {
    kth::istream_reader reader(stream);
    
    // Try with provided args first
    if constexpr (sizeof...(args) > 0) {
        if constexpr (requires { T::from_data(reader, std::forward<Args>(args)...); }) {
            auto exp_result = T::from_data(reader, std::forward<Args>(args)...);
            if (exp_result) {
                result.~T();
                new (&result) T(std::move(*exp_result));
                return true;
            }
            return false;
        }
    }
    
    // Try with default wire=true
    if constexpr (requires { T::from_data(reader, true); }) {
        auto exp_result = T::from_data(reader, true);
        if (exp_result) {
            result.~T();
            new (&result) T(std::move(*exp_result));
            return true;
        }
        return false;
    }
    // Try without extra args
    else if constexpr (requires { T::from_data(reader); }) {
        auto exp_result = T::from_data(reader);
        if (exp_result) {
            result.~T();
            new (&result) T(std::move(*exp_result));
            return true;
        }
        return false;
    }
    
    return false;
}

// Overload for direct istream_reader deserialization
template <typename T, typename... Args>
bool entity_from_data(T& result, kth::istream_reader& reader, Args&&... args) {
    // Try with provided args first
    if constexpr (sizeof...(args) > 0) {
        if constexpr (requires { T::from_data(reader, std::forward<Args>(args)...); }) {
            auto exp_result = T::from_data(reader, std::forward<Args>(args)...);
            if (exp_result) {
                result.~T();
                new (&result) T(std::move(*exp_result));
                return true;
            }
            return false;
        }
    }
    
    // Try with default wire=true
    if constexpr (requires { T::from_data(reader, true); }) {
        auto exp_result = T::from_data(reader, true);
        if (exp_result) {
            result.~T();
            new (&result) T(std::move(*exp_result));
            return true;
        }
        return false;
    }
    // Try without extra args
    else if constexpr (requires { T::from_data(reader); }) {
        auto exp_result = T::from_data(reader);
        if (exp_result) {
            result.~T();
            new (&result) T(std::move(*exp_result));
            return true;
        }
        return false;
    }
    
    return false;
}

// Overload for direct byte_reader deserialization
template <typename T, typename... Args>
bool entity_from_data(T& result, kth::byte_reader& reader, Args&&... args) {
    // Try with provided args first
    if constexpr (sizeof...(args) > 0) {
        if constexpr (requires { T::from_data(reader, std::forward<Args>(args)...); }) {
            auto exp_result = T::from_data(reader, std::forward<Args>(args)...);
            if (exp_result) {
                result.~T();
                new (&result) T(std::move(*exp_result));
                return true;
            }
            return false;
        }
    }
    
    // Try with default wire=true
    if constexpr (requires { T::from_data(reader, true); }) {
        auto exp_result = T::from_data(reader, true);
        if (exp_result) {
            result.~T();
            new (&result) T(std::move(*exp_result));
            return true;
        }
        return false;
    }
    // Try without extra args
    else if constexpr (requires { T::from_data(reader); }) {
        auto exp_result = T::from_data(reader);
        if (exp_result) {
            result.~T();
            new (&result) T(std::move(*exp_result));
            return true;
        }
        return false;
    }
    
    return false;
}

// Backward compatibility overload for legacy test pattern: entity_from_data(instance, version, data)
template <typename T, typename Version>
bool entity_from_data(T& result, Version version, kth::data_chunk const& data) {
    return entity_from_data(result, data, version);
}

// Backward compatibility overload for stream version
template <typename T, typename Version>
bool entity_from_data(T& result, Version version, std::istream& stream) {
    return entity_from_data(result, stream, version);
}

// Backward compatibility overload for reader versions
template <typename T, typename Version>
bool entity_from_data(T& result, Version version, kth::istream_reader& reader) {
    return entity_from_data(result, reader, version);
}

template <typename T, typename Version>
bool entity_from_data(T& result, Version version, kth::byte_reader& reader) {
    return entity_from_data(result, reader, version);
}

// Generic create function that auto-detects the correct from_data signature
template <typename T, typename... Args>
T create(kth::data_chunk const& data, Args&&... args) {
    kth::byte_reader reader(data);
    
    // Try with provided args first
    if constexpr (sizeof...(args) > 0) {
        if constexpr (requires { T::from_data(reader, std::forward<Args>(args)...); }) {
            auto exp_result = T::from_data(reader, std::forward<Args>(args)...);
            if (exp_result) {
                return std::move(*exp_result);
            }
            return T{};
        }
    }
    
    // Try with default wire=true for chain types
    if constexpr (requires { T::from_data(reader, true); }) {
        auto exp_result = T::from_data(reader, true);
        if (exp_result) {
            return std::move(*exp_result);
        }
        return T{};
    }
    // Try without extra args
    else if constexpr (requires { T::from_data(reader); }) {
        auto exp_result = T::from_data(reader);
        if (exp_result) {
            return std::move(*exp_result);
        }
        return T{};
    }
    
    return T{};
}

// Overload for stream-based creation (std::istream)
template <typename T, typename... Args>
T create(std::istream& stream, Args&&... args) {
    kth::istream_reader reader(stream);
    
    // Try with provided args first
    if constexpr (sizeof...(args) > 0) {
        if constexpr (requires { T::from_data(reader, std::forward<Args>(args)...); }) {
            auto exp_result = T::from_data(reader, std::forward<Args>(args)...);
            if (exp_result) {
                return std::move(*exp_result);
            }
            return T{};
        }
    }
    
    // Try with default wire=true
    if constexpr (requires { T::from_data(reader, true); }) {
        auto exp_result = T::from_data(reader, true);
        if (exp_result) {
            return std::move(*exp_result);
        }
        return T{};
    }
    // Try without extra args
    else if constexpr (requires { T::from_data(reader); }) {
        auto exp_result = T::from_data(reader);
        if (exp_result) {
            return std::move(*exp_result);
        }
        return T{};
    }
    
    return T{};
}

// Overload for direct istream_reader creation
template <typename T, typename... Args>
T create(kth::istream_reader& reader, Args&&... args) {
    // Try with provided args first
    if constexpr (sizeof...(args) > 0) {
        if constexpr (requires { T::from_data(reader, std::forward<Args>(args)...); }) {
            auto exp_result = T::from_data(reader, std::forward<Args>(args)...);
            if (exp_result) {
                return std::move(*exp_result);
            }
            return T{};
        }
    }
    
    // Try with default wire=true
    if constexpr (requires { T::from_data(reader, true); }) {
        auto exp_result = T::from_data(reader, true);
        if (exp_result) {
            return std::move(*exp_result);
        }
        return T{};
    }
    // Try without extra args
    else if constexpr (requires { T::from_data(reader); }) {
        auto exp_result = T::from_data(reader);
        if (exp_result) {
            return std::move(*exp_result);
        }
        return T{};
    }
    
    return T{};
}

// Overload for direct byte_reader creation
template <typename T, typename... Args>
T create(kth::byte_reader& reader, Args&&... args) {
    // Try with provided args first
    if constexpr (sizeof...(args) > 0) {
        if constexpr (requires { T::from_data(reader, std::forward<Args>(args)...); }) {
            auto exp_result = T::from_data(reader, std::forward<Args>(args)...);
            if (exp_result) {
                return std::move(*exp_result);
            }
            return T{};
        }
    }
    
    // Try with default wire=true
    if constexpr (requires { T::from_data(reader, true); }) {
        auto exp_result = T::from_data(reader, true);
        if (exp_result) {
            return std::move(*exp_result);
        }
        return T{};
    }
    // Try without extra args
    else if constexpr (requires { T::from_data(reader); }) {
        auto exp_result = T::from_data(reader);
        if (exp_result) {
            return std::move(*exp_result);
        }
        return T{};
    }
    
    return T{};
}

// Backward compatibility overload for legacy test pattern: create<T>(version, data)
template <typename T, typename Version>
T create(Version version, kth::data_chunk const& data) {
    return create<T>(data, version);
}

// Backward compatibility overload for stream version
template <typename T, typename Version>
T create(Version version, std::istream& stream) {
    return create<T>(stream, version);
}

// Backward compatibility overload for reader versions
template <typename T, typename Version>
T create(Version version, kth::istream_reader& reader) {
    return create<T>(reader, version);
}

template <typename T, typename Version>
T create(Version version, kth::byte_reader& reader) {
    return create<T>(reader, version);
}

} // namespace kth

#endif // KTH_INFRASTRUCTURE_TEST_HELPERS_HPP
