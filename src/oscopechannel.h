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

#ifndef OSCOPECHANNEL_H
#define OSCOPECHANNEL_H

#include <QVector>

#include "oscope.h"

class OscopeChannel
{
        friend class Oscope;
        friend class PlotDisplay;

    public:
        OscopeChannel( Oscope* oscope, QString id );
        ~OscopeChannel();

        void initialize();
        void updateStep( uint64_t simTime );
        void voltChanged( uint64_t time, double data );

        void setFilter( double f );

    private:
        void updateValues();

        QVector<double> m_buffer;
        QVector<uint64_t> m_time;

        uint64_t m_totalP;
        uint64_t m_lastMax;
        uint64_t m_numMax;       // Number of Maximum found
        uint64_t m_nCycles;

        int m_dataTime;

        uint64_t m_risEdge;
        uint64_t m_period;

        bool m_rising;
        bool m_falling;
        bool m_trigger;
        int m_trigIndex;

        int m_channel;
        int m_bufferCounter;

        double m_lastValue;
        double m_freq;
        double m_mid;

        double m_maxVal;
        double m_minVal;
        double m_midVal;
        double m_dispMax;
        double m_dispMin;
        double m_ampli;
        double m_filter;

        Oscope* m_oscope;
};

#endif

