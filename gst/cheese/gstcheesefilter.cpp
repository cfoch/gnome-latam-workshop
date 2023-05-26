/*
 * GStreamer
 * Copyright (C) 2006 Stefan Kost <ensonic@users.sf.net>
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

/**
 * SECTION:element-cheese_filter
 *
 * FIXME:Describe cheese_filter here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! cheese_filter ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/base/base.h>
#include <gst/controller/controller.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>

#include "gstcheesefilter.h"

GST_DEBUG_CATEGORY_STATIC (gst_cheesefilter_debug);
#define GST_CAT_DEFAULT gst_cheesefilter_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_COLOR,
  PROP_RADIUS,
  PROP_N_CIRCLES
};

#define DEFAULT_RADIUS    0.02
#define DEFAULT_N_CIRCLES 10
#define DEFAULT_COLOR     GST_CHEESE_FILTER_COLOR_WHITE

/* the capabilities of the inputs and outputs.
 *
 * FIXME:describe the real formats here.
 */
static GstStaticPadTemplate sink_template = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE ("RGB"))
    );

static GstStaticPadTemplate src_template = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (GST_VIDEO_CAPS_MAKE ("RGB"))
    );

#define gst_cheesefilter_parent_class parent_class
G_DEFINE_TYPE (Gstcheesefilter, gst_cheesefilter, GST_TYPE_OPENCV_VIDEO_FILTER);
GST_ELEMENT_REGISTER_DEFINE (cheesefilter, "cheesefilter", GST_RANK_NONE,
    GST_TYPE_CHEESE_FILTER);

static void gst_cheesefilter_set_property (GObject * object,
    guint prop_id, const GValue * value, GParamSpec * pspec);
static void gst_cheesefilter_get_property (GObject * object,
    guint prop_id, GValue * value, GParamSpec * pspec);

static GstFlowReturn gst_cheesefilter_transform_ip (GstOpencvVideoFilter *
    base, GstBuffer * outbuf, cv::Mat img);

/* GObject vmethod implementations */

#define GST_TYPE_CHEESE_FILTER_COLOR (gst_cheesefilter_color_get_type ())
static GType
gst_cheesefilter_color_get_type (void)
{
  static GType cheese_filter_color_type = 0;
  static const GEnumValue color_types[] = {
    {GST_CHEESE_FILTER_COLOR_WHITE, "White color", "white"},
    {GST_CHEESE_FILTER_COLOR_BLACK, "Black color", "black"},
    {GST_CHEESE_FILTER_COLOR_BLUE, "Blue color", "blue"},
    {GST_CHEESE_FILTER_COLOR_GREEN, "Green color", "green"},
    {GST_CHEESE_FILTER_COLOR_RED, "Red color", "red"},
    {0, NULL, NULL}
  };

  if (!cheese_filter_color_type) {
    cheese_filter_color_type =
        g_enum_register_static ("GstCheesefilterColor", color_types);
  }
  return cheese_filter_color_type;
}

/* initialize the cheese_filter's class */
static void
gst_cheesefilter_class_init (GstcheesefilterClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_cheesefilter_set_property;
  gobject_class->get_property = gst_cheesefilter_get_property;

  g_object_class_install_property (gobject_class, PROP_COLOR,
      g_param_spec_enum ("color", "Color", "Circle color", GST_TYPE_CHEESE_FILTER_COLOR,
          DEFAULT_COLOR, (GParamFlags) (G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE)));

  g_object_class_install_property (gobject_class, PROP_RADIUS,
      g_param_spec_double ("radius", "Radius", "Radius size relative to image height",
          0, 1.0, DEFAULT_RADIUS,
          (GParamFlags) (G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE)));

  g_object_class_install_property (gobject_class, PROP_N_CIRCLES,
      g_param_spec_uint ("n-circles", "N Circles", "Number of circles to display",
          0, G_MAXUINT, DEFAULT_N_CIRCLES,
          (GParamFlags) (G_PARAM_READWRITE | GST_PARAM_CONTROLLABLE)));

  gst_element_class_set_details_simple (gstelement_class,
      "cheesefilter",
      "Generic/Filter",
      "FIXME:Generic Template Filter", "forccon <<user@hostname.org>>");

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_template));
  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sink_template));

  GST_OPENCV_VIDEO_FILTER_CLASS (klass)->cv_trans_ip_func =
      GST_DEBUG_FUNCPTR (gst_cheesefilter_transform_ip);

  /* debug category for fltering log messages
   *
   * FIXME:exchange the string 'Template cheese_filter' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_cheesefilter_debug, "cheese_filter", 0,
      "Template cheese_filter");
}

/* initialize the new element
 * initialize instance structure
 */
