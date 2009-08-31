#!/usr/bin/python
# -*- coding: utf-8 -*-

import urllib2
import HTMLParser
import re
import sys
import sqlite3
import os, os.path

class Line(object):
    def __init__(self, name, stations, schedule):
        self.name = name
        self.stations = stations
        self.schedule = schedule

    def to_database(self, c):
        stations = {}

        c.execute('insert into lines(name) values (?)', (self.name,))
        c.execute('select id from lines where name=?', (self.name,))
        self.id = c.fetchone()[0]

        for s in self.stations:
            try:
                c.execute('insert into stations(name) values (?)', (s,))
            except sqlite3.IntegrityError:
                pass
            c.execute('select id from stations where name=?', (s,))
            stations[s] = c.fetchone()[0]

        for s, e in self.schedule:
            c.execute('insert into schedules(start_station_id, end_station_id, line_id) values (?, ?, ?)', (stations[s], stations[e], self.id,))

    def __str__(self):
        return self.name.encode('utf-8')

    def __hash__(self):
        return hash(self.name)

class LinePageParser(HTMLParser.HTMLParser):
    line_url_expr = re.compile('/orienter/horaires_metro(?P<name>\w+)\.php')

    stations_replace = (
        (u'', u'-'),
        (u'Sèvres -Babylone', u'Sèvres - Babylone'),
        (u'Reuilly-Diderot', u'Reuilly - Diderot'),
        (u'Chaussée d\'Antin - La Fayette', u'Chaussée d\'Antin'),
        (u'Chaussée d\'Antin', u'Chaussée d\'Antin - La Fayette'),
    )

    def __init__(self):
        HTMLParser.HTMLParser.__init__(self)

        self.reading_station_name = False
        self.reading_own_line_name = False

        self.current_station = None
        self.stations = set()
        self.schedule = []
        self.line_name = u''

        self.lines = set()

    def get_attr_value(self, attrs, name):
        for l_name, l_value in attrs:
            if l_name == name:
                return l_value
        return None

    def handle_starttag(self, tag, attrs):
        if tag == 'font' and self.get_attr_value(attrs, 'color') == '#999999':
            self.reading_own_line_name = True
        elif tag == 'a':
            url = self.get_attr_value(attrs, 'href')
            e = self.line_url_expr.match(url)
            if e:
                self.lines.add((e.group('name'), 'http://www.ratp.info%s' % url))
        elif tag == 'td':
            headers = self.get_attr_value(attrs, 'headers')
            if headers == 'station':
                self.reading_station_name = True

    def handle_data(self, data):
        if self.reading_own_line_name:
            data = unicode(data.decode('iso-8859-1'))
            if data.endswith('*'): data = data[:-1]
            self.line_name = data
            self.reading_own_line_name = False
        elif self.reading_station_name:
            data = unicode(data.decode('iso-8859-1'))
            if data.endswith('*'): data = data[:-1]
            for w, r in self.stations_replace:
                data = data.replace(w, r)
            if self.current_station:
                self.schedule.append((self.current_station, data))
                self.schedule.append((data, self.current_station))
            self.current_station = data
            self.stations.add(data)
            self.reading_station_name = False

def get_lines(base_url, verbose=False):
    lines = set()
    urls = set([base_url])
    first_iteration = True

    while len(urls) != 0:
        url = urls.pop()

        try:
            if verbose: print 'Getting data from the server [%s]' % url
            data = urllib2.urlopen(url)
        except urllib2.HTTPError, e:
            if verbose: print 'HTTP status error %s' % e.code
            return None
        except urllib2.URLError:
            if verbose: print 'Could not reach the server'
            return None
        except ValueError:
            if verbose: print 'Invalid url'
            return None

        if verbose: print 'Parsing lines'
        p = LinePageParser()
        p.feed(data.read())

        if first_iteration:
            urls = set()
            if verbose: print 'Got %d lines:' % len(p.lines)
            i = 0
            for l, u in p.lines:
                # i += 1
                # if i > 1: break
                urls.add(u)
                if verbose: print ' * %s [%s]' % (l, u)
            urls.remove(url)

            first_iteration = False

        if verbose:
            print 'Known stations:'
            for s in p.stations:
                print ' * %s' % s
            print 'Known schedule:'
            for s, t in p.schedule:
                print ' * %s -> %s' % (s, t)

        lines.add(Line(p.line_name, p.stations, p.schedule))

    return lines

def print_lines(base_url, verbose=False):
    lines = get_lines(base_url, verbose)
    stations = set()

    if lines == None:
        if verbose: print 'Could not list lines, or no lines available'
        return False

    db_file = 'ratp.db'
    if os.path.exists(db_file):
        os.remove(db_file)

    con = sqlite3.connect(db_file)
    c = con.cursor()
    c.execute('create table lines     (id integer primary key, name varchar unique)')
    c.execute('create table stations  (id integer primary key, name varchar unique)')
    c.execute('create table schedules (start_station_id integer, end_station_id integer, line_id integer, unique(start_station_id, end_station_id, line_id))')

    print 'Got %d lines' % len(lines)
    for l in lines:
        print ' * Line %s' % l
        stations.update(l.stations)
        l.to_database(c)

    print 'Got %d stations' % len(stations)
    sorted_stations = list(stations)
    sorted_stations.sort()
    for s in sorted_stations:
        print ' * %s' % s

    con.commit()
    con.close()

    return True

def main():
    url = 'http://www.ratp.info/orienter/horaires_metro1.php'
    print_lines(url, False)

if __name__ == "__main__":
    try:
        main()
    except (KeyboardInterrupt):
        sys.exit()
