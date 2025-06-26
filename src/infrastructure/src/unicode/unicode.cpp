// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <kth/infrastructure/unicode/unicode.hpp>

#include <cstddef>
#include <cstring>
#include <cwchar>
#include <iostream>
#include <mutex>
#include <stdexcept>
#include <string>

#include <boost/locale.hpp>

#include <kth/infrastructure/define.hpp>
// #include <kth/infrastructure/math/limits.hpp>
#include <kth/infrastructure/unicode/console_streambuf.hpp>
#include <kth/infrastructure/unicode/unicode_istream.hpp>
#include <kth/infrastructure/unicode/unicode_ostream.hpp>
#include <kth/infrastructure/utility/assert.hpp>
#include <kth/infrastructure/utility/data.hpp>
#include <kth/infrastructure/utility/limits.hpp>


#ifdef _MSC_VER
    #include <fcntl.h>
    #include <io.h>
#endif

namespace kth {

using namespace boost::locale;

// The width of utf16 stdio buffers.
constexpr size_t utf16_buffer_size = 256;

// Local definition for max number of bytes in a utf8 character.
constexpr size_t utf8_max_character_size = 4;

// Ensure console_streambuf::initialize is called only once.
static std::once_flag io_mutex;

// Ensure validate_localization is called only once.
static std::once_flag icu_mutex;

// Static initializer for kth::cin.
static std::istream& cin_stream()
{
    std::call_once(io_mutex, console_streambuf::initialize, utf16_buffer_size);
    static unicode_istream input(std::cin, std::wcin, utf16_buffer_size);
    return input;
}

// Static initializer for kth::cout.
static std::ostream& cout_stream()
{
    std::call_once(io_mutex, console_streambuf::initialize, utf16_buffer_size);
    static unicode_ostream output(std::cout, std::wcout, utf16_buffer_size);
    return output;
}

// Static initializer for kth::cerr.
static std::ostream& cerr_stream()
{
    std::call_once(io_mutex, console_streambuf::initialize, utf16_buffer_size);
    static unicode_ostream error(std::cerr, std::wcerr, utf16_buffer_size);
    return error;
}

// Use kth::cin in place of std::cin, etc.
std::istream& cin = cin_stream();
std::ostream& cout = cout_stream();
std::ostream& cerr = cerr_stream();

#ifdef WITH_ICU

// The backend selection is ignored if invalid (in this case on Windows).
static std::string normal_form(std::string const& value, norm_type form)
{
    auto backend = localization_backend_manager::global();
    backend.select(KI_LOCALE_BACKEND);
    const generator locale(backend);
    return normalize(value, form, locale(KI_LOCALE_UTF8));
}

// One time verifier of the localization backend manager. This is
// necessary because boost::normalize will fail silently to perform
// normalization if the ICU dependency is missing.
static void validate_localization()
{
    auto const ascii_space = "> <";
    auto const ideographic_space = ">　<";
    auto const normal = normal_form(ideographic_space, norm_type::norm_nfkd);

    if (normal != ascii_space)
        throw std::runtime_error(
            "Unicode normalization test failed, a dependency may be missing.");
}

// Normalize strings using unicode nfc normalization.
std::string to_normal_nfc_form(std::string const& value)
{
    std::call_once(icu_mutex, validate_localization);
    return normal_form(value, norm_type::norm_nfc);
}

// Normalize strings using unicode nfkd normalization.
std::string to_normal_nfkd_form(std::string const& value)
{
    std::call_once(icu_mutex, validate_localization);
    return normal_form(value, norm_type::norm_nfkd);
}

#endif

// Convert wmain environment to utf8 main environment.
data_chunk to_utf8(wchar_t* environment[])
{
    int count;
    for (count = 0; environment[count] != nullptr; count++); //NOLINT

    return to_utf8(count, environment);
}

// Convert wmain parameters to utf8 main parameters.
data_chunk to_utf8(int argc, wchar_t* argv[]) {
    auto const arg_count_ex = safe_to_unsigned<size_t>(argc);
    if ( ! arg_count_ex) {
        return {};
    }

    auto const arg_count = *arg_count_ex;

    // Convert each arg and determine the payload size.
    size_t payload_size = 0;
    std::vector<std::string> collection(arg_count + 1);

    for (size_t arg = 0; arg < arg_count; arg++) {
        collection[arg] = to_utf8(argv[arg]);
        payload_size += collection[arg].size() + 1;
    }

    // TODO: unsafe multiplication.
    // Determine the index size.
    auto const arg_count_p1_ex = safe_add(arg_count, size_t{1});
    if ( ! arg_count_p1_ex) {
        return {};
    }
    auto const index_size = *arg_count_p1_ex * sizeof(void*);

    // Allocate the new buffer.
    auto const buffer_size_ex = safe_add(index_size, payload_size);
    if ( ! buffer_size_ex) {
        return {};
    }
    auto const buffer_size = *buffer_size_ex;
    data_chunk buffer(buffer_size, 0x00);
    buffer.resize(buffer_size);

    // Set pointers into index and payload buffer sections.
    auto index = reinterpret_cast<char**>(&buffer[0]);
    auto arguments = reinterpret_cast<char*>(&buffer[index_size]);

    // Clone the converted collection into the new narrow argv.
    for (size_t arg = 0; arg < arg_count; arg++) {
        index[arg] = arguments;
        auto const size = collection[arg].size();
        std::copy_n(collection[arg].begin(), size, index[arg]);
        auto const size_p1 = safe_add(size, size_t{ 1 });
        if ( ! size_p1) {
            return {};
        }
        arguments += *size_p1;
    }

    return buffer;
}

// Convert wstring to utf8 string.
std::string to_utf8(const std::wstring& wide)
{
    using namespace boost::locale;
    return conv::utf_to_utf<char>(wide, conv::method_type::stop);
}

// Convert wchar_t buffer to utf8 char buffer.
size_t to_utf8(char out[], size_t out_bytes, const wchar_t in[],
    size_t in_chars)
{
    KTH_ASSERT(in != nullptr);
    KTH_ASSERT(out != nullptr);
    KTH_ASSERT(out_bytes >= utf8_max_character_size * in_chars);

    if (in_chars == 0) {
        return 0;
}

    size_t bytes = 0;

    try
    {
        auto const narrow = to_utf8({ in, &in[in_chars] });
        bytes = narrow.size();

        if (bytes <= out_bytes) {
            memcpy(out, narrow.data(), bytes);
}
    }
    catch (const boost::locale::conv::conversion_error&) {
        bytes = 0;
    }

    if (bytes == 0) {
        throw std::istream::failure("utf-16 to utf-8 conversion failure");
}

    if (bytes > out_bytes) {
        throw std::ios_base::failure("utf8 buffer is too small");
}

    return bytes;
}

// All non-leading bytes of utf8 have the same two bit prefix.
static bool is_utf8_trailing_byte(char byte)
{
    // 10xxxxxx
    return ((0xC0 & byte) == 0x80);
}

// Determine if the full sequence is a valid utf8 character.
static bool is_utf8_character_sequence(char const sequence[], uint8_t bytes)
{
    KTH_ASSERT(bytes <= utf8_max_character_size);

    // See tools.ietf.org/html/rfc3629#section-3 for definition.
    switch (bytes) {
        case 1:
            // 0xxxxxxx
            return
                ((0x80 & sequence[0]) == 0x00);
        case 2:
            // 110xxxxx 10xxxxxx
            return
                ((0xE0 & sequence[0]) == 0xC0) &&
                is_utf8_trailing_byte(sequence[1]);
        case 3:
            // 1110xxxx 10xxxxxx 10xxxxxx
            return
                ((0xF0 & sequence[0]) == 0xE0) &&
                is_utf8_trailing_byte(sequence[1]) &&
                is_utf8_trailing_byte(sequence[2]);
        case 4:
            // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
            return
                ((0xF8 & sequence[0]) == 0xF0) &&
                is_utf8_trailing_byte(sequence[1]) &&
                is_utf8_trailing_byte(sequence[2]) &&
                is_utf8_trailing_byte(sequence[3]);
        default:;
    }

    return false;
}

// Determine if the text is terminated by a valid utf8 character.
static bool is_terminal_utf8_character(char const text[], size_t size)
{
    KTH_ASSERT(text != nullptr);

    // Walk back up to the max length of a utf8 character.
    for (uint8_t length = 1; length <= utf8_max_character_size &&
        length < size; length++) {
        auto const start = size - length;
        auto const sequence = &text[start];
        if (is_utf8_character_sequence(sequence, length)) {
            return true;
}
    }

    return false;
}

// This optimizes character split detection by taking advantage of utf8
// character recognition so we don't have to convert in full up to 3 times.
// This does not guaratee that the entire string is valid as utf8, just that a
// returned offset follows the last byte of a utf8 terminal char if it exists.
static uint8_t offset_to_terminal_utf8_character(char const text[], size_t size)
{
    KTH_ASSERT(text != nullptr);

    // Walk back up to the max length of a utf8 character.
    for (uint8_t unread = 0; unread < utf8_max_character_size &&
        unread < size; unread++) {
        auto const length = size - unread;
        if (is_terminal_utf8_character(text, length)) {
            return unread;
}
    }

    return 0;
}

// Convert utf8 char buffer to wchar_t buffer, with truncation handling.
size_t to_utf16(wchar_t out[], size_t out_chars, char const in[],
    size_t in_bytes, uint8_t& truncated)
{
    KTH_ASSERT(in != nullptr);
    KTH_ASSERT(out != nullptr);
    KTH_ASSERT(out_chars >= in_bytes);

    // Calculate a character break offset of 0..4 bytes.
    truncated = offset_to_terminal_utf8_character(in, in_bytes);

    if (in_bytes == 0) {
        return 0;
}

    size_t chars = 0;

    try
    {
        auto const wide = to_utf16({ in, &in[in_bytes - truncated] });
        chars = wide.size();

        if (chars <= out_chars) {
            wmemcpy(out, wide.data(), chars);
}
    }
    catch (const boost::locale::conv::conversion_error&) {
        chars = 0;
    }

    if (chars == 0) {
        throw std::ostream::failure("utf-8 to utf-16 conversion failure");
}

    if (chars > out_chars) {
        throw std::ios_base::failure("utf16 buffer is too small");
}

    return chars;
}

// Convert utf8 string to wstring.
std::wstring to_utf16(std::string const& narrow)
{
    using namespace boost::locale;
    return conv::utf_to_utf<wchar_t>(narrow, conv::method_type::stop);
}

LCOV_EXCL_START("Untestable but visually-verifiable section.")

static void set_utf8_stdio(FILE* file)
{
#ifdef _MSC_VER
    if (_setmode(_fileno(file), _O_U8TEXT) == -1)
        throw std::runtime_error("Could not set STDIO to utf8 mode.");
#endif
}

static void set_binary_stdio(FILE* file)
{
#ifdef _MSC_VER
    if (_setmode(_fileno(file), _O_BINARY) == -1)
        throw std::runtime_error("Could not set STDIO to binary mode.");
#endif
}

// Set stdio to use UTF8 translation on Windows.
void set_utf8_stdio()
{
    set_utf8_stdin();
    set_utf8_stdout();
    set_utf8_stderr();
}

// Set stdio to use UTF8 translation on Windows.
void set_utf8_stdin()
{
    set_utf8_stdio(stdin);
}

// Set stdio to use UTF8 translation on Windows.
void set_utf8_stdout()
{
    set_utf8_stdio(stdout);
}

// Set stdio to use UTF8 translation on Windows.
void set_utf8_stderr()
{
    set_utf8_stdio(stderr);
}

// Set stdio to use UTF8 translation on Windows.
void set_binary_stdin()
{
    set_binary_stdio(stdin);
}

// Set stdio to use UTF8 translation on Windows.
void set_binary_stdout()
{
    set_binary_stdio(stdout);
}

LCOV_EXCL_STOP()

} // namespace kth
