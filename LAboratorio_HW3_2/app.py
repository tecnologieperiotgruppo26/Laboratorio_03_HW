import cherrypy
import json

class Log(object):
    #@cherrypy.expose
    exposed = True
    requests = []

    def GET(self, *uri, **params):
        return json.dumps(self.requests)
    
    def POST(self, *uri, **params):
        cherrypy.response.headers['Content-Type'] = 'application/json'
        request_body = cherrypy.request.body.read()
        request = json.loads(request_body)
        self.requests.append(request)
        return "Request saved!"


if __name__ == "__main__":
    # standard configuration to serve the url "localhost:8080"

    conf = {
        '/': {
            'request.dispatch': cherrypy.dispatch.MethodDispatcher(),
            'tools.encode.text_only': False,
        }
    }

    cherrypy.tree.mount(Log(), '/', conf)
    cherrypy.config.update({'server.socket_host': '192.168.1.69'})
    cherrypy.config.update({'server.socket_port': 9090})
    cherrypy.engine.start()
    cherrypy.engine.block()



