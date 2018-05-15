/*
 *   Copyright (C) 2015,2016,2017 by Jonathan Naylor G4KLX
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#if !defined(DEBUG_H)
#define  DEBUG_H

#include "Config.h"
#include "Globals.h"
#include "Log.h"

#define  DEBUG1(a)          LogDebug((a))
#define  DEBUG2(a,b)        LogDebug((a),(b))
#define  DEBUG3(a,b,c)      LogDebug((a),(b),(c))
#define  DEBUG4(a,b,c,d)    LogDebug((a),(b),(c),(d))
#define  DEBUG5(a,b,c,d,e)  LogDebug((a),(b),(c),(d),(e))

#endif

