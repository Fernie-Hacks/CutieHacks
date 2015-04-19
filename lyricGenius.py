#!/usr/bin/env python
# -*- coding: utf-8 -*-
import os
import sys
import codecs
from mutagen.mp3 import MP3
from mutagen.id3 import ID3NoHeaderError
from mutagen.id3 import ID3, TIT2, TALB, TPE1, TPE2, COMM, USLT, TCOM, TCON, TDRC

TEXT_ENCODING = 'utf8'
SET_OTHER_ID3_TAGS = False

# Specify the name of the track 
if (len(sys.argv) < 3):
    print'\tERROR: Did not provide the name of a song & path'
    sys.exit()
else:
    song = os.path.abspath(os.path.dirname(sys.argv[1]))
    fname = os.path.abspath(os.path.dirname(sys.argv[2]))
    
lyrfname = fname[:-3] + 'txt'

if not os.path.exists(lyrfname):
    print '\tERROR: No lyrics file found:', lyrfname, '...skipping'
    continue
else:
    lyrics = open(lyrfname).read().strip()

#Find the right encoding
for enc in ('utf8','iso-8859-1','iso-8859-15','cp1252','cp1251','latin1'):
    try:
        lyrics = lyrics.decode(enc)
        print enc,
        break
    except:
        pass
        
# create ID3 tag if not exists
try: 
    tags = ID3(fname)
except ID3NoHeaderError:
    print "Adding ID3 header;",
    tags = ID3()

#Remove old unsychronized lyrics
if len(tags.getall(u"USLT::'en'")) != 0:
    print "Removing Lyrics."
    tags.delall(u"USLT::'en'")
    tags.save(fname)
            
tags[u"USLT::'eng'"] = (USLT(encoding=3, lang=u'eng', desc=u'desc', text=lyrics))
print 'Added USLT frame to', fname        
tags.save(fname)

print 'Done'
