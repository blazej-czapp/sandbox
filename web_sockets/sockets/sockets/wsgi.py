"""
WSGI config for sockets project.

It exposes the WSGI callable as a module-level variable named ``application``.

For more information on this file, see
https://docs.djangoproject.com/en/3.0/howto/deployment/wsgi/
"""

import os

from django.core.wsgi import get_wsgi_application, WSGIHandler

os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'sockets.settings')

application = get_wsgi_application()

# class Interceptor(WSGIHandler):
#     def __init__(self, *args, **kwargs):
#         super().__init__(*args, **kwargs)

#     def __call__(self, environ, start_response):
#         print(environ)
#         # print(environ['wsgi.input'].read())
#         return super().__call__(environ, start_response)

# application = Interceptor()