static void
gst_cheesefilter_init (Gstcheesefilter * filter)
{
  filter->silent = FALSE;
  filter->color = DEFAULT_COLOR;
  filter->cv_color = cv::Scalar(255, 255, 255);
  filter->radius = DEFAULT_RADIUS;
  filter->n_circles = DEFAULT_N_CIRCLES;
  gst_opencv_video_filter_set_in_place (GST_OPENCV_VIDEO_FILTER_CAST (filter),
      TRUE);
}

static void
gst_cheesefilter_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  Gstcheesefilter *filter = GST_CHEESE_FILTER (object);

  switch (prop_id) {
    case PROP_COLOR:
    {
      GST_OBJECT_LOCK (filter);
      filter->color = (GstCheesefilterColor) g_value_get_enum (value);
      switch (filter->color) {
        case GST_CHEESE_FILTER_COLOR_WHITE:
          filter->cv_color = cv::Scalar (255, 255, 255);
          break;
        case GST_CHEESE_FILTER_COLOR_BLACK:
          filter->cv_color = cv::Scalar (0, 0, 0);
          break;
        case GST_CHEESE_FILTER_COLOR_BLUE:
          filter->cv_color = cv::Scalar (0, 0, 255);
          break;
        case GST_CHEESE_FILTER_COLOR_GREEN:
          filter->cv_color = cv::Scalar (0, 255, 0);
          break;
        case GST_CHEESE_FILTER_COLOR_RED:
          filter->cv_color = cv::Scalar (255, 0, 0);
          break;
        default:
          g_assert_not_reached ();
      }
      GST_OBJECT_UNLOCK (filter);
      break;
    }
    case PROP_RADIUS:
      GST_OBJECT_LOCK (filter);
      filter->radius = g_value_get_double (value);
      GST_OBJECT_UNLOCK (filter);
      break;
    case PROP_N_CIRCLES:
      GST_OBJECT_LOCK (filter);
      filter->n_circles = g_value_get_uint (value);
      GST_OBJECT_UNLOCK (filter);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_cheesefilter_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  Gstcheesefilter *filter = GST_CHEESE_FILTER (object);

  switch (prop_id) {
    case PROP_COLOR:
      GST_OBJECT_LOCK (filter);
      g_value_set_enum (value, filter->color);
      GST_OBJECT_UNLOCK (filter);
      break;
    case PROP_RADIUS:
      GST_OBJECT_LOCK (filter);
      g_value_set_double (value, filter->radius);
      GST_OBJECT_UNLOCK (filter);
      break;
    case PROP_N_CIRCLES:
      GST_OBJECT_LOCK (filter);
      g_value_set_uint (value, filter->n_circles);
      GST_OBJECT_UNLOCK (filter);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* GstOpencvVideoFilter vmethod implementations */

/* this function does the actual processing
 */
static GstFlowReturn
gst_cheesefilter_transform_ip (GstOpencvVideoFilter * base, GstBuffer * outbuf,
  cv::Mat img)
{
  Gstcheesefilter *filter = GST_CHEESE_FILTER (base);
  cv::Mat mask;
  guint i;

  if (GST_CLOCK_TIME_IS_VALID (GST_BUFFER_TIMESTAMP (outbuf)))
    gst_object_sync_values (GST_OBJECT (filter), GST_BUFFER_TIMESTAMP (outbuf));

  GST_OBJECT_LOCK (filter);
  for (i = 0; i < filter->n_circles; i++) {
      cv::Point center(
          g_random_int_range(0, img.cols),
          g_random_int_range(0, img.rows)
      );

      cv::circle(img, center, img.rows * filter->radius, filter->cv_color, -1);
  }
  GST_OBJECT_UNLOCK (filter);

  return GST_FLOW_OK;
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
cheese_init (GstPlugin * plugin)
{
  return GST_ELEMENT_REGISTER (cheesefilter, plugin);
}

/* gstreamer looks for this structure to register cheese_filters
 *
 * FIXME:exchange the string 'Template cheese_filter' with you cheese_filter description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    cheese,
    "cheese",
    cheese_init,
    PACKAGE_VERSION, GST_LICENSE, GST_PACKAGE_NAME, GST_PACKAGE_ORIGIN)
