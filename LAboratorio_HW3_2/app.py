import cherrypy
import json

class Log(object):
    #@cherrypy.expose
    exposed = True
    requests = []

    def GET(self, *uri, **params):
        return json.loads(self.requests)
    
    def POST(self, *uri, **params): 
        request_body = cherrypy.request.body.read()
        request = json.loads(request_body)
        self.requests.append(request)
        return "Request saved!"


if __name__ == "__main__":
    # standard configuration to serve the url "localhost:8080"

    conf = {
        '/': {
            'request.dispatch': cherrypy.dispatch.MethodDispatcher(),
        }
    }

    cherrypy.tree.mount(Log(), '/converter', conf)
    cherrypy.config.update({'server.socket_host': '127.0.0.1'})
    cherrypy.config.update({'server.socket_port': 8080})
    cherrypy.engine.start()
    cherrypy.engine.stop()



