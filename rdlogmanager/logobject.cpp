// logobject.cpp
//
// Generate/merge logs from the command line.
//
//   (C) Copyright 2018 Fred Gleason <fredg@paravelsystems.com>
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

#include <stdlib.h>

#include <qapplication.h>

#include <rdapplication.h>
#include <rdcreate_log.h>
#include <rddatedecode.h>
#include <rdlog.h>
#include <rdsvc.h>

#include "logobject.h"

LogObject::LogObject(const QString &svcname,int start_offset,
		     bool protect_existing,bool gen_log,bool merge_mus,
		     bool merge_tfc,QObject *parent)
  : QObject(parent)
{
  QString err_msg;

  log_service_name=svcname;
  log_start_offset=start_offset;
  log_protect_existing=protect_existing;
  log_generate_log=gen_log;
  log_merge_music=merge_mus;
  log_merge_traffic=merge_tfc;

  //
  // Open the Database
  //
  rda=new RDApplication("RDLogManager","rdlogmanager","");
  if(!rda->open(&err_msg)) {
    fprintf(stderr,"rdlogmanager: %s\n",(const char *)err_msg);
    exit(1);
  }

  connect(rda,SIGNAL(userChanged()),this,SLOT(userData()));
  rda->ripc()->connectHost("localhost",RIPCD_TCP_PORT,
			   rda->config()->password());
}
  

void LogObject::userData()
{
  QString err_msg;
  QString sql;
  RDSqlQuery *q;
  QString report;
  QString unused_report;
  QString svcname_table=log_service_name;
  svcname_table.replace(" ","_");

  if(rda->user()->createLog()) {
    //
    // Some Basic Structures
    //
    RDSvc *svc=new RDSvc(log_service_name,rda->station(),rda->config());
    if(!svc->exists()) {
      fprintf(stderr,"rdlogmanager: no such service\n");
      exit(256);
    }
    QDate start_date=QDate::currentDate().addDays(1+log_start_offset);
    QString logname=
      RDDateDecode(svc->nameTemplate(),start_date,rda->station(),rda->config(),
		   svc->name());
    RDLog *log=new RDLog(logname);

    //
    // Generate Log
    //
    if(log_generate_log) {
      if(log_protect_existing&&log->exists()) {
	fprintf(stderr,"log \"%s\" already exists\n",
		(const char *)log->name().utf8());
	exit(256);
      }
      SendNotification(RDNotification::DeleteAction,log->name());
      log->removeTracks(rda->station(),rda->user(),rda->config());
      srand(QTime::currentTime().msec());
      sql=RDCreateStackTableSql(svcname_table,rda->config());
      q=new RDSqlQuery(sql);
      if(!q->isActive()) {
	fprintf(stderr,"SQL: %s\n",(const char *)sql);
	fprintf(stderr,"SQL Error: %s\n",
		(const char *)q->lastError().databaseText());
      }
      delete q;
      if(!svc->generateLog(start_date,
			   RDDateDecode(svc->nameTemplate(),start_date,
				   rda->station(),rda->config(),svc->name()),
			   RDDateDecode(svc->nameTemplate(),start_date.addDays(1),
				   rda->station(),rda->config(),svc->name()),
			   &unused_report,rda->user(),&err_msg)) {
	fprintf(stderr,"rdlogmanager: %s\n",(const char *)err_msg);
	exit(256);
      }
      log->updateTracks();
      SendNotification(RDNotification::AddAction,log->name());

      //
      // Generate Exception Report
      //
      RDLogEvent *event=
	new RDLogEvent(QString().sprintf("%s_LOG",(const char *)logname));
      event->load();
      if((event->validate(&report,start_date)!=0)||
	 (!unused_report.isEmpty())) {
	printf("%s\n\n%s",(const char*)report,(const char*)unused_report);
      }
      delete event;
    }

    //
    // Merge Music
    //
    if(log_merge_music) {
      if(!log->exists()) {
	fprintf(stderr,"rdlogmanager: log does not exist\n");
	exit(256);
      }
      if(log_protect_existing&&
	 (log->linkState(RDLog::SourceMusic)==RDLog::LinkDone)) {
	fprintf(stderr,
		"rdlogmanager: music for log \"%s\" is already imported\n",
		(const char *)log->name().utf8());
	exit(256);
      }
      report="";
      log->removeTracks(rda->station(),rda->user(),rda->config());
      if(!svc->clearLogLinks(RDSvc::Traffic,logname,rda->user(),&err_msg)) {
	fprintf(stderr,"rdlogmanager: %s\n",(const char *)err_msg);
	exit(256);
      }
      if(!svc->clearLogLinks(RDSvc::Music,logname,rda->user(),&err_msg)) {
	fprintf(stderr,"rdlogmanager: %s\n",(const char *)err_msg);
	exit(256);
      }
      if(svc->linkLog(RDSvc::Music,start_date,logname,&report,rda->user(),
		      &err_msg)) {
	printf("%s\n",(const char*)report);
      }
      else {
	fprintf(stderr,"rdlogmanager: %s\n",(const char *)err_msg);
	exit(256);
      }
      SendNotification(RDNotification::ModifyAction,log->name());
    }

    //
    // Merge Traffic
    //
    if(log_merge_traffic) {
      if(!log->exists()) {
	fprintf(stderr,"rdlogmanager: log does not exist\n");
	exit(256);
      }
      if(log_protect_existing&&
	 (log->linkState(RDLog::SourceTraffic)==RDLog::LinkDone)) {
	fprintf(stderr,
		"rdlogmanager: traffic for log \"%s\" is already imported\n",
		(const char *)log->name().utf8());
	exit(256);
      }
      report="";
      if(!svc->clearLogLinks(RDSvc::Traffic,logname,rda->user(),&err_msg)) {
	fprintf(stderr,"rdlogmanager: %s\n",(const char *)err_msg);
	exit(256);
      }
      if(svc->linkLog(RDSvc::Traffic,start_date,logname,&report,rda->user(),
		      &err_msg)) {
	printf("%s\n",(const char*)report);
      }
      else {
	fprintf(stderr,"rdlogmanager: %s\n",(const char *)err_msg);
      }
      SendNotification(RDNotification::ModifyAction,log->name());
    }

    //
    // Clean Up
    //
    delete log;
    delete svc;
    exit(0);
  }
  fprintf(stderr,"rdlogmanager: insufficient permissions\n");
  exit(1);
}


void LogObject::SendNotification(RDNotification::Action action,
				 const QString &logname)
{
  RDNotification *notify=new RDNotification(RDNotification::LogType,
					    action,QVariant(logname));
  rda->ripc()->sendNotification(*notify);
  qApp->processEvents();
  delete notify;
}
