#!/usr/bin/python
# -*- coding: utf-8 -*-

import sqlite3

DB_FILE = 'ratp.db'

class Line(object):
    def __init__(self, id, name):
        self.id = id
        self.name = name
        self.stations = set()

    def __unicode__(self):
        return u'line %s (%s stations)' % (self.name, len(self.stations))

    def __hash__(self):
        return self.id

class Station(object):
    def __init__(self, id, name):
        self.id = id
        self.name = name
        self.lines = set()

    def __unicode__(self):
        return u'station %s (%s)' % (self.name, ', '.join([l.name for l in self.lines]))

    def __hash__(self):
        return self.id

def get_lines_from_db(db_file):
    con = sqlite3.connect(db_file)
    c = con.cursor()

    lines = {}
    stations = {}

    c.execute('select id, name from lines')
    for l in c:
        id, name = l
        id = int(id)
        lines[id] = Line(id, name)

    c.execute('select id, name from stations')
    for s in c:
        id, name = s
        id = int(id)
        stations[id] = Station(id, name)

    c.execute('select start_station_id, end_station_id, line_id from schedules')
    for s in c:
        start_station_id, end_station_id, line_id = s
        start_station_id = int(start_station_id)
        end_station_id = int(end_station_id)
        line_id = int(line_id)
        ss = stations[start_station_id]
        es = stations[end_station_id]
        l = lines[line_id]
        ss.lines.add(l)
        es.lines.add(l)
        l.stations.add(ss)
        l.stations.add(es)

    return lines, stations

lines, stations = get_lines_from_db(DB_FILE)

for l in lines.values():
    print unicode(l)

for s in stations.values():
    print unicode(s)
