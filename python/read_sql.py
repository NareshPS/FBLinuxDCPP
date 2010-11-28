
import cookielib
import os
import ConfigParser
from sqlite3 import dbapi2

FIREFOXPATH = os.environ['HOME']+'/.mozilla/firefox'
INIPROFILEPATH = os.path.join(FIREFOXPATH, 'profiles.ini')
COOKIEFILE = "cookies.sqlite"
TEMPFILE = '.cookies.sqlite'

#print FIREFOXPATH
#print INIPROFILEPATH
#print TEMPFILE

if not os.path.exists(FIREFOXPATH) or  not os.path.exists(INIPROFILEPATH):
    raise Exception, "Firefox path does not exist"

def getProfiles():
    parser = ConfigParser.ConfigParser()
    parser.read(INIPROFILEPATH)
    index = 0
    profiles =0
    try:
        while 1:
            path = parser.get('Profile%d' % index, 'Path')
            name = parser.get('Profile%d' % index, 'Name')
            if name == 'default':
                profiles = os.path.join(FIREFOXPATH, path)
                break 
            #print path 
            #print name
            index+=1
    except ConfigParser.NoSectionError, err:
        print err
      
    finally:
        return profiles


def sqlite2cookie(filename):
    from cStringIO import StringIO
    from pysqlite2 import dbapi2 as sqlite
    rand = 0 
    con = sqlite.connect(filename)
    cur = con.cursor()
    cur.execute("select host, path, isSecure, expiry, name, value from moz_cookies")
    symbol = 'naresh.dyndns-at-home.com'
    #t = (symbol,)
    #c.execute('select * from stocks where symbol=?', t)
   # cur.execute("select host, path, isSecure, expiry, name, value from moz_cookies where host")
 
    cur.execute("select host, path, isSecure, expiry, name, value from moz_cookies where host = '%s' " %symbol)
 
    ftstr = ["FALSE","TRUE"]
 
    #s = StringIO()
    for item in cur.fetchall():
    #    s.write("%s\t%s\t%s\t%s\t%s\t%s\t%s\n" % (
     #       item[0], ftstr[item[0].startswith('.')], item[1],
      #      ftstr[item[2]], item[3], item[4], item[5]))
        if item[4] == 'randomToken':
            rand = item[5]
    #print s.getvalue()  
    #s.seek(0)
    return rand


def read_sql():
    c = 0
    strC = '-1'
    c = sqlite2cookie(TEMPFILE);
    if c != 0 :
        strC    = str(c)
    return strC

def getCookieData():
    profiles = getProfiles();    
    print profiles
    path = 0
    if profiles == 0:
        return '-1'
    path = os.path.join(profiles,COOKIEFILE)
    print path
    if not os.path.exists(path):
        return '-1'
    os.system ("cp %s %s" % (path,TEMPFILE))
    res =read_sql();   
    print res 
    os.system ("rm %s " % (TEMPFILE))
    return res;

if __name__ == '__main__':
    getCookieData();

