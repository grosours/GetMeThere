#!/usr/bin/python
# -*- coding: utf-8 -*-

import urllib2
from urllib import urlencode
from xml.dom import minidom
import sqlite3
import sys

DB_FILE = 'ratp.db'
GMAPS_KEY = ''
GMAPS_URL = 'http://maps.google.com/maps/geo'

def resolve(location):
    global GMAPS_KEY, GMAPS_URL

    url = '%s?%s' % (GMAPS_URL, urlencode((
        ('q', location),
        ('key', GMAPS_KEY),
        ('output', 'xml'),
        ('oe', 'utf8'),
        ('sensor', 'false'),
        ('gl', '.fr'),
    )))

    request = urllib2.Request(url)
    try:
        response = urllib2.urlopen(request)
    except urllib2.HTTPError, e:
        print 'HTTP status error %s on request (%s)' % (e.code, e.read())
        return False
    except urllib2.URLError, e:
        print 'Could not reach the server (%s)' % e.reason
        return False

    data = response.read()
    doc = minidom.parseString(data)
    accuracy = doc.getElementsByTagName("AddressDetails")[0].attributes['Accuracy'].value
    coordinates = doc.getElementsByTagName("coordinates")[0].childNodes[0].data

    return accuracy, coordinates


def main():
    global DB_FILE

    con = sqlite3.connect(DB_FILE)
    c = con.cursor()
    c.execute('select name from stations')
    for s in c:
        n, = s
        accuracy, coordinates = resolve('%s, Paris, France' % n.encode('utf-8'))
        print u'%s: %s (%s)' % (n, coordinates, accuracy)

if __name__ == "__main__":
    try:
        main()
    except (KeyboardInterrupt):
        sys.exit()
