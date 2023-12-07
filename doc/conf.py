# -*- coding: utf-8 -*-
#
# conf.py
#
# This file is part of NEST.
#
# Copyright (C) 2004 The NEST Initiative
#
# NEST is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 2 of the License, or
# (at your option) any later version.
#
# NEST is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with NEST.  If not, see <http://www.gnu.org/licenses/>.


import sys
import os
import re
import pip
import subprocess

from pathlib import Path
from shutil import copyfile

source_dir = os.environ.get('NESTSRCDIR', False)
if source_dir:
    source_dir = Path(source_dir)
else:
    source_dir = Path(__file__).resolve().parent.resolve()

doc_build_dir = source_dir

print("doc_build_dir", str(doc_build_dir))
print("source_dir", str(source_dir))

source_suffix = '.rst'
master_doc = 'contents'

os.system('mkdir _static')
os.system('install -v -D css/custom.css _static/css/custom.css')
os.system('install -v -D css/pygments.css _static/css/pygments.css')

sys.path.insert(0, str(source_dir))
sys.path.insert(0, str(source_dir / 'doc'))
sys.path.insert(0, str(doc_build_dir))

# -- General configuration ------------------------------------------------
extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.napoleon',
    'sphinx.ext.autosummary',
    'sphinx.ext.doctest',
    'sphinx.ext.intersphinx',
    'sphinx.ext.mathjax'
]

mathjax_path = "https://cdnjs.cloudflare.com/ajax/libs/mathjax/2.7.4/MathJax.js?config=TeX-AMS-MML_HTMLorMML"  # noqa

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# General information about the project.
project = u'NEST simulator extension module documentation'
copyright = u'2004, nest-simulator'
author = u'nest-simulator'


# The version info for the project you're documenting, acts as replacement for
# |version| and |release|, also used in various other places throughout the
# built documents.
#
# The short X.Y version.
version = '1.0'
# The full version, including alpha/beta/rc tagss
release = '1.0'
# The language for content autogenerated by Sphinx. Refer to documentation
# for a list of supported languages.
#
# This is also used if you do content translation via gettext catalogs.
# Usually you set "language" from the command line for these cases.
language = None

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This patterns also effect to html_static_path and html_extra_path
exclude_patterns = ['Thumbs.db', '.DS_Store', 'nest_by_example', 'README.md']

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = 'manni'

# If true, `todo` and `todoList` produce output, else they produce nothing.
todo_include_todos = False

# add numbered figure link
numfig = True

numfig_secnum_depth = (2)
numfig_format = {'figure': 'Figure %s', 'table': 'Table %s',
                 'code-block': 'Code Block %s'}
# -- Options for HTML output ----------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'
html_logo = str('fig/nest_logo.png')
html_theme_options = {'logo_only': True,
                      'display_version': False,
                      'navigation_depth': 2,
                      'collapse_navigation': False}

# Theme options are theme-specific and customize the look and feel of a theme
# further.  For a list of options available for each theme, see the
# documentation.
#
# html_theme_options = {}

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = [str(doc_build_dir / 'css'),
					str(doc_build_dir / 'fig')]

# -- Options for HTMLHelp output ------------------------------------------

# Output file base name for HTML help builder.
htmlhelp_basename = 'NESTsimulatorextmoddoc'

html_show_sphinx = False
html_show_copyright = False

# This way works for ReadTheDocs
# With this local 'make html' is broken!
github_doc_root = ''

intersphinx_mapping = {'https://docs.python.org/': None}

# The master toctree document.
master_doc = "index"

def setup(app):
    app.add_css_file('css/custom.css')
    app.add_css_file('css/pygments.css')

# -- Options for LaTeX output ---------------------------------------------


latex_elements = {
    # The paper size ('letterpaper' or 'a4paper').
    #
    # 'papersize': 'letterpaper',

    # The font size ('10pt', '11pt' or '12pt').
    #
    # 'pointsize': '10pt',

    # Additional stuff for the LaTeX preamble.
    #
    # 'preamble': '',

    # Latex figure (float) alignment
    #
    # 'figure_align': 'htbp',
}

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title,
#  author, documentclass [howto, manual, or own class]).
latex_documents = [
    (master_doc, 'NESTextmod.tex', u'NEST Simulator extension module documentation',
     u'NEST Developer Community', 'manual'),
]
