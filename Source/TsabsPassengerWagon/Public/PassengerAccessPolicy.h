#pragma once

#include <bit>
#include <cstdint>
#include <limits>

namespace TsabsPassenger
{
    inline constexpr double PassengerAccessSpeedLimitKmh = 30.0;
    inline constexpr double PassengerAccessSpeedLimitCmPerSecond =
        PassengerAccessSpeedLimitKmh * 100000.0 / 3600.0;

    inline bool IsPassengerAccessSpeedAllowed(float SpeedCmPerSecond) noexcept
    {
        static_assert(sizeof(float) == sizeof(std::uint32_t));
        static_assert(std::numeric_limits<float>::is_iec559);
        // Classification via bits survives fast-math; NaN/inf never authorize
        // entry or exit. Strip only the sign so reverse obeys the same limit.
        const std::uint32_t Magnitude = std::bit_cast<std::uint32_t>(SpeedCmPerSecond) & 0x7fff'ffffu;
        if (Magnitude >= 0x7f80'0000u) return false;
        // Compare in double against the exact-unit limit, rather than rounding
        // the configured km/h limit down to float32 and rejecting a valid neighbour.
        return static_cast<double>(std::bit_cast<float>(Magnitude)) < PassengerAccessSpeedLimitCmPerSecond;
    }
}

