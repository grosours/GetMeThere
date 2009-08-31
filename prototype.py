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
        return u'line %s' % self.name

    def __hash__(self):
        return self.id

class Station(object):
    def __init__(self, id, name):
        self.id = id
        self.name = name
        self.lines = set()

    def __unicode__(self):
        return u'station %s' % self.name

    def __hash__(self):
        return self.id

class Schedule(object):
    def __init__(self, start_station, end_station, line):
        self.start_station = start_station
        self.end_station = end_station
        self.line = line

        self.line.stations.add(self.start_station)
        self.line.stations.add(self.end_station)
        self.start_station.lines.add(self.line)
        self.end_station.lines.add(self.line)

    def __unicode__(self):
        return u'schedule from %s to %s on %s' % (self.start_station, self.end_station, self.line)

def get_lines_from_db(db_file):
    con = sqlite3.connect(db_file)
    c = con.cursor()

    lines = {}
    stations = {}
    schedules = []

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
        sc = Schedule(stations[int(start_station_id)], stations[int(end_station_id)], lines[int(line_id)])
        schedules.append(sc)

    return lines, stations, schedules


def main():
    lines, stations, schedules = get_lines_from_db(DB_FILE)

    for l in lines.values():
        print unicode(l)

    for s in stations.values():
        print unicode(s)

    for s in schedules:
        print unicode(s)

if __name__ == "__main__":
    try:
        main()
    except (KeyboardInterrupt):
        sys.exit()
