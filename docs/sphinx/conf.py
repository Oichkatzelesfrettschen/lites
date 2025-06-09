import os
import sys

project = 'Lites'
extensions = ['breathe']

breathe_projects = {
    'Lites': os.path.join(os.path.dirname(__file__), '..', '..', 'html', 'xml')
}
breathe_default_project = 'Lites'

autodoc_default_options = {
    'members': True,
}

html_theme = 'alabaster'

sys.path.insert(0, os.path.abspath('..'))
