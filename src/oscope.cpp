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

#include <QComboBox>
#include <QDebug>

#include "oscope.h"
#include "oscopechannel.h"
#include "oscwidget.h"

Oscope::Oscope()
{
    m_audioIO = NULL;
    m_audioIn = NULL;

    m_numChannels = 4;
    m_trigger = 4;
    m_auto    = 4;
    m_timerId = 0;
    m_paused = false;

    m_bufferSize = 600000;

    m_color[0] = QColor( 240, 240, 100 );
    m_color[1] = QColor( 220, 220, 255 );
    m_color[2] = QColor( 255, 210, 90  );
    m_color[3] = QColor( 000, 245, 160 );
    m_color[4] = QColor( 255, 255, 255 );

    m_oscWidget  = new OscWidget( this );
    m_display = m_oscWidget->display();
    m_display->setPlotBase( this );
    m_oscWidget->getLayout()->addWidget( m_display );

    for( int i=0; i<4; i++ )
    {
        m_channel[i] = new OscopeChannel( this, "Chan"+QString::number(i) );
        m_channel[i]->m_channel = i;
        m_channel[i]->m_buffer.resize( m_bufferSize );
        m_channel[i]->m_time.resize( m_bufferSize );

        m_hideCh[i] = false;

        m_display->setChannel( i, m_channel[i] );
        m_display->setColor( i, m_color[i] );

        setTimePos( i, 0 );
        setVoltDiv( i, 1 );
        setVoltPos( i, 0 );
    }
    setFilter( 0.1 );
    setTimeDiv( 2*1e11 ); // 200 ms
}
Oscope::~Oscope(){}

void Oscope::initialize()
{
    for( int i=0; i<4; ++i ) m_channel[i]->initialize();
}
void Oscope::updateStep()
{
    uint64_t period = 0;
    uint64_t timeFrame = m_timeDiv*10;

    if( !m_paused )
    {
        if( m_trigger < 4 ) period = m_channel[m_trigger]->m_period; // We want a trigger

        if( period > 10 ) // We have a Trigger
        {
            uint64_t risEdge = m_channel[m_trigger]->m_risEdge;

            uint64_t nCycles = timeFrame/period;
            if( timeFrame%period ) nCycles++;
            if( nCycles%2 )        nCycles++;

            uint64_t delta = nCycles*period/2-timeFrame/2;
            if( delta > risEdge ) delta = risEdge;
            m_audioTime = risEdge-delta;
        }
        else m_audioTime = m_audioIO->time(); // free running

        m_display->setTimeEnd( m_audioTime );

        for( int i=0; i<4; i++ )
        {
            if( !m_connect[i] ) continue;
            m_channel[i]->m_trigIndex = m_channel[i]->m_bufferCounter;
            m_channel[i]->updateStep( m_audioTime );
        }
    }
    m_display->update(); //redrawScreen();
}

void Oscope::setValue( int ch, uint64_t time, double volt )
{
    //if( ch == 0 ) qDebug() << time/1e6 << volt;
    if( !m_paused ) m_channel[ch]->voltChanged( time, volt );
}


void Oscope::setFilter( double filter )
{
    m_filter = filter;
    m_oscWidget->setFilter( filter );
    for( int i=0; i<2; i++ ) m_channel[i]->setFilter( filter );
}

void Oscope::setTrigger( int ch )
{
    m_trigger = ch;
    m_oscWidget->setTrigger( ch );

    if( ch > 3 ) return;
    for( int i=0; i<4; ++i ){
        if( ch == i ) m_channel[i]->m_trigger = true;
        else          m_channel[i]->m_trigger = false;
}   }

void Oscope::setAutoSC( int ch )
{
    m_auto = ch;
    m_oscWidget->setAuto( ch );
}

QString Oscope::hideCh()
{
    QString list;
    QString hide;
    for( int i=0; i<4; ++i ){
        hide = m_hideCh[i]? "true":"false";
        list.append( hide ).append(",");
    }
    return list;
}

void Oscope::setHideCh( QString hc )
{
    QStringList list = hc.split(",");
    for( int i=0; i<4; ++i ){
        if( i == list.size() ) break;
        bool hide = (list.at(i) == "true")? true:false;
        hideChannel( i, hide );
}   }

void Oscope::hideChannel( int ch, bool hide )
{
    if( ch > 3 ) return;
    m_hideCh[ch] = hide;
    m_display->hideChannel( ch , hide );
    m_oscWidget->hideChannel( ch, hide );
}

int Oscope::tracks() { return m_display->tracks(); }
void Oscope::setTracks( int tracks )
{
    m_display->setTracks( tracks );
    m_oscWidget->setTracks( tracks );
}

void Oscope::setTimeDiv( uint64_t td )
{
    if( td < 1 ) td = 1;
    m_timeDiv = td;
    m_display->setTimeDiv( td );
    m_oscWidget->updateTimeDivBox( td );
}

QString Oscope::timPos()
{
    QString list;
    for( int i=0; i<4; ++i ) list.append( QString::number( m_timePos[i] )).append(",");
    return list;
}

void Oscope::setTimPos( QString tp )
{
    QStringList list = tp.split(",");
    for( int i=0; i<4; ++i ){
        if( i == list.size() ) break;
        setTimePos( i, list.at(i).toLongLong() );
}   }

QString Oscope::volDiv()
{
    QString list;
    for( int i=0; i<4; ++i ) list.append( QString::number( m_voltDiv[i] )).append(",");
    return list;
}

void Oscope::setVolDiv( QString vd )
{
    QStringList list = vd.split(",");
    for( int i=0; i<4; ++i ){
        if( i == list.size() ) break;
        setVoltDiv( i, list.at(i).toDouble() );
}   }

QString Oscope::volPos()
{
    QString list;
    for( int i=0; i<4; ++i ) list.append( QString::number( m_voltPos[i] )).append(",");
    return list;
}

void Oscope::setVolPos( QString vp )
{
    QStringList list = vp.split(",");
    for( int i=0; i<4; ++i ){
        if( i == list.size() ) break;
        setVoltPos( i, list.at(i).toDouble() );
}   }

void Oscope::setTimePos( int ch, int64_t tp )
{
    m_timePos[ch] = tp;
    m_display->setHPos( ch, tp );
    m_oscWidget->updateTimePosBox( ch, tp );
}

void Oscope::moveTimePos( int64_t delta )
{
    if( m_trigger < 4  )
    {
        uint64_t period = m_channel[m_trigger]->m_period; // check wave period
        if( period > 10 ) return;                // We have a Trigger: Don't Zoom at mouse pos.
    }
    for( int i=0; i<4; ++i )
    {
        m_timePos[i] = m_timePos[i]+delta;
        m_display->setHPos( i, m_timePos[i] );
        m_oscWidget->updateTimePosBox( i, m_timePos[i] );
    }
}

void Oscope::setVoltDiv( int ch, double vd )
{
    m_voltDiv[ch] = vd;
    m_display->setVTick( ch, vd );
    m_oscWidget->updateVoltDivBox( ch, vd );
}

void Oscope::setVoltPos( int ch, double vp )
{
    m_voltPos[ch] = vp;
    m_display->setVPos( ch, vp );
    m_oscWidget->updateVoltPosBox( ch, vp );
}
