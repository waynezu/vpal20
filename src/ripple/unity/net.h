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

#ifndef RIPPLE_NET_H_INCLUDED
#define RIPPLE_NET_H_INCLUDED

#undef DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_7_AND_LATER /**/
#include <boost/asio/ssl.hpp>

#include <ripple/unity/resource.h>

#include <ripple/unity/data.h>
#include <ripple/websocket/autosocket/AutoSocket.h>

#include <ripple/net/HTTPRequest.h>
#include <ripple/net/HTTPClient.h>
#include <ripple/net/RPCServer.h>
#include <ripple/net/RPCDoor.h>
#include <ripple/net/SNTPClient.h>

#include <ripple/net/RPCErr.h>
#include <ripple/net/RPCUtil.h>
#include <ripple/net/RPCCall.h>
#include <ripple/net/InfoSub.h>
#include <ripple/net/RPCSub.h>

#endif