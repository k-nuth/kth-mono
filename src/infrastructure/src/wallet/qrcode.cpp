// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifdef WITH_QRENCODE

#include <kth/infrastructure/wallet/qrcode.hpp>

#include <iostream>
#include <string>
#include <kth/infrastructure/constants.hpp>
#include <kth/infrastructure/define.hpp>
#include <kth/infrastructure/utility/data.hpp>
#include <kth/infrastructure/utility/container_sink.hpp>
#include <kth/infrastructure/utility/container_source.hpp>
#include <kth/infrastructure/utility/istream_reader.hpp>
#include <kth/infrastructure/utility/ostream_writer.hpp>


namespace kth::infrastructure::wallet {

data_chunk qr::encode(data_chunk const& data)
{
    return qr::encode(data, version, level, mode, case_sensitive);
}

data_chunk qr::encode(data_chunk const& data, uint32_t version,
    error_recovery_level level, encode_mode mode, bool case_sensitive)
{
    data_chunk out;
    data_sink ostream(out);
    data_source istream(data);

    if (qr::encode(istream, version, level, mode, case_sensitive, ostream))
        return out;

    return {};
}

bool qr::encode(std::istream& in, std::ostream& out)
{
    return qr::encode(in, version, level, mode, case_sensitive, out);
}

bool qr::encode(std::istream& in, uint32_t version, error_recovery_level level,
    encode_mode mode, bool case_sensitive, std::ostream& out)
{
    std::string qr_string;
    getline(in, qr_string);

    auto const qrcode = QRcode_encodeString(qr_string.c_str(), version,
        level, mode, case_sensitive);

    if (qrcode == nullptr)
        return false;

    if (kth::max_size_t / qrcode->width < qrcode->width)
        return false;

    auto const area = qrcode->width * qrcode->width;
    auto width_ptr = reinterpret_cast<uint8_t const*>(&qrcode->width);
    auto version_ptr = reinterpret_cast<uint8_t const*>(&qrcode->version);

    // Write out raw format of QRcode structure (defined in qrencode.h).
    // Format written is:
    // uint32_t version
    // uint32_t width
    // unsigned char* data (of width^2 length)
    ostream_writer sink(out);
    sink.write_bytes(version_ptr, sizeof(uint32_t));
    sink.write_bytes(width_ptr, sizeof(uint32_t));
    sink.write_bytes(qrcode->data, area);
    out.flush();

    return true;
}

} // namespace kth::infrastructure::wallet

#endif // WITH_QRENCODE
