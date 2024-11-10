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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QMenu>

#include "oscope.h"

class AboutDialog;
class SettingsWidget;

class MainWindow : public QMainWindow, public Oscope
{
    Q_OBJECT
    public:
        MainWindow();
        ~MainWindow();

        void start();
        void stop();
        void timerEvent( QTimerEvent* e ) ;

    public slots:
        void about();

    private slots:
        void power();
        void pause();
        void slotSettings();

        void deviceChanged( int index );

    private:
        void createActions();
        //void createToolBars();
        void initializeAudio( const QAudioDeviceInfo &deviceInfo );

        void powerOn();
        void powerOff();

        QAction* m_powerAct;
        QAction* m_pauseAct;
        QAction* m_settingsAct;
        QAction* m_aboutQtAct;
        QAction* m_aboutAct;

        QString m_version;

        QComboBox* m_deviceBox;
        AboutDialog* m_about;

        QToolBar m_toolBar;
        QMenu    m_infoMenu;
};

#endif
