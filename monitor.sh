top -pid $(ps aux | grep QIce | awk {'print $2'} | sort -rn | tail -1)
