import datetime, os
from pathlib import Path

project = "mxsrclib"
author = "ИРС"
copyright = f'{datetime.datetime.today().year}, ООО "ИРС"'
plantuml = f"java -jar {Path(os.path.abspath(__file__)).parent}/.vscode/plantuml.jar"

extensions = [
    'myst_parser',
    'sphinx_tabs.tabs',
    'sphinx_copybutton',
    'sphinx.ext.graphviz',
    'sphinxcontrib.plantuml',
    'breathe',
    'sphinx.ext.todo',
    'sphinx.ext.autodoc',
    'sphinx_substitution_extensions',
]
sphinx_tabs_disable_tab_closing = True
myst_enable_extensions = [
    'substitution',
]
templates_path = ['_templates']
exclude_patterns = ['venv']
html_theme = 'sphinx_rtd_theme'
html_static_path = ["_static_local"]
html_css_files = ["css/custom.css"]
source_suffix = {
    '.md': 'markdown',
    '.rst': 'restructuredtext',
}
breathe_projects = {project: './doxygen/out/xml'}
breathe_default_project = project
breathe_default_members = ('members', 'undoc-members')
