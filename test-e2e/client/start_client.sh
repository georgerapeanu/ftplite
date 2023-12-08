set -e && cd test-e2e/client && sudo chown ftplite-server client* && sudo chgrp ftplite-server client* && sleep 10 && ./ftp_commands.sh | valgrind ../../target/bin/client localhost 27015 && sleep 5 && echo "Showing diff" 1>&2 &&
set -e && 
for i in `ls`; do 
  if [ -x $i ] ; then
    :
  else 
    sudo diff $i ../server/$i
  fi
done

