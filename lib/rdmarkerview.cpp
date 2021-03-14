// rdmarkerview.cpp
//
// Widget for displaying/editing cut markers
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

#include <QGraphicsLineItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPen>
#include <QPolygonF>

#include "rdescape_string.h"
#include "rdmarkerview.h"

#define LEFT_MARGIN 20

RDMarkerHandle::RDMarkerHandle(RDMarkerHandle::PointerRole role,
			       PointerType type,void *mkrview,
			       QGraphicsItem *parent)
  : QGraphicsPolygonItem(parent)
{
  setFlags(QGraphicsItem::ItemIsMovable);
  d_marker_view=mkrview;
  d_minimum=-1;
  d_maximum=-1;

  QPolygonF triangle;

  switch(type) {
  case RDMarkerHandle::Start:
    triangle << QPointF(0.0,0.0) << QPointF(-16,8) << QPointF(-16,-8);
    break;

  case RDMarkerHandle::End:
    triangle << QPointF(0.0,0.0) << QPointF(16,8) << QPointF(16,-8);
    break;
  }
  setPolygon(triangle);
  setPen(QPen(RDMarkerHandle::pointerRoleColor(role)));
  setBrush(RDMarkerHandle::pointerRoleColor(role));
  d_name=RDMarkerHandle::pointerRoleTypeText(role);
  d_role=role;
  setToolTip(RDMarkerHandle::pointerRoleTypeText(role));
}


QString RDMarkerHandle::name() const
{
  return d_name;
}


RDMarkerHandle::PointerRole RDMarkerHandle::role() const
{
  return d_role;
}


int RDMarkerHandle::minimum() const
{
  return d_minimum;
}


void RDMarkerHandle::setMinimum(int pos)
{
  d_minimum=pos;
}


int RDMarkerHandle::maximum() const
{
  return d_maximum;
}


void RDMarkerHandle::setMaximum(int pos)
{
  d_maximum=pos;
}


void RDMarkerHandle::setRange(int min,int max)
{
  d_minimum=min;
  d_maximum=max;
}


void RDMarkerHandle::mousePressEvent(QGraphicsSceneMouseEvent *e)
{
  if(e->button()==Qt::LeftButton) {
    e->accept();
    if(d_peers.size()==0) {
      QGraphicsScene *s=scene();
      QList<QGraphicsItem *> items=s->items();
      for(int i=0;i<items.size();i++) {
	if(items.at(i)->toolTip()==toolTip()) {
	  d_peers.push_back(items.at(i));
	}
      }
    }
  }
}


void RDMarkerHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *e)
{
  RDMarkerView *view=static_cast<RDMarkerView *>(d_marker_view);

  //
  // Update the Marker Graphics
  //
  qreal dx=e->pos().rx()-e->lastPos().rx();
  for(int i=0;i<d_peers.size();i++) {
    QGraphicsItem *peer=d_peers.at(i);
    peer->setPos(peer->pos().rx()+dx,peer->pos().ry());
  }

  //
  // Send Position
  //
  view->updatePosition(d_role,pos().x()-LEFT_MARGIN);
}


void RDMarkerHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *e)
{
  RDMarkerView *view=static_cast<RDMarkerView *>(d_marker_view);

  //
  // Send Position
  //
  view->updatePosition(d_role,pos().x()-LEFT_MARGIN);
}


QString RDMarkerHandle::pointerRoleText(PointerRole role)
{
  QString ret=QObject::tr("Unknown");

  switch(role) {
  case RDMarkerHandle::CutStart:
  case RDMarkerHandle::CutEnd:
    ret=QObject::tr("Length");
    break;

  case RDMarkerHandle::TalkStart:
  case RDMarkerHandle::TalkEnd:
    ret=QObject::tr("Talk");
    break;

  case RDMarkerHandle::SegueStart:
  case RDMarkerHandle::SegueEnd:
    ret=QObject::tr("Segue");
    break;

  case RDMarkerHandle::HookStart:
  case RDMarkerHandle::HookEnd:
    ret=QObject::tr("Hook");
    break;

  case RDMarkerHandle::FadeUp:
    ret=QObject::tr("Fade Up");
    break;

  case RDMarkerHandle::FadeDown:
    ret=QObject::tr("Fade Down");
    break;

  case RDMarkerHandle::LastRole:
    break;
  }

  return ret;
}


QString RDMarkerHandle::pointerRoleTypeText(RDMarkerHandle::PointerRole role)
{
  QString ret=QObject::tr("Unknown");

  switch(role) {
  case RDMarkerHandle::CutStart:
    ret=QObject::tr("Cut Start");
    break;

  case RDMarkerHandle::CutEnd:
    ret=QObject::tr("Cut End");
    break;

  case RDMarkerHandle::TalkStart:
    ret=QObject::tr("Talk Start");
    break;

  case RDMarkerHandle::TalkEnd:
    ret=QObject::tr("Talk End");
    break;

  case RDMarkerHandle::SegueStart:
    ret=QObject::tr("Segue Start");
    break;

  case RDMarkerHandle::SegueEnd:
    ret=QObject::tr("Segue End");
    break;

  case RDMarkerHandle::HookStart:
    ret=QObject::tr("Hook Start");
    break;

  case RDMarkerHandle::HookEnd:
    ret=QObject::tr("Hook End");
    break;

  case RDMarkerHandle::FadeUp:
    ret=QObject::tr("Fade Up");
    break;

  case RDMarkerHandle::FadeDown:
    ret=QObject::tr("Fade Down");
    break;

  case RDMarkerHandle::LastRole:
    break;
  }

  return ret;
}


