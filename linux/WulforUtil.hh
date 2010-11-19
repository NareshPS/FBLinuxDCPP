/*
 * Copyright © 2004-2008 Jens Oknelid, paskharen@gmail.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * In addition, as a special exception, compiling, linking, and/or
 * using OpenSSL with this program is allowed.
 */

#ifndef WULFOR_UTIL_H
#define WULFOR_UTIL_H

#include <gtk/gtk.h>
#include <client/stdinc.h>
#include <client/DCPlusPlus.h>
#include <client/CID.h>
#include <client/User.h>

class WulforUtil
{
	public:
		static std::vector<int> splitString(const std::string &str, const std::string &delimiter);
		static std::string linuxSeparator(const std::string &ps);
		static std::string windowsSeparator(const std::string &ps);
		static std::vector<std::string> getLocalIPs();
		static std::string getNicks(const std::string &cid);
		static std::string getNicks(const CID& cid);
		static std::string getNicks(const User::Ptr& user);
		static std::string getHubNames(const std::string &cid);
		static std::string getHubNames(const CID& cid);
		static std::string getHubNames(const User::Ptr& user);
		static StringList getHubAddress(const CID& cid);
		static StringList getHubAddress(const User::Ptr& user);
		static std::string getTextFromMenu(GtkMenuItem *item);
		static std::vector<std::string>& getCharsets();
		static void openURI(const std::string &uri);
 		// Magnet links
		static std::string makeMagnet(const std::string &name, const int64_t size, const std::string &tth);
		static bool splitMagnet(const std::string &magnet, std::string &name, int64_t &size, std::string &tth);
		static bool isMagnet(const std::string &text);
		static bool isLink(const std::string &text);
		static bool isHubURL(const std::string &text);
		// Profile locking
		static bool profileIsLocked();

		static const std::string ENCODING_SYSTEM_DEFAULT;
		static const std::string ENCODING_GLOBAL_HUB_DEFAULT;
		
	private:
		static std::vector<std::string> charsets;
		static const std::string magnetSignature;
};

#endif
