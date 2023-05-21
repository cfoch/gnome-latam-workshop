/* 
 * GStreamer
 * Copyright (C) 2006 Stefan Kost <ensonic@users.sf.net>
 * Copyright (C) 2020 Niels De Graef <niels.degraef@gmail.com>
 * Copyright (C) 2023 forccon <<user@hostname.org>>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
 
#ifndef __GST_CHEESE_FILTER_H__
#define __GST_CHEESE_FILTER_H__

#include <gst/gst.h>
#include <gst/opencv/gstopencvvideofilter.h>

G_BEGIN_DECLS

// #define GST_TYPE_CHEESE_FILTER (gst_cheesefilter_get_type())
// G_DECLARE_FINAL_TYPE (Gstcheesefilter, gst_cheesefilter,
//    GST, CHEESE_FILTER, GstOpencvVideoFilter)

#define GST_TYPE_CHEESE_FILTER \
  (gst_cheesefilter_get_type())
#define GST_CHEESE_FILTER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_CHEESE_FILTER,Gstcheesefilter))
#define GST_CHEESE_FILTER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_CHEESE_FILTER,GstcheesefilterClass))
#define GST_IS_CHEESE_FILTER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_CHEESE_FILTER))
#define GST_IS_CHEESE_FILTER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_CHEESE_FILTER))
#define GST_CHEESE_FILTER_CLASS_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS((obj),GST_TYPE_CHEESE_FILTER,GstcheesefilterClass))

typedef struct _Gstcheesefilter  Gstcheesefilter;
typedef struct _GstcheesefilterClass GstcheesefilterClass;

struct _Gstcheesefilter
{
  GstOpencvVideoFilter element;

  gboolean silent;
};

struct _GstcheesefilterClass
{
  GstOpencvVideoFilterClass parent_class;
};

G_END_DECLS

#endif /* __GST_CHEESE_FILTER_H__ */
