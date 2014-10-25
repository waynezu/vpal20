//------------------------------------------------------------------------------
/*
    Portions of this file are from Vpallab: https://github.com/vpallabs
    Copyright (c) 2013 - 2014 - Vpallab.com.
    Please visit http://www.vpallab.com/
    
    This file is part of rippled: https://github.com/ripple/rippled
    Copyright (c) 2012, 2013 Ripple Labs Inc.

    Permission to use, copy, modify, and/or distribute this software for any
    purpose  with  or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

#ifndef RIPPLE_ORDERBOOK_H
#define RIPPLE_ORDERBOOK_H

namespace ripple {

/** Describes a serialized ledger entry for an order book. */
class OrderBook : beast::LeakChecked <OrderBook>
{
public:
    typedef std::shared_ptr <OrderBook> pointer;
    typedef std::shared_ptr <OrderBook> const& ref;
    typedef std::vector<pointer> List;

    /** Construct from a currency specification.

        @param index ???
        @param book in and out currency/issuer pairs.
    */
    // VFALCO NOTE what is the meaning of the index parameter?
    OrderBook (uint256 const& base, Book const& book)
            : mBookBase(base), mBook(book)
    {
    }

    uint256 const& getBookBase () const
    {
        return mBookBase;
    }

    Book const& book() const
    {
        return mBook;
    }

    Currency const& getCurrencyIn () const
    {
        return mBook.in.currency;
    }

    Currency const& getCurrencyOut () const
    {
        return mBook.out.currency;
    }

    Account const& getIssuerIn () const
    {
        return mBook.in.account;
    }

    Account const& getIssuerOut () const
    {
        return mBook.out.account;
    }

private:
    uint256 const mBookBase;
    Book const mBook;
};

} // ripple

#endif
