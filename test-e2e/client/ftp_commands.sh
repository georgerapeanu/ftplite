echo "ftplite-server"
echo "ftplite1"
# TYPE A, ACTIVE
echo "STOR client_ascii_active"
echo "TYPE I"
# TYPE I, ACTIVE
echo "STOR client_image_active"
echo "PASV"
# TYPE I, PASSIVE
echo "STOR client_image_passive"
echo "TYPE A"
# TYPE A, PASSIVE
echo "STOR client_ascii_passive"
echo "TYPE A"
### Server side
# TYPE A, PASSIVE
echo "RETR server_ascii_passive"
echo "TYPE I"
# TYPE I, PASSIVE
echo "RETR server_image_passive"
echo "PORT 12345"
# TYPE I, ACTIVE
echo "RETR server_image_active"
# TYPE A, ACTIVE
echo "RETR server_ascii_active"

