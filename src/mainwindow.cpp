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

#include <QApplication>
#include <QStyleFactory>
#include <QToolButton>
#include <QComboBox>
#include <QSettings>
#include <QDebug>

#include "mainwindow.h"
#include "oscwidget.h"
#include "about.h"

MainWindow::MainWindow()
          : Oscope()
          , m_infoMenu( this )
{
    setWindowIcon( QIcon(":/simoscope.png") );

    m_version = "SimOscope-"+QString( APP_VERSION );
    this->setWindowTitle( m_version );

    QApplication::setStyle( QStyleFactory::create("Fusion") );

    createActions();

    QLabel* label = new QLabel( tr("Input device:  ") );
    QFont font = label->font();
    font.setPointSize( 10 );
    font.setBold( true );
    label->setFont( font );
    m_toolBar.addWidget( label );

    m_deviceBox = new QComboBox( this );
    m_toolBar.addWidget( m_deviceBox );

    QWidget* spacer = new QWidget( this );
    spacer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred) ;
    m_toolBar.addWidget( spacer );

    m_infoMenu.addAction( m_aboutAct );
    m_infoMenu.addAction( m_aboutQtAct );
    QToolButton* infoButton = new QToolButton( this );
    infoButton->setToolTip( tr("Info") );
    infoButton->setMenu( &m_infoMenu );
    infoButton->setIcon( QIcon(":/help.png") );
    infoButton->setPopupMode( QToolButton::InstantPopup );
    m_toolBar.addWidget( infoButton );

    spacer = new QWidget( this );
    spacer->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Preferred) ;
    m_toolBar.addWidget( spacer );

    label = new QLabel( tr("Start/Stop:  ") );
    label->setFont( font );
    m_toolBar.addWidget( label );
    m_toolBar.addAction( m_powerAct );
    m_toolBar.addAction( m_pauseAct );

    connect( m_deviceBox, QOverload<int>::of(&QComboBox::activated), this, &MainWindow::deviceChanged );

    this->addToolBar( &m_toolBar );

    setCentralWidget( m_oscWidget  );
    m_oscWidget->show();

    this->resize( 1024, 600 );

    const QAudioDeviceInfo &defaultDeviceInfo = QAudioDeviceInfo::defaultInputDevice();

    m_deviceBox->addItem( defaultDeviceInfo.deviceName(), QVariant::fromValue(defaultDeviceInfo) );

    for( auto &deviceInfo: QAudioDeviceInfo::availableDevices( QAudio::AudioInput ) )
    {
        if( deviceInfo != defaultDeviceInfo )
        {
            QAudioFormat format = deviceInfo.preferredFormat();

            int channels = format.channelCount();
            qDebug() << "\nAdding:"  << deviceInfo.deviceName();
            qDebug() << "sampleRate" << format.sampleRate()<<1e12/format.sampleRate();
            qDebug() << "sampleSize" << format.sampleSize();
            qDebug() << "channels"   << channels;
            qDebug() << "codec"      << format.codec();
            if( channels ) m_deviceBox->addItem( deviceInfo.deviceName(), QVariant::fromValue( deviceInfo ) );
        }
    }
    initializeAudio( defaultDeviceInfo );

    powerOff();
}
MainWindow::~MainWindow()
{
    stop();
    if( m_audioIO ) delete m_audioIO;
    if( m_audioIn ) delete m_audioIn;
}

void MainWindow::initializeAudio( const QAudioDeviceInfo &deviceInfo )
{
    qDebug() << "\nInitializing:" << deviceInfo.deviceName();
    QAudioFormat format = deviceInfo.preferredFormat();

    int channels = format.channelCount();
    qDebug() << format.sampleRate()<<1e12/format.sampleRate();
    qDebug() << format.sampleSize();
    qDebug() << channels;
    qDebug() << format.codec();

    setTracks( channels );

    for( int i=0; i<4; ++i )
    {
        bool connect = i < channels;
        m_connect[i] = connect;
        display()->connectChannel( i, connect );
        hideChannel( i, !connect );
    }
    //format.setSampleRate(8000);
    //format.setChannelCount(1);
    format.setSampleSize( 16 );
    format.setSampleType( QAudioFormat::SignedInt );
    format.setByteOrder( QAudioFormat::LittleEndian );
    format.setCodec("audio/pcm");

    if( !deviceInfo.isFormatSupported( format ) ) {
        qWarning() << "Default format not supported - trying to use nearest";
        format = deviceInfo.nearestFormat( format );
    }
    m_audioIO = new AudioIO( format, this );
    m_audioIn = new QAudioInput( deviceInfo, format );
}

void MainWindow::start()
{
    if( m_timerId != 0 ) return;
    m_timerId = this->startTimer( 50, Qt::PreciseTimer );

    initialize();

    m_audioIO->start();
    m_audioIn->start( m_audioIO );
}

void MainWindow::stop()
{
    if( m_timerId == 0 ) return;
    this->killTimer( m_timerId );
    m_timerId = 0;

    m_audioIO->stop();
    m_audioIn->stop();
}

