/**
 * NEWPASSCommand.cc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 * $Id: NEWPASSCommand.cc,v 1.17 2005/11/17 22:20:57 kewlio Exp $
 */

#include	<string>
#include	<sstream>
#include	<iostream>
#include	<iomanip>
#include	<inttypes.h>

#include	"md5hash.h"
#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"responses.h"
#include	"networkData.h"
#include	"cservice_config.h"
#include	"levels.h"

const char NEWPASSCommand_cc_rcsId[] = "$Id: NEWPASSCommand.cc,v 1.17 2005/11/17 22:20:57 kewlio Exp $" ;

namespace gnuworld
{
using std::string ;
using std::endl ;
using std::ends ;
using std::stringstream ;

const char validChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.$*_";

bool NEWPASSCommand::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.NEWPASS");

#ifndef USE_NEWPASS
bot->Notice(theClient, "To change your account password, please use the web interface.");
return true;
#else

StringTokenizer st( Message ) ;
if( st.size() < 2 )
	{
	Usage(theClient);
	return true;
	}

/*
 *  Fetch the sqlUser record attached to this client. If there isn't one,
 *  they aren't logged in - tell them they should be.
 */

sqlUser* tmpUser = bot->isAuthed(theClient, true);
if (!tmpUser)
	{
	return false;
	}

int admAccess = (int)bot->getAdminAccessLevel(tmpUser);
string newpass = st.assemble(1);
sqlUser* targetUser = tmpUser;
//bool notMe = false;
if ((admAccess) && (st.size() > 2))
{
	targetUser = bot->getUserRecord(st[1]);
	if (targetUser)
	{
		if (admAccess < level::newpass)
		{
			
