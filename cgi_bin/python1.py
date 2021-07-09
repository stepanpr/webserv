#!/usr/bin/python

import sys
import os


print "CONTENT_LENGTH = " + os.environ["CONTENT_LENGTH"]
print "GATEWAY_INTERFACE = " + os.environ["GATEWAY_INTERFACE"]
print "REMOTE_ADDR = " + os.environ["REMOTE_ADDR"]
print "REQUEST_METHOD = " + os.environ["REQUEST_METHOD"]
print "SERVER_NAME = " + os.environ["SERVER_NAME"]
print "SERVER_PORT = " + os.environ["SERVER_PORT"]
print "gogogogogogoogogogogogogogogogogogo"
body = sys.stdin.read( int(os.environ["CONTENT_LENGTH"]) )
print body