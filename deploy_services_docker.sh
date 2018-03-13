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

printf "\nDOCKER_CONTAINER_ID = "
./run.sh

printf "\nRestarting Glassfish in background ..."

until $(curl --output /dev/null --silent --head --fail https://caxman.clesgo.net/imati/wp3/orientation_optimization?wsdl); 
do 
	printf '.';
	sleep 1;
done

printf "\n\n Deployed Web services! \n\n"

#printf "\n\nTo check Glassfish log, use :\n  docker logs DOCKER_CONTAINER_ID\n\n\n"
