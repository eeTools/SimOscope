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

#include "oscopechannel.h"
#include "oscope.h"
#include "plotdisplay.h"
#include "utils.h"

OscopeChannel::OscopeChannel( Oscope* oscope, QString id )
{
    m_oscope = oscope;
    m_trigIndex = 0;
    m_filter = 0.0;
}
OscopeChannel::~OscopeChannel()  { }

void OscopeChannel::initialize()
{
    m_rising   = false;
    m_falling  = false;

    m_period = 0;
    m_risEdge = 0;
    m_nCycles = 0;
    m_totalP = 0;
    m_numMax = 0;
    m_lastMax = 0;
    m_ampli = 0;
    m_maxVal = -1e12;
    m_minVal = 1e12;
    m_midVal = 0;
    m_dispMax = 5;
    m_dispMin =-5;

    m_lastValue = 0;
    m_bufferCounter = 0;

    m_freq = 0;

    m_buffer.fill(0);
    m_time.fill(0);

    //updateStep();
}

void OscopeChannel::updateValues()
{
    m_dataTime = 0;

    ///if( !Simulator::self()->isRunning() ) m_freq = 0;
    double val = m_freq*1e12;
    int decs = 0;
    QString unit = " ";
    if( val >= 1 ) {valToUnit( val, unit, decs );}
    QString f = " "+QString::number( val, 'f', decs )+unit+"Hz";

    //m_oscope->dataW()->setData( m_channel, f );
}

void OscopeChannel::updateStep( uint64_t simTime )
{
    if( m_period > 10 )  // We have a wave
    {
        if( m_numMax > 1 )  // Got enought maximums to calculate Freq
        {
            double avgPeriod = (double)m_totalP/(double)(m_numMax-1);
            m_freq = 1e12/avgPeriod;

            m_totalP  = 0;
            m_numMax  = 0;
        }
        ///if( (m_subSample<1) && (m_freq<20) )voltChanged();
    }else{
        ///voltChanged();
        m_freq = 0;
        m_maxVal  =-1e12;
        m_minVal  = 1e12;
    }
    if( m_oscope->autoSC() == m_channel )
    {
        m_oscope->setTimePos( m_channel, 0 );
        if( m_period > 10 )
        {
            m_oscope->setTimeDiv( m_period/5 );
            m_oscope->setVoltDiv( m_channel, m_ampli/10 );
            m_oscope->setVoltPos( m_channel, -m_midVal );
            m_oscope->display()->setLimits( m_channel, m_dispMax, m_dispMin );
    }   }
    else{
        m_dispMax = m_oscope->voltDiv( m_channel )*10;
        m_dispMin = 0;
        m_oscope->display()->setLimits( m_channel, m_dispMax, m_dispMin );
    }
    /// updateValues();

    if( m_period > 10 )  // Do we still have a wave?
    {
        uint64_t stepsPF  = 25000000;
        uint64_t lost = m_period*2;
        if( lost < stepsPF*2 ) lost = stepsPF*2;

        if( simTime-m_lastMax > lost ) // Wave lost
        {
            m_freq    = 0;
            m_period  = 0;
            m_risEdge = 0;
            m_nCycles = 0;
            m_totalP  = 0;
            m_numMax  = 0;
            m_lastMax = 0;
            m_ampli   = 0;
        }
    }
}

void OscopeChannel::voltChanged( uint64_t time, double data )
{
    if( data > m_maxVal ) m_maxVal = data;
    if( data < m_minVal ) m_minVal = data;

    if( ++m_bufferCounter >= m_buffer.size() ) m_bufferCounter = 0;
    m_buffer[m_bufferCounter] = data;
    m_time[m_bufferCounter] = time;

    double delta = data-m_lastValue;

    if( delta > 0 )               // Rising
    {
        if( delta > m_filter )
        {
            if( m_falling && !m_rising )     // Min To Rising
            {
                if( m_numMax > 0 ) m_totalP += time-m_lastMax;
                m_lastMax = time;

                m_numMax++;
                m_nCycles++;
                m_falling = false;
            }
            m_rising = true;
            m_lastValue = data;
        }
        if( m_nCycles > 2 )     // Trigger lost? Reset Max and Min values
        {
            m_nCycles = 0;
            m_maxVal  =-1e12;
            m_minVal  = 1e12;
        }
        else if( m_nCycles > 1 )     // Wait for a full wave
        {
            m_ampli  = m_maxVal-m_minVal;
            m_midVal = m_minVal + m_ampli/2;

            if( data >= m_midVal )            // Rising edge
            {
                if( m_numMax > 1 )
                {
                    m_dispMax = m_maxVal;
                    m_dispMin = m_minVal;
                    m_maxVal  =-1e12;
                    m_minVal  = 1e12;
                }
                m_nCycles--;

                if( m_risEdge > 0 ) m_period = time-m_risEdge; // period = this_edge_time - last_edge_time
                m_risEdge = time;
                //if( m_trigger ) m_oscope->triggerEvent();
    }   }   }

    else if( delta < -m_filter )         // Falling
    {
        if( m_rising && !m_falling )    // Max Found
        {
            m_rising = false;
        }
        m_falling = true;
        m_lastValue = data;
}   }

void OscopeChannel::setFilter( double f )
{
    m_risEdge = 0;
    m_nCycles = 0;
    m_totalP  = 0;
    m_numMax  = 0;

    m_filter = f;
}
