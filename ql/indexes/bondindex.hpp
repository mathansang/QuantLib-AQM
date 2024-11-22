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

/*! \file swapindex.hpp
    \brief swap-rate indexes
*/

#ifndef quantlib_bondindex_hpp
#define quantlib_bondindex_hpp

#include <ql/indexes/swapindex.hpp>
#include <ql/instruments/bond.hpp>

namespace QuantLib {

    class Schedule;

    class IborIndex;


    //! base class for swap-rate indexes
    class BondIndex : public InterestRateIndex {
      public:
        BondIndex(const std::string& familyName,
                  const Period& tenor,
                  Natural settlementDays,
                  const Currency& currency,
                  const Calendar& fixingCalendar,
                  const Period& fixedLegTenor,
                  BusinessDayConvention fixedLegConvention,
                  const DayCounter& fixedLegDayCounter,
                  Handle<YieldTermStructure> discountingTermStructure);
        //! \name InterestRateIndex interface
        //@{
        Date maturityDate(const Date& valueDate) const override;
        //@}
        //! \name Inspectors
        //@{
        Period fixedLegTenor() const { return fixedLegTenor_; }
        BusinessDayConvention fixedLegConvention() const;
        Handle<YieldTermStructure> discountingTermStructure() const;
        /*! \warning Relinking the term structure underlying the index will
                     not have effect on the returned swap.
        */
        // ext::shared_ptr<Bond> underlyingBond(const Date& fixingDate) const;
        //@}
        //! \name Other methods
        //@{
        virtual ext::shared_ptr<BondIndex> clone(
                        const Handle<YieldTermStructure>& discounting) const;
        //! returns a copy of itself with different tenor
        virtual ext::shared_ptr<BondIndex> clone(
                        const Period& tenor) const;
        // @}

        ext::shared_ptr<SwapIndex> dummySwapIndex() const { return this->dummySwapIndex_; }

        
      protected:
        Rate forecastFixing(const Date& fixingDate) const override;
        Period tenor_;
        // ext::shared_ptr<IborIndex> iborIndex_;
        Period fixedLegTenor_;
        BusinessDayConvention fixedLegConvention_;
        Handle<YieldTermStructure> discount_;
        // cache data to avoid swap recreation when the same fixing date
        // is used multiple time to forecast changing fixing
        mutable ext::shared_ptr<Bond> lastBond_;
        mutable Date lastFixingDate_;
        ext::shared_ptr<SwapIndex> dummySwapIndex_;
    };

    // inline definitions
    inline BusinessDayConvention BondIndex::fixedLegConvention() const {
        return fixedLegConvention_;
    }


}

#endif
