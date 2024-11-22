/*
 Copyright (C) 2006, 2009 Ferdinando Ametrano
 Copyright (C) 2006, 2007, 2009 StatPro Italia srl

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

#include <ql/indexes/iborindex.hpp>
#include <ql/indexes/bondindex.hpp>
#include <ql/instruments/makeois.hpp>
#include <ql/instruments/makevanillaswap.hpp>
#include <ql/time/schedule.hpp>
#include <sstream>
#include <utility>

namespace QuantLib {

    BondIndex::BondIndex(const std::string& familyName,
                         const Period& tenor,
                         Natural settlementDays,
                         const Currency& currency,
                         const Calendar& fixingCalendar,
                         const Period& fixedLegTenor,
                         BusinessDayConvention fixedLegConvention,
                         const DayCounter& fixedLegDayCounter,
                         Handle<YieldTermStructure> discount)
    : InterestRateIndex(
          familyName, tenor, settlementDays, currency, fixingCalendar, fixedLegDayCounter),
      tenor_(tenor), fixedLegTenor_(fixedLegTenor),
      fixedLegConvention_(fixedLegConvention), discount_(std::move(discount)) 
    {
        // iborIndex_(std::move(iborIndex)), 
        // registerWith(iborIndex_);
        registerWith(discount_);

        auto iborIndex = ext::make_shared<IborIndex>(
			"dummyIborIndex",
			fixedLegTenor,
			settlementDays,
			currency,
			fixingCalendar,
			fixedLegConvention,
			false, 
            fixedLegDayCounter,
			discount_);

		this->dummySwapIndex_ = ext::make_shared<SwapIndex>(
			"dummySwapIndex",
			tenor,
			settlementDays,
			currency,
			fixingCalendar,
			fixedLegTenor,
			fixedLegConvention, 
            fixedLegDayCounter,
			iborIndex,
			discount_);
    }

    Handle<YieldTermStructure> BondIndex::discountingTermStructure() const {
        return discount_;  // empty if not exogenous
    }

    Rate BondIndex::forecastFixing(const Date& fixingDate) const 
    {
        Real v = 0.0;
        
        Date effectiveDate = this->valueDate(fixingDate);
        Date maturityDate = this->maturityDate(effectiveDate);

        Real d_t = this->discount_->discount(effectiveDate);
        Real d_T = this->discount_->discount(maturityDate);

        Schedule schedule(effectiveDate, maturityDate, fixedLegTenor_, fixingCalendar(), 
            fixedLegConvention_, fixedLegConvention_, DateGeneration::Forward, false);

        Size size = schedule.size() - 1;

        for (Size i = 0; i < size; i++) {
            Date d = schedule.date(i+1);
            DiscountFactor df = this->discount_->discount(d) / d_t;
            v += (df * this->dayCounter().yearFraction(effectiveDate, maturityDate));
        }

        v = v / (d_T - d_t);

        return v;

    }

    //Rate BondIndex::forecastFixing2(const Date& fixingDate) const {
    //    this->dummySwapIndex_->forecastFixing(fixingDate);
    //}

    Date BondIndex::maturityDate(const Date& valueDate) const {
        return this->fixingCalendar().advance(valueDate, tenor_, this->fixedLegConvention_);
    }

    ext::shared_ptr<BondIndex>
    BondIndex::clone(const Handle<YieldTermStructure>& forwarding) const {

        return ext::make_shared<BondIndex>(familyName(),
                        tenor(),
                        fixingDays(),
                        currency(),
                        fixingCalendar(),
                        fixedLegTenor(),
                        fixedLegConvention(),
                        dayCounter(),
                        this->discount_);
    }

    ext::shared_ptr<BondIndex>
    BondIndex::clone(const Period& tenor) const {
        return ext::make_shared<BondIndex>(familyName(),
                        tenor,
                        fixingDays(),
                        currency(),
                        fixingCalendar(),
                        fixedLegTenor(),
                        fixedLegConvention(),
                        dayCounter(),
                        this->discount_);

    }

}
