#include "PassengerAccessPolicy.h"
#include <bit>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>

struct Case { const char* Name; float Speed; bool ExpectedAllowed; };
int main()
{
    constexpr double LimitCms = 30.0 * 100000.0 / 3600.0;
    const float Below = static_cast<float>(LimitCms);
    const float Above = std::nextafter(Below, std::numeric_limits<float>::infinity());
    const std::vector<Case> Cases{
        {"zero", 0.0f, true}, {"negative zero", -0.0f, true},
        {"slow forward", 1.0f, true}, {"slow reverse", -1.0f, true},
        {"denormal forward", std::numeric_limits<float>::denorm_min(), true},
        {"denormal reverse", -std::numeric_limits<float>::denorm_min(), true},
        {"9 km/h", 250.0f, true}, {"reverse 9 km/h", -250.0f, true},
        {"float immediately below 30", Below, true},
        {"reverse immediately below 30", -Below, true},
        {"first float above exact 30", Above, false},
        {"reverse first float above exact 30", -Above, false},
        {"10.8 km/h now permitted", 300.0f, true}, {"reverse 10.8 km/h now permitted", -300.0f, true},
        {"27 km/h", 750.0f, true}, {"reverse 27 km/h", -750.0f, true},
        {"28.8 km/h", 800.0f, true}, {"reverse 28.8 km/h", -800.0f, true},
        {"32.4 km/h", 900.0f, false}, {"reverse 32.4 km/h", -900.0f, false},
        {"largest finite", std::numeric_limits<float>::max(), false},
        {"negative largest finite", -std::numeric_limits<float>::max(), false},
        {"NaN", std::numeric_limits<float>::quiet_NaN(), false},
        {"negative NaN", std::bit_cast<float>(0xffc00000u), false},
        {"positive infinity", std::numeric_limits<float>::infinity(), false},
        {"negative infinity", -std::numeric_limits<float>::infinity(), false}
    };
    int Failures = 0;
    for (const auto& C : Cases)
    {
        if (TsabsPassenger::IsPassengerAccessSpeedAllowed(C.Speed) != C.ExpectedAllowed)
        { std::cerr << "FAIL: " << C.Name << '\n'; ++Failures; }
    }
    if (Failures) { std::cerr << Failures << " speed-policy checks failed\n"; return 1; }
    std::cout << "PASSENGER_ACCESS_SPEED_POLICY_OK: " << Cases.size() << " checks passed\n";
    return 0;
}

