#! /usr/bin/env python

import Options
import Utils
import Build

VERSION = '0.2.0'
APPNAME = 'thunar-dropbox'

top = '.'
out = 'build'

def set_options(opt):
	opt.tool_options('compiler_cc')
	opt.tool_options('gnu_dirs')
	opt.add_option('--libdir', action='store', default="${PREFIX}/lib", help="libdir")

def configure(conf):
	conf.check_tool('compiler_cc')
	conf.check_cfg(package='thunarx-2', uselib_store='THUNARX', mandatory=True, args='--cflags --libs')
	conf.check_cfg(package='gio-2.0', uselib_store='GIO', mandatory=True, args='--cflags --libs')
	conf.env.LIBDIR= Options.options.libdir

def build(bld):
	prog = bld.new_task_gen('cc', 'cshlib')
	prog.target = 'thunar-dropbox'
	prog.install_path = None
	prog.uselib = 'THUNARX THUNARVFS'
	prog.includes = 'src'
	prog.find_sources_in_dirs('src')
	bld.install_files ('${PREFIX}/share/icons/hicolor/16x16/apps', 'data/icons/hicolor/16x16/apps/thunar-dropbox.png')
	bld.install_as (bld.env.LIBDIR + '/thunarx-2/thunar-dropbox.so', 'libthunar-dropbox.so', chmod=0755)

def shutdown():
	if Options.commands['install'] or Options.commands['uninstall']:
		dir = '%s/share/icons/hicolor' % Build.bld.env['PREFIX']
		command = 'gtk-update-icon-cache -q -t -f %s 2> /dev/null' % dir
		try:
			if Utils.exec_command(command):
				Utils.pprint('YELLOW', 'Icon cache not updated.')
		except:
			pass
