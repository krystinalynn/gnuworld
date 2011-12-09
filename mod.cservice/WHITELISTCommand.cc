/**
 * WHITELISTCommand.cc
 *
 * 22/11/2011 - Jochen Meesters <ekips@pandora.be>
 * Initial Version.
 *
 * Adds, removes or views IP entries from 
 * a whitelist, to be used by an external application.
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
 */


#include        <string>
#include        <sstream>

#include        "StringTokenizer.h"
#include        "cservice.h"
#include        "levels.h"
#include        "responses.h"
#include        "dbHandle.h"


namespace gnuworld
{
using std::string ;
using std::endl ;
using std::ends ;
using std::stringstream ;

bool WHITELISTCommand::Exec( iClient* theClient, const string& Message )
{

bot->incStat("COMMANDS.WHITELIST");

StringTokenizer st( Message ) ;
if( st.size() < 3 )
        {
        Usage(theClient);
        return true;
        }


/*
 *  Fetch the sqlUser record attached to this client. If there isn't one,
 *  they aren't logged in - tell them they should be.
 */

sqlUser* theUser = bot->isAuthed(theClient, true);
if (!theUser)
        {
        return false;
        }

/*
 *  Check the user has sufficient access for this command..
 */

int level = bot->getAdminAccessLevel(theUser);
if (level < level::whitelist)
        {
        bot->Notice(theClient,
                bot->getResponse(theUser,
                        language::insuf_access,
                        string("You have insufficient access to perform that command")));
        return false;
        }

if (!ConnectionManager::isIpAddress(st[2]))
	{
	bot->Notice(theClient, "You can only supply a single IPv4 address in dot-decimal notation. (a.b.c.d)");
	return true;
	}

stringstream whitelistQuery;
whitelistQuery 	<< "SELECT ip, addedby, addedon, expiresat, reason FROM whitelist WHERE "
		<< "IP = '" << st[2] << "'" << ends;

#ifdef LOG_SQL
        elog    << "WHITELIST::sqlQuery> "
                << whitelistQuery.str()
                << endl;
#endif

if( !bot->SQLDb->Exec( whitelistQuery, true ) )
        {
        elog    << "WHITELIST> SQL Error: "
                << bot->SQLDb->ErrorMessage()
                << endl ;
        return false ;
        }

bool onList = false;
string dbIP, dbAddedBy, dbReason;
unsigned int dbAddedOn, dbExpires;
if (bot->SQLDb->Tuples() > 0) 
	{
	onList = true;
	dbIP = bot->SQLDb->GetValue(0, 0);
	dbAddedBy = bot->SQLDb->GetValue(0, 1);
	dbAddedOn = atoi(bot->SQLDb->GetValue(0, 2).c_str());
	dbExpires = atoi(bot->SQLDb->GetValue(0, 3).c_str());
	dbReason = bot->SQLDb->GetValue(0, 4);
	}

string option = string_upper(st[1]);

if (option == "ADD")
	{
	if( st.size() < 5 )
	        {
	        Usage(theClient);
		return true;
		}

	if (onList) {
		bot->Notice(theClient, "The IP %s is already added to the whitelist.", st[2].c_str());
		return true;
		}

	unsigned int whiteTime;
	unsigned int maxWhiteTime = 86400;
	if (IsTimeSpec(st[3])) 
		{
		whiteTime = extractTime(st[3], 3600);
		} else {
			if (st[3] == "*")
				{
				whiteTime = 0;
				} else {
					bot->Notice(theClient, "Please enter a valid duration.");
					return true;
				}
			}

	if ((whiteTime == 0 || whiteTime > maxWhiteTime) && (level < level::whitelistadmin))
		{
		bot->Notice(theClient, "Sorry, you can't use that duration.");
		return true;
		}
	
	unsigned int expireTime;	
	if (whiteTime == 0) {
		expireTime = 0;
	} else {
		expireTime = bot->currentTime() + whiteTime;
	}
	
	string whiteReason = st.assemble(4);
//	elog << st[1] << " " << st[2] << " " << whiteTime << " " << whiteReason << endl;

	stringstream whitelistQuery;
	whitelistQuery 	<< "INSERT INTO whitelist (IP, AddedBy, AddedOn, ExpiresAt, Reason) "
                	<< "VALUES ('" << st[2] << "', '" << theUser->getUserName()
			<< "', now()::abstime::int4, " << expireTime << ", '"
			<< whiteReason << "')"
	                << ends;

#ifdef LOG_SQL
        elog    << "WHITELIST::sqlQuery> "
                << whitelistQuery.str()
                << endl;
#endif

if( !bot->SQLDb->Exec( whitelistQuery, true ) )
        {
        elog    << "WHITELIST> SQL Error: "
                << bot->SQLDb->ErrorMessage()
                << endl ;
		return false;
        }
	bot->Notice(theClient, "IP %s added to the whitelist.", st[2].c_str());
	return true;
	}

if (option == "REM" || option == "DEL")
        {
        if (!onList) 
		{
	        bot->Notice(theClient, "Can't find IP %s on the whitelist.", st[2].c_str());
		return true;
		}
	
	if (dbExpires == 0 && level < level::whitelistadmin)
		{
		bot->Notice(theClient, "Sorry, you can't remove permanent whitelist entries.");
		return true;
		}

	stringstream whitelistQuery;
	whitelistQuery  << "DELETE FROM whitelist "
			<< "WHERE ip = '" << dbIP
			<< "'" << ends;
#ifdef LOG_SQL
        elog    << "WHITELIST::sqlQuery> "
                << whitelistQuery.str()
	        << endl;
#endif

if( !bot->SQLDb->Exec( whitelistQuery, true ) )
        {
	elog    << "WHITELIST> SQL Error: "
                << bot->SQLDb->ErrorMessage()
                << endl ;
                return false;
        }
	bot->Notice(theClient, "Removed IP %s from the whitelist.", st[2].c_str());
        return true;
        }

if (option == "VIEW")
        {
	if (!onList)
		{
		bot->Notice(theClient, "Can't find IP %s on the whitelist.", st[2].c_str());
		return true;
		}

	const time_t timeAddedOn = (time_t)dbAddedOn;
	bot->Notice(theClient, "%s: Added on %s (expires in: %s) by %s -- %s", dbIP.c_str(), ctime(&timeAddedOn));
	bot->Notice(theClient, "(expires in: %s) by %s -- %s", 
		bot->prettyDuration(bot->currentTime() - (dbExpires - bot->currentTime())).c_str(), 
		dbAddedBy.c_str(), dbReason.c_str());
        return true;
        }

Usage(theClient);
return true;
}
}
