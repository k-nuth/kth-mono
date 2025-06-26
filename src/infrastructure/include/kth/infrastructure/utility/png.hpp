// Copyright (c) 2016-2024 Knuth Project developers.
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef KTH_INFRASTRUCTURE_PNG_HPP
#define KTH_INFRASTRUCTURE_PNG_HPP

#include <cstdint>
#include <istream>

#include <kth/infrastructure/compat.hpp>
#include <kth/infrastructure/define.hpp>
#include <kth/infrastructure/utility/color.hpp>
#include <kth/infrastructure/utility/data.hpp>

#ifdef WITH_PNG
#include <png.h>

namespace kth {

class KI_API png
{
public:
    static constexpr uint32_t margin = 2;
    static constexpr uint32_t dots_per_inch = 72;
    static constexpr uint32_t inches_per_meter = (100.0 / 2.54);

    static const color get_default_foreground() {
        static constexpr color default_foreground{ 0, 0, 0, 255 };
        return default_foreground;
    }

    static const color get_default_background() {
        static constexpr color default_background{ 255, 255, 255, 255 };
        return default_background;
    }

    /**
     * A method that takes encoded qrcode as a data chunk and writes it to
     * an output stream in png format with the default parameters.  The
     * size parameter specifies the number of dots (pixels) per qr code
     * modules.
     */
    static bool write_png(data_chunk const& data, uint32_t size,
        std::ostream& out);

    /**
     * A method that takes encoded qrcode data as a data chunk and writes
     * it to an output stream in png format with the specified parameters.
     */
    static bool write_png(data_chunk const& data, uint32_t size,
        uint32_t dots_per_inch, uint32_t margin, uint32_t inches_per_meter,
        const color& foreground, const color& background, std::ostream& out);

    /**
     * A method that reads encoded qrcode data via an input stream and
     * writes it to an output stream in png format with the default
     * parameters.  The size parameter specifies the number of dots
     * (pixels) per qr code modules.
     */
    static bool write_png(std::istream& in, uint32_t size, std::ostream& out);

    /**
     * A method that reads encoded qrcode data via an input stream and
     * writes it to an output stream in png format with the specified
     * parameters.
     */
    static bool write_png(std::istream& in, uint32_t size,
        uint32_t dots_per_inch, const uint32_t margin,
        uint32_t inches_per_meter, const color& foreground,
        const color& background, std::ostream& out);
};

} // namespace kth

#endif // WITH_PNG

#endif
