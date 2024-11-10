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


#ifndef AUDIOIO_H
#define AUDIOIO_H

#include <QObject>
#include <QIODevice>
#include <QAudioFormat>

class Oscope;

class AudioIO : public QIODevice
{
    Q_OBJECT

    public:
        AudioIO( QAudioFormat &format, Oscope* oscope );

        void start();
        void stop();

        uint64_t time() { return m_time; }

        qint64 readData( char* data, qint64 maxlen ) override;
        qint64 writeData( const char* data, qint64 len ) override;
    signals:
        void update();

    private:
        const QAudioFormat m_format;

        uint64_t m_time;
        uint64_t m_timeDelta;

        Oscope* m_oscope;
};

#endif
