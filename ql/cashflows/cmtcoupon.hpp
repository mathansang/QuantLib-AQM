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
 or FITNESS FOR A PARTICULAR PURPOSE. See the license for more details. */

/*! \file cmscoupon.hpp
    \brief CMS coupon
*/

#ifndef quantlib_cmt_coupon_hpp
#define quantlib_cmt_coupon_hpp

#include <ql/cashflows/cmscoupon.hpp>

namespace QuantLib {

    class BondIndex;

    //! CMS coupon class
    /*! \warning This class does not perform any date adjustment,
                 i.e., the start and end date passed upon construction
                 should be already rolled to a business day.
    */
    class CmtCoupon : public CmsCoupon {
      public:
        CmtCoupon(const Date& paymentDate,
                  Real nominal,
                  const Date& startDate,
                  const Date& endDate,
                  Natural fixingDays,
                  const ext::shared_ptr<BondIndex>& index,
                  Real gearing = 1.0,
                  Spread spread = 0.0,
                  const Date& refPeriodStart = Date(),
                  const Date& refPeriodEnd = Date(),
                  const DayCounter& dayCounter = DayCounter(),
                  bool isInArrears = false,
                  const Date& exCouponDate = Date());

        CmtCoupon(const Date& paymentDate,
                  Real nominal,
                  const Date& startDate,
                  const Date& endDate,
                  const Date& fixingDate,
                  const ext::shared_ptr<BondIndex>& index,
                  Real gearing = 1.0,
                  Spread spread = 0.0,
                  const Date& refPeriodStart = Date(),
                  const Date& refPeriodEnd = Date(),
                  const DayCounter& dayCounter = DayCounter(),
                  bool isInArrears = false,
                  const Date& exCouponDate = Date());

        //! \name Inspectors
        //@{
        const ext::shared_ptr<BondIndex>& bondIndex() const { return bondIndex_; }
        Date fixingDate() const override { return this->fixingDate_; }

        //@}
        //! \name Visitability
        //@{
        void accept(AcyclicVisitor&) override;
        //@}
      private:
        ext::shared_ptr<BondIndex> bondIndex_;
        Date fixingDate_;
    };


    //! helper class building a sequence of capped/floored cms-rate coupons
    class CmtLeg {
      public:
        CmtLeg(Schedule schedule, ext::shared_ptr<BondIndex> bondIndex);
        CmtLeg& withNotionals(Real notional);
        CmtLeg& withNotionals(const std::vector<Real>& notionals);
        CmtLeg& withPaymentDayCounter(const DayCounter&);
        CmtLeg& withPaymentAdjustment(BusinessDayConvention);
        CmtLeg& withFixingDays(Natural fixingDays);
        CmtLeg& withFixingDays(const std::vector<Natural>& fixingDays);
        CmtLeg& withGearings(Real gearing);
        CmtLeg& withGearings(const std::vector<Real>& gearings);
        CmtLeg& withSpreads(Spread spread);
        CmtLeg& withSpreads(const std::vector<Spread>& spreads);
        CmtLeg& withCaps(Rate cap);
        CmtLeg& withCaps(const std::vector<Rate>& caps);
        CmtLeg& withFloors(Rate floor);
        CmtLeg& withFloors(const std::vector<Rate>& floors);
        CmtLeg& inArrears(bool flag = true);
        CmtLeg& withZeroPayments(bool flag = true);
        CmtLeg& withExCouponPeriod(const Period&,
                                   const Calendar&,
                                   BusinessDayConvention,
                                   bool endOfMonth);
        operator Leg() const;
      private:
        Schedule schedule_;
        ext::shared_ptr<BondIndex> bondIndex_;
        std::vector<Real> notionals_;
        DayCounter paymentDayCounter_;
        BusinessDayConvention paymentAdjustment_ = Following;
        std::vector<Natural> fixingDays_;
        std::vector<Real> gearings_;
        std::vector<Spread> spreads_;
        std::vector<Rate> caps_, floors_;
        bool inArrears_ = false, zeroPayments_ = false;
        Period exCouponPeriod_;
        Calendar exCouponCalendar_;
        BusinessDayConvention exCouponAdjustment_ = Following;
        bool exCouponEndOfMonth_ = false;
    };

}

#endif