QColor RDMarkerHandle::pointerRoleColor(RDMarkerHandle::PointerRole role)
{
  QColor ret;

  switch(role) {
  case RDMarkerHandle::CutStart:
  case RDMarkerHandle::CutEnd:
    ret=Qt::red;
    break;

  case RDMarkerHandle::TalkStart:
  case RDMarkerHandle::TalkEnd:
    ret=Qt::blue;
    break;

  case RDMarkerHandle::SegueStart:
  case RDMarkerHandle::SegueEnd:
    ret=Qt::cyan;
    break;

  case RDMarkerHandle::HookStart:
  case RDMarkerHandle::HookEnd:
    ret=Qt::magenta;
    break;

  case RDMarkerHandle::FadeUp:
  case RDMarkerHandle::FadeDown:
    ret=Qt::darkYellow;
    break;

  case RDMarkerHandle::LastRole:
    break;
  }

  return ret;
}


RDMarkerView::RDMarkerView(int width,int height,QWidget *parent)
  : QWidget(parent)
{
  d_width=width;
  d_height=height;
  d_scene=NULL;
  d_sample_rate=rda->system()->sampleRate();
  clear();

  d_view=new QGraphicsView(this);

  d_wave_factory=new RDWaveFactory(RDWaveFactory::MultiTrack);

  d_pointer_fields.push_back("START_POINT");
  d_pointer_fields.push_back("END_POINT");
  d_pointer_fields.push_back("TALK_START_POINT");
  d_pointer_fields.push_back("TALK_END_POINT");
  d_pointer_fields.push_back("SEGUE_START_POINT");
  d_pointer_fields.push_back("SEGUE_END_POINT");
  d_pointer_fields.push_back("HOOK_START_POINT");
  d_pointer_fields.push_back("HOOK_END_POINT");
  d_pointer_fields.push_back("FADEUP_POINT");
  d_pointer_fields.push_back("FADEDOWN_POINT");
}


RDMarkerView::~RDMarkerView()
{
  if(d_scene!=NULL) {
    delete d_scene;
  }
  delete d_wave_factory;
  delete d_view;
}


QSize RDMarkerView::sizeHint() const
{
  return QSize(1000,d_height);
}


QSizePolicy RDMarkerView::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::Fixed);
}


int RDMarkerView::audioGain() const
{
  return d_audio_gain;
}


int RDMarkerView::shrinkFactor() const
{
  return d_shrink_factor;
}


int RDMarkerView::pointerValue(RDMarkerHandle::PointerRole role)
{
  return d_pointers[role];
}


bool RDMarkerView::hasUnsavedChanges() const
{
  return d_has_unsaved_changes;
}


void RDMarkerView::updatePosition(RDMarkerHandle::PointerRole role, int offset)
{
  //
  // Careful! Don't overflow 32 bits.
  //
  int64_t pframes=(int64_t)(1152*offset*d_shrink_factor);
  d_pointers[role]=(int)((int64_t)1000*pframes/(int64_t)d_sample_rate);
  d_has_unsaved_changes=true;

  emit pointerValueChanged(role,d_pointers[role]);
}


void RDMarkerView::setAudioGain(int lvl)
{
  if(d_audio_gain!=lvl) {
    d_audio_gain=lvl;
    WriteWave();
  }
}


void RDMarkerView::setShrinkFactor(int sf)
{
  if((d_shrink_factor!=sf)&&(sf<=d_max_shrink_factor)) {
    d_shrink_factor=sf;
    WriteWave();
  }
}


void RDMarkerView::setMaximumShrinkFactor()
{
  setShrinkFactor(d_max_shrink_factor);
}


bool RDMarkerView::setCut(QString *err_msg,unsigned cartnum,int cutnum)
{
  d_cart_number=cartnum;
  d_cut_number=cutnum;
  if(!LoadCutData()) {
    *err_msg=tr("No such cart/cut!");
    return false;
  }
  if(!d_wave_factory->setCut(err_msg,cartnum,cutnum)) {
    return false;
  }
  d_max_shrink_factor=1;
  while(d_wave_factory->energySize()>=(d_width*d_max_shrink_factor)) {
    d_max_shrink_factor=d_max_shrink_factor*2;
  }
  d_pad_size=(d_width*d_max_shrink_factor-d_wave_factory->energySize())/d_max_shrink_factor-1;
  d_shrink_factor=d_max_shrink_factor;
  WriteWave();

  return true;
}


