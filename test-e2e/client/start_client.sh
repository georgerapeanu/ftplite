set -e && cd test-e2e/client && sudo chown ftplite-server client* && sudo chgrp ftplite-server client* && sleep 10 && ./ftp_commands.sh | valgrind ../../target/bin/client localhost 27015
