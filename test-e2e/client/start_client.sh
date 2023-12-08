set -e && cd test-e2e/client && sudo chown ftplite-server client* && sudo chgrp ftplite-server client* && sleep 10 && ./ftp_commands.sh | valgrind ../../target/bin/client localhost 27015 && sleep 5 && echo "Showing diff" 1>&2 &&
for i in `ls`; do 
  if [ -x $i ] ; then
    :
  else 
    diff_msg=$(sudo diff $i ../server/$i);
    if [ -n "${diff_msg}%?" ] ; then 
      echo 1>&2 "$diff_msg"
      echo 1>&2 "Diff failed for $i"
      exit 1 
    fi
  fi
done

