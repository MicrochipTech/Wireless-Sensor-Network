#!/bin/bash
while [ : ]
do
    clear
	echo "Resetting!"
	sudo apachectl restart
	echo "all Done."
    sleep 5
done