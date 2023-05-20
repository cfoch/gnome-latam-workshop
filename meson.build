project('gnome-latam-workshop', 'c', version : '0.0.0', license : 'LGPL')

plugins_install_dir = join_paths(get_option('libdir'), 'gstreamer-1.0')


pkg_version = meson.project_version()
api_version = '1.0'

gst_dep = dependency('gstreamer-1.0', version : '>=1.19',
    required : true, fallback : ['gstreamer', 'gst_dep'])
gstbase_dep = dependency('gstreamer-base-1.0', version : '>=1.19',
  fallback : ['gstreamer', 'gst_base_dep'])

plugin_c_args = ['-DHAVE_CONFIG_H']

cdata = configuration_data()
cdata.set_quoted('PACKAGE_VERSION', pkg_version)
cdata.set_quoted('PACKAGE', 'gnome-latam-workshop')
cdata.set_quoted('GST_LICENSE', 'LGPL')
cdata.set_quoted('GST_API_VERSION', api_version)
cdata.set_quoted('GST_PACKAGE_NAME', 'GNOME Latam: Cheese Filter plug-in')
cdata.set_quoted('GST_PACKAGE_ORIGIN', 'https://github.com/fluendo/gnome-latam-workshop')

subdir('gst')
