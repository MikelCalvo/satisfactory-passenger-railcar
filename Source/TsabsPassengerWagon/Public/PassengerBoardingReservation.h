#pragma once
#include "CoreMinimal.h"

/** Server game-thread transaction only; not a replicated occupant or a player validator. */
class FPassengerBoardingReservation
{
public:
    bool IsReserved() const { return bReserved; }
    bool IsHeldFor(const UObject* Identity) const
    {
        return bReserved && IsValid(Identity) && Requester == Identity;
    }
    bool TryAcquire(const UObject* Identity)
    {
        if (bReserved || !IsValid(Identity)) return false;
        bReserved = true;
        Requester = Identity;
        return true;
    }
    bool Release(const UObject* Identity)
    {
        if (!bReserved || Requester != Identity) return false;
        Requester = nullptr;
        bReserved = false;
        return true;
    }
private:
    bool bReserved = false;
    // Opaque synchronous identity only; never dereferenced/kept across frames.
    // Raw comparison also lets the owning scope release after Destroy callbacks.
    const UObject* Requester = nullptr;
};

class FScopedPassengerBoardingReservation
{
public:
    FScopedPassengerBoardingReservation(FPassengerBoardingReservation& InReservation, const UObject* InIdentity)
        : Reservation(InReservation), Identity(InIdentity), bAcquired(Reservation.TryAcquire(Identity)) {}
    ~FScopedPassengerBoardingReservation() { if (bAcquired) Reservation.Release(Identity); }
    bool IsAcquired() const { return bAcquired; }
    FScopedPassengerBoardingReservation(const FScopedPassengerBoardingReservation&) = delete;
    FScopedPassengerBoardingReservation& operator=(const FScopedPassengerBoardingReservation&) = delete;
private:
    FPassengerBoardingReservation& Reservation;
    const UObject* Identity;
    bool bAcquired;
};
