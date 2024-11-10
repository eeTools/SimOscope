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

#include <qendian.h>

#include "audioio.h"
#include "oscope.h"

AudioIO::AudioIO( QAudioFormat &format, Oscope* oscope )
       : m_format( format )
{
    m_oscope = oscope;
    //case 16:
    //case QAudioFormat::SignedInt:
        //m_maxAmplitude = 32767;
}

void AudioIO::start()
{
    open( QIODevice::WriteOnly );
    m_time = 0;
    m_timeDelta = 1e12/m_format.sampleRate();
}
void AudioIO::stop(){  close(); }

qint64 AudioIO::readData( char* data, qint64 maxlen )
{ return 0; }

qint64 AudioIO::writeData( const char* data, qint64 len )
{
    const int numSamples = len / (m_format.channelCount() * 2); // 16 bits signed
    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(data);

    for( int i=0; i<numSamples; ++i )
    {
        for( int ch=0; ch<m_format.channelCount(); ++ch ) {

            //else if( m_format.sampleSize() == 16 && m_format.sampleType() == QAudioFormat::UnSignedInt)
            qint16 value = qFromLittleEndian<quint16>( ptr );

            double volt = value; /// CALCULATE
            volt = volt*5/32767;

            m_oscope->setValue( ch, m_time, volt );

            //maxValue = qMax( value, maxValue );
            ptr += 2;
        }
        m_time += m_timeDelta;
    }
    return len;
}
