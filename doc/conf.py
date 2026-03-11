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

from pathlib import Path

source_dir = os.environ.get('NESTSRCDIR', False)
if source_dir:
    source_dir = Path(source_dir)
else:
    source_dir = Path(__file__).resolve().parent

print("source_dir", str(source_dir))

sys.path.insert(0, str(source_dir))
sys.path.insert(0, str(source_dir / 'doc'))

# -- General configuration ------------------------------------------------
extensions = [
    'sphinx.ext.autodoc',
    'sphinx.ext.napoleon',
    'sphinx.ext.autosummary',
    'sphinx.ext.doctest',
    'sphinx.ext.intersphinx',
    'sphinx.ext.mathjax'
]

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# General information about the project.
project = 'NEST simulator extension module documentation'
copyright = '2004, nest-simulator'
author = 'nest-simulator'

# The short X.Y version.
version = '1.0'
# The full version, including alpha/beta/rc tags
release = '1.0'

language = 'en'

source_suffix = '.rst'

# The master toctree document.
master_doc = 'index'

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
exclude_patterns = ['Thumbs.db', '.DS_Store', 'nest_by_example', 'README.md']

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = 'manni'

# If true, `todo` and `todoList` produce output, else they produce nothing.
todo_include_todos = False

# add numbered figure link
numfig = True
numfig_secnum_depth = 2
numfig_format = {'figure': 'Figure %s', 'table': 'Table %s',
                 'code-block': 'Code Block %s'}

# -- Options for HTML output ----------------------------------------------

html_theme = 'sphinx_rtd_theme'
html_logo = 'fig/nest_logo.png'
html_theme_options = {
    'logo_only': True,
    'navigation_depth': 2,
    'collapse_navigation': False,
}

# Paths containing custom static files; contents are copied to _static/.
# 'css/' → _static/custom.css, _static/pygments.css
html_static_path = ['css']
html_css_files = ['custom.css', 'pygments.css']

# -- Options for HTMLHelp output ------------------------------------------

htmlhelp_basename = 'NESTsimulatorextmoddoc'

html_show_sphinx = False
html_show_copyright = False

intersphinx_mapping = {
    'python': ('https://docs.python.org/3/', None),
}


# -- Options for LaTeX output ---------------------------------------------

latex_elements = {}

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title, author, documentclass).
latex_documents = [
    (master_doc, 'NESTextmod.tex', 'NEST Simulator extension module documentation',
     'NEST Developer Community', 'manual'),
]
