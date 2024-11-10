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

#ifndef OSCOPE_H
#define OSCOPE_H

#include <QColor>
#include <QAudioInput>
#include <QAudioDeviceInfo>

#include "audioio.h"

class LibraryItem;
class OscopeChannel;
class PlotDisplay;
class OscWidget;
class DataWidget;
class QComboBox;

class Oscope
{
    public:
        Oscope();
        ~Oscope();

        void initialize();
        void updateStep();

        double filter() { return m_filter; }
        void setFilter( double filter );

        int autoSC() { return m_auto; }
        void setAutoSC( int ch );

        QString hideCh();
        void setHideCh( QString hc );

        int tracks();
        void setTracks( int tracks );

        virtual QString timPos();
        virtual void setTimPos( QString tp );

        virtual QString volDiv();
        virtual void setVolDiv( QString vd );

        QString volPos();
        void setVolPos( QString vp );

        int trigger() { return m_trigger; }
        void setTrigger( int ch );

        uint64_t timeDiv() { return m_timeDiv; }
        void setTimeDiv( uint64_t td );

        int64_t timePos( int ch ){ return m_timePos[ch]; }
        void setTimePos( int ch, int64_t tp );
        virtual void moveTimePos( int64_t delta );

        double voltDiv( int ch ){ return m_voltDiv[ch]; }
        void setVoltDiv( int ch, double vd );

        double voltPos( int ch ){ return m_voltPos[ch]; }
        void setVoltPos( int ch, double vp );

        void hideChannel( int ch, bool hide );

        void setValue( int ch, uint64_t time, double volt );

        PlotDisplay* display() { return m_display; }

        QColor getColor( int c ) { return m_color[c]; }

    protected:
        int m_timerId;

        bool m_paused;

        int m_bufferSize;
        int m_trigger;

        double m_dataSize;

        uint64_t m_audioTime;
        uint64_t m_timeDiv;
        uint64_t m_risEdge;

        int m_numChannels;
        OscopeChannel* m_channel[4];

        QColor m_color[5];

        double m_filter;

        int m_auto;

        int64_t m_timePos[4];
        double  m_voltDiv[4];
        double  m_voltPos[4];
        bool    m_hideCh[4];
        bool    m_connect[4];

        PlotDisplay* m_display;

        OscWidget* m_oscWidget;

        QAudioInput* m_audioIn;
        AudioIO*     m_audioIO;
};

#endif
