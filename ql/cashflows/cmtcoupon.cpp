/*
 Copyright (C) 2006 Giorgio Facchinetti
 Copyright (C) 2006 Mario Pucci
 Copyright (C) 2006, 2007 StatPro Italia srl

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/license.shtml>.


 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 or FITNESS FOR A PARTICULAR PURPOSE. See the license for more details.
*/

#include <ql/cashflows/capflooredcoupon.hpp>
#include <ql/cashflows/cashflowvectors.hpp>
#include <ql/cashflows/cmtcoupon.hpp>
#include <ql/indexes/bondindex.hpp>
#include <utility>

namespace QuantLib {

    CmtCoupon::CmtCoupon(const Date& paymentDate,
                         Real nominal,
                         const Date& startDate,
                         const Date& endDate,
                         Natural fixingDays,
                         const ext::shared_ptr<BondIndex>& bondIndex,
                         Real gearing,
                         Spread spread,
                         const Date& refPeriodStart,
                         const Date& refPeriodEnd,
                         const DayCounter& dayCounter,
                         bool isInArrears,
                         const Date& exCouponDate)
    : CmsCoupon(paymentDate, nominal, startDate, endDate,
                         fixingDays, bondIndex-> dummySwapIndex(), gearing, spread,
                         refPeriodStart, refPeriodEnd,
                         dayCounter, isInArrears, exCouponDate),
      bondIndex_(bondIndex) {
        this->fixingDate_ = this->fixingDate();
    }

    CmtCoupon::CmtCoupon(const Date& paymentDate,
                         Real nominal,
                         const Date& startDate,
                         const Date& endDate,
                         const Date& fixingDate,
                         const ext::shared_ptr<BondIndex>& bondIndex,
                         Real gearing,
                         Spread spread,
                         const Date& refPeriodStart,
                         const Date& refPeriodEnd,
                         const DayCounter& dayCounter,
                         bool isInArrears,
                         const Date& exCouponDate)
    : CmsCoupon(paymentDate, nominal, startDate, endDate,
                         Null<Natural>(),
                         bondIndex->dummySwapIndex(), gearing, spread,
                         refPeriodStart, refPeriodEnd,
                         dayCounter, isInArrears, exCouponDate),
      bondIndex_(bondIndex) {
        this->fixingDate_ = fixingDate;
    }

    void CmtCoupon::accept(AcyclicVisitor& v) {
        auto* v1 = dynamic_cast<Visitor<CmtCoupon>*>(&v);
        if (v1 != nullptr)
            v1->visit(*this);
        else
            FloatingRateCoupon::accept(v);
    }

    CmtLeg::CmtLeg(Schedule schedule, ext::shared_ptr<BondIndex> bondIndex)
    : schedule_(std::move(schedule)), bondIndex_(std::move(bondIndex)) {
        QL_REQUIRE(bondIndex, "no index provided");
    }

    CmtLeg& CmtLeg::withNotionals(Real notional) {
        notionals_ = std::vector<Real>(1, notional);
        return *this;
    }

    CmtLeg& CmtLeg::withNotionals(const std::vector<Real>& notionals) {
        notionals_ = notionals;
        return *this;
    }

    CmtLeg& CmtLeg::withPaymentDayCounter(const DayCounter& dayCounter) {
        paymentDayCounter_ = dayCounter;
        return *this;
    }

    CmtLeg& CmtLeg::withPaymentAdjustment(BusinessDayConvention convention) {
        paymentAdjustment_ = convention;
        return *this;
    }

    CmtLeg& CmtLeg::withFixingDays(Natural fixingDays) {
        fixingDays_ = std::vector<Natural>(1, fixingDays);
        return *this;
    }

    CmtLeg& CmtLeg::withFixingDays(const std::vector<Natural>& fixingDays) {
        fixingDays_ = fixingDays;
        return *this;
    }

    CmtLeg& CmtLeg::withGearings(Real gearing) {
        gearings_ = std::vector<Real>(1, gearing);
        return *this;
    }

    CmtLeg& CmtLeg::withGearings(const std::vector<Real>& gearings) {
        gearings_ = gearings;
        return *this;
    }

    CmtLeg& CmtLeg::withSpreads(Spread spread) {
        spreads_ = std::vector<Spread>(1, spread);
        return *this;
    }

    CmtLeg& CmtLeg::withSpreads(const std::vector<Spread>& spreads) {
        spreads_ = spreads;
        return *this;
    }

    CmtLeg& CmtLeg::withCaps(Rate cap) {
        caps_ = std::vector<Rate>(1, cap);
        return *this;
    }

    CmtLeg& CmtLeg::withCaps(const std::vector<Rate>& caps) {
        caps_ = caps;
        return *this;
    }

    CmtLeg& CmtLeg::withFloors(Rate floor) {
        floors_ = std::vector<Rate>(1, floor);
        return *this;
    }

    CmtLeg& CmtLeg::withFloors(const std::vector<Rate>& floors) {
        floors_ = floors;
        return *this;
    }

    CmtLeg& CmtLeg::inArrears(bool flag) {
        inArrears_ = flag;
        return *this;
    }

    CmtLeg& CmtLeg::withZeroPayments(bool flag) {
        zeroPayments_ = flag;
        return *this;
    }

    CmtLeg& CmtLeg::withExCouponPeriod(
                                const Period& period,
                                const Calendar& cal,
                                BusinessDayConvention convention,
                                bool endOfMonth) {
        exCouponPeriod_ = period;
        exCouponCalendar_ = cal;
        exCouponAdjustment_ = convention;
        exCouponEndOfMonth_ = endOfMonth;
        return *this;
    }

    CmtLeg::operator Leg() const {
        return FloatingLeg<BondIndex, CmtCoupon, CappedFlooredCmtCoupon>(
                         schedule_, notionals_, bondIndex_, paymentDayCounter_,
                         paymentAdjustment_, fixingDays_, gearings_, spreads_,
                         caps_, floors_, inArrears_, zeroPayments_,
                         0, Calendar(),
                         exCouponPeriod_, exCouponCalendar_,
                         exCouponAdjustment_, exCouponEndOfMonth_);
   }

}
