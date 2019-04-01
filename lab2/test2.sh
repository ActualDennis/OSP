find $1 ! -empty -size +1c -size -100000c  -type f -exec md5sum {} + | sort | uniq -w32 -dD


