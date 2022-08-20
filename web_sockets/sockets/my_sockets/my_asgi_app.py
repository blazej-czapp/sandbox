group = {}

async def websocket_application(scope, receive, send):
    group[scope['client'][1]] = send
    while True:
        event = await receive()
        print(scope)
        print(event)
        if event['type'] == 'websocket.connect':
            await send({
                'type': 'websocket.accept'
            })
        if event['type'] == 'websocket.disconnect':
            break

        if event['type'] == 'websocket.receive':
            if event['text'] == 'ping':
                for client in group:
                    await group[client]({
                        'type': 'websocket.send',
                        'text': 'pong!'
                    })