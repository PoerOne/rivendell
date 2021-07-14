// timeengine_test.h
//
// Test the RDTimeEngine class
//
//   (C) Copyright 2021 Fred Gleason <fredg@paravelsystems.com>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License version 2 as
//   published by the Free Software Foundation.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public
//   License along with this program; if not, write to the Free Software
//   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//

#ifndef TIMEENGINE_TEST_H
#define TIMEENGINE_TEST_H

#include <QObject>

#include <rdtimeengine.h>

#define TIMEENGINE_TEST_USAGE "--time-event=<id>:<hh:mm:ss> [...]\n"

class MainObject : public QObject
{
  Q_OBJECT;
 public:
  MainObject(QObject *parent=0);

 private slots:
  void timeoutData(int id);

 private:
  RDTimeEngine *d_time_engine;
};


#endif  // TIMEENGINE_TEST_H
