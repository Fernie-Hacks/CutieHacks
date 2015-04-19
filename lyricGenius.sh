!/bin/bash

find ./ -iname "*.mp3" >> listOfSongs.txt
FILE="./listofSongs.txt"

while [[ -s $FILE ]]
do
    SONG="$(head -1 $FILE)"
    echo $SONG
    LYRICS="$(./a.out $SONG)"
    ./lyricGenius.py $FILE $SONG
    echo "$(tail -n +2 $FILE)" > $FILE
    rm $LYRICS
done


