find /home/actualdennis/Documents/testfolder ! -empty -size +1c -size -100000c  -type f -exec md5sum {} + | sort | uniq -w32 -dD
