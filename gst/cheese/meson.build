sources = [
  'gstcheesefilter.c',
]

configure_file(output : 'config.h', configuration : cdata)

gstcheese_filterexample = library('gstcheese',
  sources,
  c_args: plugin_c_args,
  dependencies : [gst_dep, gstbase_dep],
  install : true,
  install_dir : plugins_install_dir,
)

