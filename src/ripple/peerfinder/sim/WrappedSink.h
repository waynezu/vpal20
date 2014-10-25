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

#ifndef RIPPLE_PEERFINDER_SIM_WRAPPEDSINK_H_INCLUDED
#define RIPPLE_PEERFINDER_SIM_WRAPPEDSINK_H_INCLUDED

namespace ripple {
namespace PeerFinder {

/** Wraps a Journal::Sink to prefix it's output. */
class WrappedSink : public beast::Journal::Sink
{
public:
    WrappedSink (std::string const& prefix, beast::Journal::Sink& sink)
        : m_prefix (prefix)
        , m_sink (sink)
    {
    }

    bool active (beast::Journal::Severity level) const
        { return m_sink.active (level); }

    bool console () const
        { return m_sink.console (); }

    void console (bool output)
        { m_sink.console (output); }

    beast::Journal::Severity severity() const
        { return m_sink.severity(); }

    void severity (beast::Journal::Severity level)
        { m_sink.severity (level); }

    void write (beast::Journal::Severity level, std::string const& text)
    {
        using beast::Journal;
        std::string s (m_prefix);
        switch (level)
        {
        case Journal::kTrace:   s += "Trace: "; break;
        case Journal::kDebug:   s += "Debug: "; break;
        case Journal::kInfo:    s += "Info : "; break;
        case Journal::kWarning: s += "Warn : "; break;
        case Journal::kError:   s += "Error: "; break;
        default:
        case Journal::kFatal:   s += "Fatal: "; break;
        };
            
        s+= text;
        m_sink.write (level, s);
    }

private:
    std::string const m_prefix;
    beast::Journal::Sink& m_sink;
};

}
}

#endif