void RDMarkerView::save()
{
  QString sql;

  sql=QString("update CUTS set ");
  for(int i=0;i<RDMarkerHandle::PointerRole::LastRole;i++) {
    sql+=d_pointer_fields.at(i)+QString().sprintf("=%d,",d_pointers[i]);
  }
  sql=sql.left(sql.length()-1)+" where "+
    "CUT_NAME=\""+RDEscapeString(RDCut::cutName(d_cart_number,d_cut_number))+
    "\"";
  RDSqlQuery::apply(sql);
  d_has_unsaved_changes=false;
}


void RDMarkerView::clear()
{
  if(d_scene!=NULL) {
    delete d_scene;
    d_scene=NULL;
  }
  for(int i=0;i<RDMarkerHandle::LastRole;i++) {
    d_pointers[i]=-1;
  }
  d_shrink_factor=1;
  d_max_shrink_factor=1;
  d_pad_size=0;
  d_audio_gain=900;
  d_has_unsaved_changes=false;
}


void RDMarkerView::resizeEvent(QResizeEvent *e)
{
  d_view->setGeometry(0,0,size().width(),size().height());
}


int RDMarkerView::Frame(int msec) const
{
  return (int)((int64_t)msec*(int64_t)d_sample_rate/(d_shrink_factor*1152000));
}


bool RDMarkerView::LoadCutData()
{
  QString sql;
  RDSqlQuery *q=NULL;
  bool ret=false;

  sql=QString("select ")+
    d_pointer_fields.join(",")+","+  // 00 - 09
    "CHANNELS "+                     // 10
    "from CUTS where "+
    "CUT_NAME=\""+
    RDEscapeString(RDCut::cutName(d_cart_number,d_cut_number))+"\"";
  q=new RDSqlQuery(sql);
  if(q->first()) {
    ret=true;
    for(int i=0;i<RDMarkerHandle::LastRole;i++) {
      d_pointers[i]=q->value(i).toInt();
    }
    d_channels=q->value(10).toInt();
  }
  delete q;

  return ret;
}


void RDMarkerView::WriteWave()
{
  //
  // Waveform
  //
  QPixmap wavemap=
    d_wave_factory->generate(d_height-20,d_shrink_factor,d_audio_gain,true);

  if(d_scene!=NULL) {
    d_scene->deleteLater();
  }
  d_scene=new QGraphicsScene(0,0,wavemap.width()+d_pad_size,d_height-20,this);

  d_scene->addRect(0,0,LEFT_MARGIN,d_height-20,QPen(Qt::gray),QBrush(Qt::gray));
  d_scene->addPixmap(wavemap)->setPos(LEFT_MARGIN,0);
  d_scene->addRect(LEFT_MARGIN+wavemap.width(),0,d_pad_size,d_height-20,QPen(Qt::gray),QBrush(Qt::gray));

  //
  // Markers
  //
  DrawMarker(RDMarkerHandle::End,RDMarkerHandle::FadeUp,100);
  DrawMarker(RDMarkerHandle::Start,RDMarkerHandle::FadeDown,100);
  DrawMarker(RDMarkerHandle::Start,RDMarkerHandle::HookStart,80);
  DrawMarker(RDMarkerHandle::End,RDMarkerHandle::HookEnd,80);
  DrawMarker(RDMarkerHandle::Start,RDMarkerHandle::TalkStart,60);
  DrawMarker(RDMarkerHandle::End,RDMarkerHandle::TalkEnd,60);
  DrawMarker(RDMarkerHandle::Start,RDMarkerHandle::SegueStart,40);
  DrawMarker(RDMarkerHandle::End,RDMarkerHandle::SegueEnd,40);
  DrawMarker(RDMarkerHandle::Start,RDMarkerHandle::CutStart,20);
  DrawMarker(RDMarkerHandle::End,RDMarkerHandle::CutEnd,20);

  d_view->setScene(d_scene);
}


void RDMarkerView::DrawMarker(RDMarkerHandle::PointerType type,
			      RDMarkerHandle::PointerRole role,int handle_pos)
{
  QGraphicsLineItem *l_item=NULL;
  RDMarkerHandle *m_item=NULL;

  if(d_pointers[role]>=0) {
    l_item=d_scene->addLine(LEFT_MARGIN+Frame(d_pointers[role]),0,
			    LEFT_MARGIN+Frame(d_pointers[role]),d_height,QPen(RDMarkerHandle::pointerRoleColor(role)));
    l_item->setToolTip(RDMarkerHandle::pointerRoleTypeText(role));

    m_item=new RDMarkerHandle(role,type,this);
    d_scene->addItem(m_item);
    m_item->setPos(LEFT_MARGIN+Frame(d_pointers[role]),handle_pos-12);

    m_item=new RDMarkerHandle(role,type,this);
    d_scene->addItem(m_item);
    m_item->setPos(LEFT_MARGIN+Frame(d_pointers[role]),d_height-handle_pos-8);
  }
}