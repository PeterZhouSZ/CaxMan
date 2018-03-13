cd ~/CaxMan/

git pull

cd WP3_WS_IMATI/target/

rm -r *

cd ..

echo "Stopping and killing existing docker images ... "

docker stop $(docker ps -a | grep 'wp3_services' |awk '{print $1}') &>> /root/docker_log
docker rm $(docker ps -a | grep 'wp3_services' |awk '{print $1}') &>> /root/docker_log

echo "Rebuilding services"

./build.sh

printf "\nRestarting Glassfish in background.. "

printf "\nDOCKER_CONTAINER_ID = "
./run.sh

printf "\nServices will be available in a while ..."

printf "\n\nTo check Glassfish log, use :\n  docker logs DOCKER_CONTAINER_ID\n\n\n"
