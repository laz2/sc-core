"""
-----------------------------------------------------------------------------
This source file is part of OSTIS (Open Semantic Technology for Intelligent Systems)
For the latest info, see http://www.ostis.net

Copyright (c) 2010 OSTIS

OSTIS is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

OSTIS is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with OSTIS.  If not, see <http://www.gnu.org/licenses/>.
-----------------------------------------------------------------------------
"""


'''
Created on Aug 28, 2010

@author: Denis Koronchik
'''

from distutils.core import setup, Extension
import sys
import os
import os.path

include_dirs = []
library_dirs = []

# Search sc-core from SC_CORE_HOME
sc_core_home = os.environ.get('SC_CORE_HOME')
print "Finding sc-core module..."
if sc_core_home:
    print "Use '%s' as sc-core root directory" % (sc_core_home)
    def check_dir(path):
        if not os.path.isdir(path):
            print "\tNot found '%s' from enviroment var SC_CORE_HOME" % (path)
            sys.exit(2)
        print "Found '%s'" % (path)
        return path
    sc_core_home = check_dir(sc_core_home)
    sc_core_include_dir = check_dir(os.path.join(sc_core_home, "include"))
    library_dirs.append(check_dir(os.path.join(sc_core_home, "lib")))
    
    include_dirs.append(sc_core_include_dir)
else:
    print "Enviroment variable SC_CORE_HOME not found"
    sys.exit(1)


DESCRIPTION = """SC-core is a model of semantic code memory."""

METADATA = {
    "name":             "pysc-core",
    "version":          "0.2.5",
    "license":          "LGPL",
    "url":              "http://www.ostis.net/",
    "author":           "Dmitry Lazurkin",
    "author_email":     "",
    "description":      "swigwin python binding for sc-core",
    "long_description": DESCRIPTION,
}

swig_opts = ['-c++', '-Iswig/', '-outdir', 'sc_core']
for include_dir in include_dirs:
    swig_opts.append('-I"%s"' % include_dir)

include_dirs.append(os.path.join(os.path.abspath('.'), 'swig'))

extra_compile_args = []

if sys.platform == 'win32':
    extra_compile_args.append('-DWIN32')
    extra_compile_args.append('/EHsc')

PACKAGEDATA = {
       "ext_modules": [Extension(
                            name='sc_core._tgf',
                            sources=['tgf_module.i'],
                            include_dirs=include_dirs, 
                            library_dirs=library_dirs, 
                            libraries=['libtgf'], 
                            swig_opts=swig_opts,
                            extra_compile_args=extra_compile_args),
                       Extension(
                            name='sc_core._pm',
                            sources=['libsc.i'],
                            include_dirs=include_dirs, 
                            library_dirs=library_dirs, 
                            libraries=['libsys', 'libtgf', 'libsc', 'libpm'], 
                            swig_opts=swig_opts,
                            extra_compile_args=extra_compile_args)],
       "packages":    [ 'sc_core' ],
       "package_dir": {'': ''},
       "package_data": {'': ['*.pyd', '*.dll', '*.so', '*.dylib']}

}

if __name__ == '__main__':
    PACKAGEDATA.update(METADATA)
    setup(**PACKAGEDATA)
