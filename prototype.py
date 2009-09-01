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
        self.schedules = set()

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

        self.start_station.schedules.add(self)
        self.end_station.schedules.add(self)

    def __unicode__(self):
        return u'schedule from %s to %s on %s' % (self.start_station, self.end_station, self.line)

    def __hash__(self):
        return hash('%d%d%d' % (self.start_station.id, self.end_station.id, self.line.id))

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

def neighbours(station, stations):
    return [s for s in stations if s != station and [sc for sc in s.schedules if sc.start_station == station]]

def find_path(start_station, end_station, lines, stations, schedules):
    if not start_station or not end_station:
        print 'Stations do not exits'
        return []

    print 'Looking for path from %s to %s' % (unicode(start_station), unicode(end_station))

    # http://en.wikipedia.org/wiki/Dijkstra%27s_algorithm
    nodes = set(stations.values())

    from sys import maxint
    for n in nodes:
        n.distance = maxint
        n.previous = None

    start_station.distance = 0

    success = False
    while len(nodes):
        print '%d nodes left to scan' % len(nodes)

        current = None
        distance = maxint
        for n in nodes:
            print 'Analysing %s [%d] [%d]' % (unicode(n), n.distance, hash(n))
            if n.distance < distance:
                print 'New distance is %d for node %s' % (n.distance, unicode(n))
                current = n
                distance = n.distance

        if current == None:
            # Fail
            print 'FAIL'
            break

        nodes2 = nodes.copy()
        print 'Removing %s' % unicode(current)
        nodes.remove(current)
        print 'Intersect: %s' % ', '.join(unicode(n.name) for n in nodes2 ^ nodes)

        print 'Handling %s' % unicode(current)
        for s in neighbours(current, nodes):
            distance = current.distance + 2
            if distance < s.distance:
                print 'Relaxing neighbour %s to %d' % (unicode(s), distance)
                s.distance = distance
                s.previous = current
                if s == end_station:
                    print 'Found the end station %s' % unicode(s)
                    success = True
                    break
        if success:
            break

    path = []
    s = end_station
    while s != start_station:
        if not s.previous:
            break
        path.append(s)
        s = s.previous
    path.append(start_station)
    path.reverse()

    return path

def find_station(name, stations):
    for s in stations.values():
        if s.name == name:
            return s

    return None

def main():
    lines, stations, schedules = get_lines_from_db(DB_FILE)

    # for l in lines.values():
    #     print unicode(l)

    # for s in stations.values():
    #     print '%d: %s' % (s.id, unicode(s.name))

    # for s in schedules:
    #     print unicode(s)

    #print ', '.join([unicode(s.name) for s in neighbours(find_station('Commerce', stations), stations.values())])
    #print '\n'.join([unicode(s) for s in find_station('Commerce', stations).schedules])

    for s in find_path(find_station('Balard', stations), find_station('Oberkampf', stations), lines, stations, schedules):
        print unicode(s)

if __name__ == "__main__":
    try:
        main()
    except (KeyboardInterrupt):
        sys.exit()
