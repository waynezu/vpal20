//------------------------------------------------------------------------------
/*
    Portions of this file are from Vpallab: https://github.com/vpallabs
    Copyright (c) 2013 - 2014 - Vpallab.com.
    Please visit http://www.vpallab.com/
    
    This file is part of Beast: https://github.com/vinniefalco/Beast
    Copyright 2013, Vinnie Falco <vinnie.falco@gmail.com>

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

#ifndef BEAST_HTTP_PARSER_H_INCLUDED
#define BEAST_HTTP_PARSER_H_INCLUDED

#include <beast/http/message.h>
#include <string>
#include <utility>

namespace beast {
namespace http {

/** Parser for HTTP messages.
    The result is stored in a message object.
*/
class parser : public beast::http::basic_parser
{
private:
    std::reference_wrapper <message> message_;

public:
    /** Construct a parser for HTTP request or response.
        The result is stored in the passed message.
    */
    parser (message& m, bool request)
        : beast::http::basic_parser (request)
        , message_(m)
    {
        message_.get().request(request);
    }

#if defined(_MSC_VER) && _MSC_VER <= 1800
    parser& operator= (parser&& other);

#else
    parser& operator= (parser&& other) = default;

#endif

private:
    template <class = void>
    void
    do_start ();

    template <class = void>
    bool
    do_request (method_t method, std::string const& url,
        int major, int minor, bool keep_alive, bool upgrade);

    template <class = void>
    bool
    do_response (int status, std::string const& text,
        int major, int minor, bool keep_alive, bool upgrade);

    template <class = void>
    void
    do_field (std::string const& field, std::string const& value);

    template <class = void>
    void
    do_body (void const* data, std::size_t bytes);

    template <class = void>
    void
    do_complete();

    void
    on_start () override
    {
        do_start();
    }

    bool
    on_request (method_t method, std::string const& url,
        int major, int minor, bool keep_alive, bool upgrade) override
    {
        return do_request (method, url, major, minor, keep_alive, upgrade);
    }

    bool
    on_response (int status, std::string const& text,
        int major, int minor, bool keep_alive, bool upgrade) override
    {
        return do_response (status, text, major, minor, keep_alive, upgrade);
    }

    void
    on_field (std::string const& field, std::string const& value) override
    {
        do_field (field, value);
    }

    void
    on_body (void const* data, std::size_t bytes) override
    {
        do_body (data, bytes);
    }

    void
    on_complete() override
    {
        do_complete();
    }
};

//------------------------------------------------------------------------------

#if defined(_MSC_VER) && _MSC_VER <= 1800
inline
parser&
parser::operator= (parser&& other)
{
    basic_parser::operator= (std::move(other));
    message_ = std::move (other.message_);
    return *this;
}
#endif

template <class>
void
parser::do_start ()
{
}

template <class>
bool
parser::do_request (method_t method, std::string const& url,
    int major, int minor, bool keep_alive, bool upgrade)
{
    message_.get().method (method);
    message_.get().url (url);
    message_.get().version (major, minor);
    message_.get().keep_alive (keep_alive);
    message_.get().upgrade (upgrade);
    return true;
}

template <class>
bool
parser::do_response (int status, std::string const& text,
    int major, int minor, bool keep_alive, bool upgrade)
{
    message_.get().status (status);
    message_.get().reason (text);
    message_.get().version (major, minor);
    message_.get().keep_alive (keep_alive);
    message_.get().upgrade (upgrade);
    return true;
}

template <class>
void
parser::do_field (std::string const& field, std::string const& value)
{
    message_.get().headers.append (field, value);
}

template <class>
void
parser::do_body (void const* data, std::size_t bytes)
{
    message_.get().body.write (data, bytes);
}

template <class>
void
parser::do_complete()
{
}

} // http
} // beast

#endif