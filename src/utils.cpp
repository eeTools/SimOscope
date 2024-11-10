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

#include <QString>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QMessageBox>
#include <QTextStream>
#include <qpoint.h>
#include <QPointF>
#include <cmath>

//#include "mainwindow.h"
#include "utils.h"

QHash<QString, double> multipliers = {{"p",1e-12},{"n",1e-9},{"µ",1e-6},{"m",1e-3},{"k",1e3},{"M",1e6},{"G",1e9}};

double getMultiplier( QString mult )
{
    mult = mult.remove(" ");
    mult = mult.left(1);
    double multiplier = multipliers.value( mult );
    if( multiplier == 0 ) multiplier = 1;
    return multiplier;
}

QString multToValStr( double value, QString mult )
{
    return QString::number( value*getMultiplier( mult ) );
}

QString val2hex( int d )
{
    QString Hex="0123456789ABCDEF";
    QString h = Hex.mid(d&15,1);
    while(d>15)
    {
        d >>= 4;
        h = Hex.mid( d&15,1 ) + h;
    }
    return h;
}

QString decToBase( int value, int base, int digits )
{
    QString converted = "";
    for( int i=0; i<digits; i++ )
    {
        if( value >= base ) converted = val2hex( value%base ) + converted;
        else                converted = val2hex( value ) + converted;

        if( i+1 == 4 ) converted = " " + converted;
        //if( (i+1)%8 == 0 ) converted = " " + converted;

        value = floor( value/base );
    }
    return converted;
}

//---------------------------------------------------

void MessageBoxNB( const QString &title, const QString &message )
{
    QMessageBox* msgBox = new QMessageBox( NULL );
    msgBox->setAttribute( Qt::WA_DeleteOnClose ); //makes sure the msgbox is deleted automatically when closed
    msgBox->setStandardButtons( QMessageBox::Ok );
    msgBox->setWindowTitle( title );
    msgBox->setText( message );
    msgBox->setModal( false ); 
    msgBox->open();
}


//--------------------------------------------------------------

int roundDown( int x, int roundness )
{
    if( x < 0 ) return (x-roundness+1) / roundness;
    else        return (x / roundness);
}


