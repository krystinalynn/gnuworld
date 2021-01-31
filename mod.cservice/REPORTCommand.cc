/**
 * REPORTCommand.cc
 *
 * 30/01/2021 - Krystinalynn Maria <QueenElsa@undernet.org>
 * Initial Version. Written, and finished.
 *
 * Allows an oper to make a report about CService abuse.
 *
 * Caveats: None
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
 * $Id: REPORTCommand.cc,v 1.0 2021/01/30 18:19:04 krystinalynn Exp $
 */

#include	<string>
#include	<sstream>
#include	<iostream>

#include	"StringTokenizer.h"
#include	"ELog.h"
#include	"cservice.h"
#include	"levels.h"
#include	"responses.h"
#include	"Network.h"
#include	"cservice_config.h"

namespace gnuworld
{
using std::endl ;
using std::ends ;
using std::string ;
using std::stringstream ;

bool REPORT::Exec( iClient* theClient, const string& Message )
{
bot->incStat("COMMANDS.REPORT");

#ifdef USE_OPERHELP
/*
 * StringTokenizer st( Message ) ;
 * if( st.size() < 2 )
 * 	{
 * 	Usage(theClient);
 * 	return true;
 * 	}
 */

/*
 *  Check the user is an oper.
 */
sqlUser* theUser = bot->isAuthed(theClient, false);

if(!theClient->isOper())
	{
	bot->Notice(theClient, bot->getResponse(theUser,
		language::ircops_only_cmd,
		"Only IRC Operators may use this command."));
	return true;
	}


/* Check the bot isn't in the channel. */
if (!theChan->getInChan())
	{
	bot->Notice(theClient,
		bot->getResponse(theUser, language::already_on_chan,
		"Oh no! It appears that I'm not in #cservice_info, so I can't help you! You'll need to contact a CService Administrator to get this fixed."));
	return false;
	}

/* Invite the oper to the channel. */
bot->Invite(theClient, "#cservice_info");

#endif // USE_OPERHELP

return true;

}

} // namespace gnuworld.