void MainWindow::timerEvent( QTimerEvent* e )  //update at m_timerTick rate (50 ms, 20 Hz max)
{
    e->accept();
    updateStep();
}

void MainWindow::deviceChanged( int index )
{
    stop();

    m_audioIn->disconnect( this );

    initializeAudio( m_deviceBox->itemData(index).value<QAudioDeviceInfo>() );

    if( m_powerAct->iconText() == "On" ) start();
}

void MainWindow::power()
{
    if     ( m_powerAct->iconText() == "Off" ) powerOn();
    else if( m_powerAct->iconText() == "On" )  powerOff();
}

void MainWindow::powerOn()
{
   if( !m_audioIO || !m_audioIn ) return;

    m_powerAct->setIcon( QIcon(":/poweron.png") );
    m_powerAct->setEnabled( true );
    m_powerAct->setIconText("On");
    m_pauseAct->setIcon( QIcon(":/pause.png") );
    m_pauseAct->setEnabled( true );
    ///this->setState("▶");
    this->start();
}

void MainWindow::powerOff()
{
    if( !m_audioIO || !m_audioIn ) return;

    this->stop();

    m_powerAct->setIcon( QIcon(":/poweroff.png") );
    m_powerAct->setIconText("Off");
    m_powerAct->setEnabled( true );
    m_pauseAct->setEnabled( false );
    ///this->setState("■");
}

void MainWindow::pause()
{
    if( m_paused )
    {
        m_paused = false;
        m_pauseAct->setIcon( QIcon(":/pause.png") );
        m_powerAct->setIcon( QIcon(":/poweron.png") );
        m_powerAct->setIconText("On");
        ///this->setState("▶");
    }else{
        m_paused = true;
        m_pauseAct->setIcon( QIcon(":/paused.png") );
        m_powerAct->setIcon( QIcon(":/poweroff.png") );
        m_powerAct->setIconText("Off");
        ///this->setState("❚❚");
    }
}

void MainWindow::slotSettings()
{

}

void MainWindow::about()
{
    if( !m_about ) m_about = new AboutDialog( this );
    m_about->show();
}

void MainWindow::createActions()
{
    /*newCircAct = new QAction( QIcon(":/newcirc.png"), tr("New C&ircuit\tCtrl+N"), this);
    newCircAct->setStatusTip( tr("Create a new Circuit"));
    connect( newCircAct, SIGNAL( triggered()),
                   this, SLOT( newCircuit()), Qt::UniqueConnection );

    openCircAct = new QAction( QIcon(":/opencirc.png"), tr("&Open Circuit\tCtrl+O"), this);
    openCircAct->setStatusTip( tr("Open an existing Circuit"));
    connect( openCircAct, SIGNAL( triggered()),
                    this, SLOT(openCirc()), Qt::UniqueConnection );

    saveCircAct = new QAction( QIcon(":/savecirc.png"), tr("&Save Circuit\tCtrl+S"), this);
    saveCircAct->setStatusTip( tr("Save the Circuit to disk"));
    connect( saveCircAct, SIGNAL( triggered()),
                    this, SLOT(saveCirc()), Qt::UniqueConnection );

    saveCircAsAct = new QAction( QIcon(":/savecircas.png"),tr("Save Circuit &As...\tCtrl+Shift+S"), this);
    saveCircAsAct->setStatusTip( tr("Save the Circuit under a new name"));
    connect( saveCircAsAct, SIGNAL( triggered()),
                      this, SLOT(saveCircAs()), Qt::UniqueConnection );*/

    m_settingsAct = new QAction( QIcon(":/config.png"),tr("Settings"), this);
    m_settingsAct->setStatusTip( tr("Settings") );
    connect( m_settingsAct, SIGNAL( triggered() ),
                   this, SLOT( slotSettings() ), Qt::UniqueConnection );

    m_powerAct = new QAction( QIcon(":/poweroff.png"),tr("Power On"), this);
    m_powerAct->setStatusTip( tr("Power On") );
    m_powerAct->setIconText("Off");
    connect( m_powerAct, SIGNAL( triggered() ),
                     this, SLOT( power()) , Qt::UniqueConnection );

    m_pauseAct = new QAction( QIcon(":/pause.png"),tr("Pause"), this);
    m_pauseAct->setStatusTip( tr("Pause") );
    connect( m_pauseAct, SIGNAL( triggered() ),
             this, SLOT( pause() ), Qt::UniqueConnection );


    m_aboutAct = new QAction( QIcon(":/about.png"),tr("About SimOscope"), this);
    m_aboutAct->setStatusTip(tr("About SimOscope"));
    connect( m_aboutAct, SIGNAL( triggered()),
                 this, SLOT(about()), Qt::UniqueConnection );

    m_aboutQtAct = new QAction( QIcon(":/about.png"),tr("About Qt"), this);
    m_aboutQtAct->setStatusTip(tr("About Qt"));
    connect( m_aboutQtAct, SIGNAL(triggered()),
                   qApp, SLOT(aboutQt()), Qt::UniqueConnection );
}
