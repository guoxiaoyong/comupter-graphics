#!/usr/bin/python2

INFO = """
\tguosimple: a simple HTTP proxy program
\tAuthor: Guo Xiaoyong 
\tEmail:  guo.xiaoyong@gmail.com
"""

InAddress  = '127.0.0.1'
ListenPort = 8080
ProxyURL   = 'http://www.guoxiaoyong.net/guosimple.php'
UserAgent  = 'GuoSimple Proxy Server'

print INFO
print '\tProxyURL: ', ProxyURL , '\n'

#############################################################

import SocketServer
import urllib
import re
import sys

class ProxyHandler(SocketServer.StreamRequestHandler):

    def handle(self):
        Has_Content     = False;
        Content_Length  = 0;
        req      = '';
        req_len  = 0;
        read_len = 4096;
        # read message 
        try:
            while 1:
                # Most common case when browsing, no Content, just HTTP Header 
                if not Has_Content:
                    line = self.rfile.readline(read_len)
                    if line == '':                                 
                        self.SendRequest(req) # send it anyway
                        print 'The end of the request header hasn\'t been found'
                        return
                    #if line[0:17].lower() == 'proxy-connection:':
                    #    req += "Connection: close\r\n"
                    #    continue
                    req += line
                    # Find the value of Content-Length,
                    # used in cases such as uploading files or login a website
                    if not Content_Length:
                        REObject = re.compile('^Content-Length: (\d+)', re.I).search(line)
                        if REObject is not None:
                            Content_Length = int(REObject.group(1))
                            continue
                    
                    if line == "\015\012" or line == "\012":
                        if not Content_Length:
                            self.SendRequest(req)
                            return
                        else:
                            Has_Content = True
                            read_len = Content_Length
                else:
                    buf = self.rfile.read(read_len)
                    req += buf
                    req_len += len(buf)
                    read_len = Content_Length - req_len
                    # if req_len < Content_Length
                    # continue read until req_len == Content_Length
                    if req_len >= Content_Length:
                        self.SendRequest(req)
                        return

        except IOError:
            print 'IOError!!!'    
            return



    def SendRequest(self, req):
        #print req
        if req == '': return
        #open URL
        HTTP_URLopener = urllib.FancyURLopener({})
        HTTP_URLopener.addheaders = [('User-Agent', UserAgent)]
        FileObject = HTTP_URLopener.open(ProxyURL, urllib.urlencode({'req': req}))
        #receive data
        while 1:
            ByteStream = FileObject.read(2048)
            if ByteStream == '': break
            self.wfile.write(ByteStream)
        #close file object    
        self.wfile.close()


ProxyServer = SocketServer.ThreadingTCPServer((InAddress, ListenPort), ProxyHandler)

print 'GuoSimple proxy server is running...'
ProxyServer.serve_forever()


