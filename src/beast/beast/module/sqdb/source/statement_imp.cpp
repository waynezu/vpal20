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

    Portions based on SOCI - The C++ Database Access Library: 

    SOCI: http://soci.sourceforge.net/

    This file incorporates work covered by the following copyright
    and permission notice:

    Copyright (C) 2004 Maciej Sobczak, Stephen Hutton, Mateusz Loskot,
    Pawel Aleksander Fedorynski, David Courtney, Rafal Bobrowski,
    Julian Taylor, Henning Basold, Ilia Barahovski, Denis Arnaud,
    Daniel Lidstr�m, Matthieu Kermagoret, Artyom Beilis, Cory Bennett,
    Chris Weed, Michael Davidsaver, Jakub Stachowski, Alex Ott, Rainer Bauer,
    Martin Muenstermann, Philip Pemberton, Eli Green, Frederic Chateau,
    Artyom Tonkikh, Roger Orr, Robert Massaioli, Sergey Nikulov,
    Shridhar Daithankar, S�ren Meyer-Eppler, Mario Valesco.

    Boost Software License - Version 1.0, August 17th, 2003

    Permission is hereby granted, free of charge, to any person or organization
    obtaining a copy of the software and accompanying documentation covered by
    this license (the "Software") to use, reproduce, display, distribute,
    execute, and transmit the Software, and to prepare derivative works of the
    Software, and to permit third-parties to whom the Software is furnished to
    do so, all subject to the following:

    The copyright notices in the Software and this entire statement, including
    the above license grant, this restriction and the following disclaimer,
    must be included in all copies of the Software, in whole or in part, and
    all derivative works of the Software, unless such copies or derivative
    works are solely in the form of machine-executable object code generated by
    a source language processor.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
    SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
    FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/
//==============================================================================

namespace beast {
namespace sqdb {
namespace detail {

/*

Running a new prepared statement:

sqlite3_prepare()   // create prepared statement
sqlite3_bind()      // define input variables
sqlite3_step()      // retrieve next row
sqlite3_column()    // extract output value
sqlite3_finalize()  // destroy prepared statement

Re-using an existing prepared statement

sqlite3_reset()     // reset the statement (input bindings are not cleared)
sqlite3_clear_bindings()  // set all input variables to NULL
  AND/OR
sqlite3_bind()      // define input variables
sqlite3_step()      // retrieve next row
sqlite3_column()    // extract output value

*/

statement_imp::statement_imp(session& s)
    : m_session(s)
    , m_stmt(0)
    , m_bReady(false)
    , m_bGotData(false)
    , m_last_insert_rowid(0)
{
}

statement_imp::statement_imp(prepare_temp_type const& prep)
    : m_session(prep.get_prepare_info().m_session)
    , m_stmt(0)
    , m_bReady(false)
    , m_bGotData(false)
{
    ref_counted_prepare_info& rcpi = prep.get_prepare_info();

    m_intos.swap(rcpi.m_intos);
    m_uses.swap(rcpi.m_uses);

    prepare(rcpi.get_query());
}

statement_imp::~statement_imp()
{
    clean_up();
}

void statement_imp::erase_bindings()
{
    // delete intos
    for (std::size_t i = m_intos.size(); i != 0; --i)
    {
        into_type_base* p = m_intos[i - 1];
        delete p;
        m_intos.resize(i - 1);
    }

    // delete uses
    for (std::size_t i = m_uses.size(); i != 0; --i)
    {
        use_type_base* p = m_uses[i - 1];
        p->clean_up();
        delete p;
        m_uses.resize(i - 1);
    }
}

void statement_imp::exchange(detail::into_type_ptr const& i)
{
    m_intos.push_back(i.get());
    i.release();
}

void statement_imp::exchange(detail::use_type_ptr const& u)
{
    m_uses.push_back(u.get());
    u.release();
}

void statement_imp::clean_up()
{
    erase_bindings();
    release_resources();
}

void statement_imp::prepare(std::string const& query, bool bRepeatable)
{
    m_query = query;
    m_session.log_query(query);
    m_last_insert_rowid = 0;

    release_resources();

    char const* tail = 0;
    int result = sqlite3_prepare_v2(
                     m_session.get_connection(),
                     query.c_str(),
                     static_cast<int>(query.size()),
                     &m_stmt,
                     &tail);

    if (result == SQLITE_OK)
    {
        m_bReady = true;
    }
    else
    {
        throw detail::sqliteError(__FILE__, __LINE__, result);
    }
}

Error statement_imp::execute()
{
    Error error;

    assert (m_stmt != nullptr);

    // ???
    m_bGotData = false;
    m_session.set_got_data(m_bGotData);

    // binds

    int iCol = 0;

    for (intos_t::iterator iter = m_intos.begin(); iter != m_intos.end(); ++iter)
        (*iter)->bind(*this, iCol);

    int iParam = 1;

    for (uses_t::iterator iter = m_uses.begin(); iter != m_uses.end(); ++iter)
        (*iter)->bind(*this, iParam);

    // reset
    error = detail::sqliteError(__FILE__, __LINE__, sqlite3_reset(m_stmt));

    if (!error)
    {
        // set input variables
        do_uses();

        m_bReady = true;
        m_bFirstTime = true;
    }

    return error;
}

bool statement_imp::fetch(Error& error)
{
    int result = sqlite3_step(m_stmt);

    if (result == SQLITE_ROW ||
            result == SQLITE_DONE)
    {
        if (m_bFirstTime)
        {
            m_last_insert_rowid = m_session.last_insert_rowid();
            m_bFirstTime = false;
        }

        if (result == SQLITE_ROW)
        {
            m_bGotData = true;
            m_session.set_got_data(m_bGotData);

            do_intos();
        }
        else
        {
            m_bGotData = false;
            m_session.set_got_data(m_bGotData);

            if (result == SQLITE_DONE)
            {
                m_bReady = false;
            }
        }
    }
    else if (result != SQLITE_OK)
    {
        m_bGotData = false;
        error = detail::sqliteError(__FILE__, __LINE__, result);
    }
    else
    {
        // should never get SQLITE_OK here
        throw std::invalid_argument ("invalid result");
    }

    return m_bGotData;
}

bool statement_imp::got_data() const
{
    return m_bGotData;
}

void statement_imp::do_intos()
{
    for (intos_t::iterator iter = m_intos.begin(); iter != m_intos.end(); ++iter)
        (*iter)->do_into();
}

void statement_imp::do_uses()
{
    Error error = detail::sqliteError(__FILE__, __LINE__,
                                      sqlite3_clear_bindings(m_stmt));

    if (error)
        throw error;

    for (uses_t::iterator iter = m_uses.begin(); iter != m_uses.end(); ++iter)
        (*iter)->do_use();
}

void statement_imp::post_use()
{
    // reverse order
    for (uses_t::reverse_iterator iter = m_uses.rbegin(); iter != m_uses.rend(); ++iter)
        (*iter)->post_use();
}

void statement_imp::release_resources()
{
    if (m_stmt)
    {
        sqlite3_finalize(m_stmt);
        m_stmt = 0;
    }

    m_bReady = false;
    m_bGotData = false;
}

rowid statement_imp::last_insert_rowid()
{
    return m_last_insert_rowid;
}

} // detail
} // sqdb
} // beast
