/***************************************************************************
 *   Copyright (C) 2022 by santiago González                               *
 *   santigoro@gmail.com                                                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>.  *
 *                                                                         *
 ***************************************************************************/

#ifndef UTILS_H
#define UTILS_H

#include <math.h>

class QDomDocument;
class QByteArray;
class QStringList;
class QString;
class QPointF;
class QPoint;
class Pin;

#define unitToVal( val, mult ) \
    if     ( mult == " n" ) val *= 1e3; \
    else if( mult == " u")  val *= 1e6; \
    else if( mult == " m" ) val *= 1e9; \
    else if( mult == " ")   val *= 1e12;

#define valToUnit( val, mult, decimals ) \
    mult = " p";\
    if( fabs( val ) > 999 ) { \
        val /= 1e3; mult = " n"; \
        if( fabs( val ) > 999 ) { \
            val /= 1e3; mult = " u"; \
            if( fabs( val ) > 999 ) { \
                val /= 1e3; mult = " m"; \
                if( fabs( val ) > 999 ) { \
                    val /= 1e3; mult = " "; \
                    if( fabs( val ) > 999 ) { \
                        val /= 1e3; mult = " k"; \
                        if( fabs( val ) > 999 ) { \
                            val /= 1e3; mult = " M"; \
                            if( fabs( val ) > 999 ) { \
                                val /= 1e3; mult = " G"; \
    }}}}}}} \
    if     ( fabs( val ) < 10)   decimals = 3; \
    else if( fabs( val ) < 100)  decimals = 2; \
    else if( fabs( val ) < 1000) decimals = 1;

double getMultiplier( QString mult );
QString multToValStr( double value, QString mult );

QString val2hex( int d );
QString decToBase( int value, int base, int digits );

//---------------------------------------------------

void MessageBoxNB( const QString &title, const QString &message );


//---------------------------------------------------

int roundDown( int x, int roundness );

#endif

