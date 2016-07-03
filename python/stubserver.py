#!/usr/bin/python
import BaseHTTPServer, SocketServer, threading

class Handler(BaseHTTPServer.BaseHTTPRequestHandler):
       template = u"""OK"""
       server_version = 'HTTPNull Python Edition - Sam Machin'
       protocol_version = 'HTTP/1.1'

       def do_HEAD(self):
          self.send_response(200)
          self.send_header("content-type", "text/plain;charset=utf-8")
          self.end_headers()

       def do_GET(self):
          self.do_HEAD()
          self.wfile.write(self.template)

       def do_POST(self):
          length = int(self.headers.getheader('content-length'))
          self.rfile.read(length) # Why Does this line break persistance!
          self.send_response(202)
          self.send_header("content-type", "text/plain;charset=utf-8")
          self.send_header("content-length", "2")
          self.end_headers()
          self.wfile.write(self.template)


class threadedServer(SocketServer.ThreadingMixIn, BaseHTTPServer.HTTPServer):
        pass

if __name__ == '__main__':
       server = threadedServer(("172.25.60.136", 9000), Handler)
       server.pages = {}
       server.serve_forever()



