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

#ifndef RIPPLE_RPC_MANAGER_H_INCLUDED
#define RIPPLE_RPC_MANAGER_H_INCLUDED

#include <ripple/rpc/Request.h>

namespace ripple {
namespace RPC {

/** Processes RPC commands. */
class Manager
{
public:
    typedef std::function <void (Request&)> handler_type;

    virtual ~Manager () = 0;

    /** Add a handler for the specified JSON-RPC command. */
    template <class Handler>
    void add (std::string const& method)
    {
        add (method, handler_type (
        [](Request& req)
        {
            Handler h;
            h (req);
        }));
    }

    virtual void add (std::string const& method, handler_type&& handler) = 0;

    /** Dispatch the JSON-RPC request.
        @return `true` if the command was found.
    */
    virtual bool dispatch (Request& req) = 0;
};

std::unique_ptr <Manager> make_Manager (beast::Journal journal);

} // RPC
} // ripple

#endif
