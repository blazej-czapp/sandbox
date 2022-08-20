"""
ASGI config for sockets project.

It exposes the ASGI callable as a module-level variable named ``application``.

For more information on this file, see
https://docs.djangoproject.com/en/3.0/howto/deployment/asgi/
"""

import os

from django.core.asgi import get_asgi_application
from my_sockets import my_asgi_app

os.environ.setdefault('DJANGO_SETTINGS_MODULE', 'sockets.settings')

# callable
django_application = get_asgi_application()

# also callable!
async def application(scope, receive, send):
    if scope['type'] == 'http':
        await django_application(scope, receive, send)
    elif scope['type'] == 'websocket':
        await my_asgi_app.websocket_application(scope, receive, send)
    else:
        raise NotImplementedError(f"Unknown scope type {scope['type']}")